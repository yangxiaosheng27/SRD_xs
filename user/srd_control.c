/*
 * 	FileName:	srd_control.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */
#include "SRD_Project.h"        // User's Funtions
#include <stdio.h>

void Init_SRD(void);
void Control_SRD_internal_loop(void);
void Control_SRD_external_loop(void);

void My_Init_Control(void);
void LOGIC_Control(void);
void SPEED_Control(void);
void TORQUE_Calculate(void);
void TORQUE_Distribution(void);
void TORQUE_Control(void);
void IGBT_Control(void);
void System_Identification(void);
void Adapt_Control(void);

#define SPEED_Expect 		500		// 1000 means 1000	r/min
#define SPEED_MAX			1500		// 1000 means 1000	r/min
#define SPEED_Kp 			3
#define SPEED_Ki			0.01
#define SPEED_Kd 			0

void Control_SRD_external_loop(void)
{
	Get_Speed();
	SPEED_Control();
	if(!SRM_FIRST_RUN)
		System_Identification();
	Adapt_Control();
}
void SPEED_Control(void)
{
	if(SPEED.Expect>SPEED.MAX)SPEED.Expect=SPEED.MAX;
	SPEED.Error = SPEED.Expect - SRM.Speed;
	SPEED.Integral += SPEED.Error;
	if(SPEED.Integral>SPEED.Integral_Max)SPEED.Integral = SPEED.Integral_Max;
	else if(SPEED.Integral<-SPEED.Integral_Max)SPEED.Integral = -SPEED.Integral_Max;
	TORQUE.Expect = SPEED.Kp * SPEED.Error + SPEED.Ki * SPEED.Integral;
	if(SRM_FIRST_RUN)	TORQUE.Expect = TORQUE.MAX ;
	if(TORQUE.Expect > TORQUE.MAX) TORQUE.Expect= TORQUE.MAX;
}
float a1=0,a2=0,b0=0,b1=0;// A*y=(z^-1)*Bu, A=1+a1(z^-1)+a2(z^-2), B=b0+b1(z^-1)
double am1=1.987,am2=0.878,f1,g0,g1,g2;

void Adapt_Control(void)
{
	static double u=0,u_1=0,u_2=0,dem,r=0,y=0,y_1=0,y_2=0;
	double a2_b0 = a2*b0;
	double b0_b0 = b0*b0;
	double b1_b1 = b1*b1;
	double b1_b1_b0 = b1_b1*b0;

	r = SPEED.Expect;
	y = SRM.Speed;

	dem = (a1*b0_b0*b1+a1*b1_b1_b0-b1_b1_b0-a2_b0*b0_b0-b1_b1);
	if(dem==0)dem = 1.4E-45;
	f1 = (am2*b1_b1_b0-a2_b0*b1_b1+a1*b1_b1_b0-a2_b0*b0*b1-b1*b1_b1*am1+a1*b1_b1-b1_b1)/dem;

	dem = b0;
	if(dem==0)dem = 1.4E-45;
	g0 = (am1-f1-a1+1)/dem;

	dem = b1_b1;
	if(dem==0)dem = 1.4E-45;
	g1 = (a2*b1+a1*b1*f1-a2*b0*f1)/dem;

	dem = b1;
	if(dem==0)dem = 1.4E-45;
	g2 = (a2*f1)/dem;

	u = (1-f1)*u_1 + f1*u_2 + (g0+g1+g2)*r - g0*y -g1*y_1 - g2*y_2;
	if(u>1000)u=1000;
	else if(u<0)u=0;

	u_2 = u_1;
	u_1 = u;
	y_2 = y_1;
	y_1 = y;
}


void System_Identification(void)
{
	static float y=0,y_1=0,y_2=0,u=0,u_1=0,u_2=0,first_run=1,p=0.98;
	static Mat fai_now,G_now,theta_now,P_now,theta_last,P_last;
	static Mat temp1,temp2,temp3,temp4;
	static float fai_element[4];
	static float theta_0[]={
			1,
			1,
			1,
			1
	};
	static float P_0[]={
			1000,	0,	0,	0,
			0,	1000,	0,	0,
			0,	0,	1000,	0,
			0,	0,	0,	1000
	};

	if(first_run)
	{
		first_run = 0;
		MatCreate(&fai_now, 4, 1);
		MatCreate(&theta_now, 4, 1);
		MatCreate(&P_now, 4, 4);
		MatCreate(&G_now, 4, 1);
		MatCreate(&theta_last, 4, 1);
		MatCreate(&P_last, 4, 4);
		MatSetVal(&theta_last, theta_0);
		MatSetVal(&P_last, P_0);
	}

	// update u,y
	u = TORQUE.Expect;
	y = SRM.Speed;

/*	//for test
  	u+=1;
	count+=1;
	if(count>1000)while(1);
	if(u>10)u=5;
	y=1.81*y_1-0.8187*y_2+0.004377*u_1+0.004679*u_2;*/

	// update fai_now
	fai_element[0] = -y_1;
	fai_element[1] = -y_2;
	fai_element[2] = u_1;
	fai_element[3] = u_2;
	MatSetVal(&fai_now,fai_element);

	// update G_now
	MatCreate(&temp1, 4, 1);
	MatMul(&P_last, &fai_now, &temp1);
	MatCreate(&temp2, 1, 4);
	MatTrans(&fai_now, &temp2);	// temp2=fai_now' will be used later
	MatCreate(&temp3, 1, 4);
	MatMul(&temp2, &P_last, &temp3);
	MatCreate(&temp4, 1, 1);
	MatMul(&temp3, &fai_now, &temp4);
	MatDiv(&temp1, **temp4.element+p, &G_now);
	MatDelete(&temp1);
	MatDelete(&temp3);
	MatDelete(&temp4);

	// update P_now
	MatCreate(&temp1, 1, 4);
	MatMul(&temp2, &P_last, &temp1);
	MatCreate(&temp3, 4, 4);
	MatMul(&G_now, &temp1, &temp3);
	MatCreate(&temp4, 4, 4);
	MatSub(&P_last, &temp3, &temp4);
	MatDiv(&temp4, p, &P_now);
	MatDelete(&temp1);
	MatDelete(&temp3);
	MatDelete(&temp4);

	// update theta_now
	MatCreate(&temp1, 1, 1);
	MatMul(&temp2, &theta_last, &temp1);
	MatCreate(&temp3, 4, 1);
	MatAmplify(&G_now, y-**(temp1.element), &temp3);
	MatAdd(&theta_last, &temp3, &theta_now);
	MatDelete(&temp1);
	MatDelete(&temp2);
	MatDelete(&temp3);

	// save for next time
	MatCopy(&P_now, &P_last);
	MatCopy(&theta_now, &theta_last);
	y_2 = y_1;
	y_1 = y;
	u_2 = u_1;
	u_1 = u;

	// A*y=(z^-1)*Bu, A=1+a1(z^-1)+a2(z^-2), B=b0+b1(z^-1)
	a1 = **(theta_now.element+0);
	a2 = **(theta_now.element+1);
	b0 = **(theta_now.element+2);
	b1 = **(theta_now.element+3);

}

#define SPEED_MAX 			1500		// 1500 means 1500r/min
#define TORQUE_MAX 			5000		// 1000 means 1.000 N*m
#define TORQUE_Kp			100
#define TORQUE_Ki			10
#define TORQUE_Hysteresis	10			// used in Theta_ov
#define PWM_MAX				1000		// 1000	means 100% duty
#define Theta_ov			20			// 20	means 2 deg

struct LOGIC_STRUCT			LOGIC;
struct SPEED_STRUCT 		SPEED;
struct TORQUE_STRUCT 		TORQUE;
struct TORQUE_STRUCT 		TORQUE_AB;
struct TORQUE_STRUCT 		TORQUE_BC;
struct TORQUE_STRUCT 		TORQUE_CA;
struct PWM_STRUCT			PWM;

void Init_SRD(void)
{
	My_Init_GPIO();
	DRV_UP;					//H is Disaable
	BR_UP;					//H is Disaable
	SS_UP;					//H is Disaable
	FAN_UP;					//H is Disaable
	My_Init_PWM();
	My_Init_Control();
	My_Init_Sensor();
	My_Init_Error();

	printf("Hi,YangXiaoSheng!\nPress \"Enter\" to run the SRM.\n");
	getchar();

#ifdef	ENABLE_CONTROL
	printf("SRM is Runing.\n");
#else
	printf("SRM is NOT Runing.\n");
#endif

	SS_DN;					//L is Enable
	FAN_DN;					//L is Enable

	DELAY_US(1000000L);
	My_Init_ADC();
	Error_Checking();

#ifdef	ENABLE_CONTROL
	DRV_DN;					//DRV_DN will enable the IGBT control signal. Be careful to use it.
#else
	DRV_UP;
#endif

	My_Init_Cputimer();
	My_Init_EQEP();
}

void Control_SRD_internal_loop(void)
{
	Get_Sensor();
	LOGIC_Control();
	TORQUE_Calculate();
	TORQUE_Distribution();
	TORQUE_Control();
	Error_Checking();
	IGBT_Control();
}

void My_Init_Control(void)
{
	//Init Control Parameters
	LOGIC.Turn 			= 0;
	LOGIC.State 		= -1; //first run
	LOGIC.Count 		= 0;
	PWM.MAX				= PWM_MAX;
	PWM.COM				= 0;
	SPEED.Expect 		= SPEED_Expect;
	SPEED.MAX 			= SPEED_MAX;
	SPEED.Kp 			= SPEED_Kp;
	SPEED.Ki 			= SPEED_Ki;
	SPEED.Kd 			= SPEED_Kd;
	SPEED.Integral		= 0;
	SPEED.Integral_Max	= TORQUE_MAX*100L;	// not use
	SPEED.Error			= 0;
	TORQUE.MAX 			= TORQUE_MAX;
	TORQUE.Hysteresis	= TORQUE_Hysteresis;
	TORQUE_AB.MAX 		= TORQUE_MAX;
	TORQUE_AB.Kp		= TORQUE_Kp;
	TORQUE_AB.Ki		= TORQUE_Ki;
	TORQUE_AB.Error		= 0;
	TORQUE_AB.Integral	= 0;
	TORQUE_AB.Integral_Max	= PWM_MAX / 2 * 1000L * TORQUE_Ki;
	TORQUE_BC.MAX 		= TORQUE_MAX;
	TORQUE_BC.Kp		= TORQUE_Kp;
	TORQUE_BC.Ki		= TORQUE_Ki;
	TORQUE_BC.Error		= 0;
	TORQUE_BC.Integral	= 0;
	TORQUE_BC.Integral_Max	= PWM_MAX / TORQUE_Ki;
	TORQUE_CA.MAX 		= TORQUE_MAX;
	TORQUE_CA.Kp		= TORQUE_Kp;
	TORQUE_CA.Ki		= TORQUE_Ki;
	TORQUE_CA.Error		= 0;
	TORQUE_CA.Integral	= 0;
	TORQUE_CA.Integral_Max	= PWM_MAX / TORQUE_Ki;
}

void LOGIC_Control(void)
{
	LOGIC.State_1 = LOGIC.State;

	if(SRM_FIRST_RUN)	//first run
	{
		SRM.Phase = 140;	// to calculate T in first run
		switch(NOW_state)
		{
		case 1:	LOGIC.State = 0; LOGIC.Count = 0;	break;
		case 2:	LOGIC.State = 0; LOGIC.Count = 1;	break;
		case 3:	LOGIC.State = 1; LOGIC.Count = 0;	break;
		case 4:	LOGIC.State = 1; LOGIC.Count = 1;	break;
		case 5:	LOGIC.State = 2; LOGIC.Count = 0;	break;
		case 0:	LOGIC.State = 2; LOGIC.Count = 1;	break;
		default:DRV_UP;LOGIC.State = -2; //error flag
		}
	}
	else if(SRM_Direction>0)	// no first run but state change
	{
		if(++LOGIC.Count>1)
		{
			LOGIC.Count = 0;
			if(++LOGIC.State>=6) LOGIC.State -= 6;
			SRM.Phase_Bias = 3600 - SRM.Angle;	//what the phase bias should be
			SRM.Phase = 0;
		}
		switch(NOW_state)
		{
		case 1:
		case 2:	if(LOGIC.State!=0 && LOGIC.State!=3) LOGIC.State = -3; break; //error flag
		case 3:
		case 4:	if(LOGIC.State!=1 && LOGIC.State!=4) LOGIC.State = -4; break; //error flag
		case 5:
		case 0:	if(LOGIC.State!=2 && LOGIC.State!=5) LOGIC.State = -5; break; //error flag
		default:DRV_UP;LOGIC.State = -2; //error flag
		}
	}
	else if(SRM_Direction<0)
	{
			if(--LOGIC.Count<0)
			{
				LOGIC.Count = 1;
				if(--LOGIC.State<0) LOGIC.State += 6;
			}
	}

	if(LOGIC.State_1 == LOGIC.State)	LOGIC.Turn = 0;
	else								LOGIC.Turn = 1;
}

int32 TORQUE_Model[6][30]={
		{   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{  60,   65,   90,   80,   80,  100,  100,   80,  120,  150,  120,  100,  110,   90,   40,   20,   10,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{ 260,  300,  365,  365,  370,  390,  380,  385,  410,  510,  410,  410,  430,  440,  210,   60,   40,   20,   10,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{ 620,  730,  830,  850,  870,  880,  880,  890,  890,  980,  910,  920,  975,  830,  420,  170,  130,   70,   50,   20,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{1210, 1370, 1470, 1500, 1520, 1540, 1530, 1560, 1550, 1580, 1530, 1560, 1660, 1690,  670,  340,  240,  160,  120,   50,   20,    0,    0,    0,    0,    0,    0,    0,    0,    0},
		{1810, 1990, 2200, 2280, 2360, 2370, 2350, 2370, 2330, 2350, 2320, 2280, 2340, 2470, 1980,  900,  500,  280,  220,  110,   40,    0,    0,    0,    0,    0,    0,    0,    0,    0}
};
#define GET_TORQUE ((TORQUE_Model[I_index+1][theta_index+1]*theta_ratio + TORQUE_Model[I_index+1][theta_index]*(100-theta_ratio))*I_ratio + (TORQUE_Model[I_index][theta_index+1]*theta_ratio + TORQUE_Model[I_index][theta_index]*(100-theta_ratio))*(100-I_ratio))/10000
#define GET_TORQUE_MAX TORQUE_Model[5][theta_index+1]*theta_ratio/10*(I_abs*I_abs/1000)/25/100 + TORQUE_Model[5][theta_index]*(100-theta_ratio)/10*(I_abs*I_abs/1000)/25/100
void TORQUE_Calculate(void)
{
	int32 I_index,I_ratio,theta_index,theta_ratio,I_abs;
	int32 Ia_abs;
	int32 Ib_abs;
	int32 Ic_abs;
	// for test!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	static int32 TORQUE_AB_MAX=0,Ia_max=0;

	Ia_abs = CURRENT.Ia_abs;
	Ib_abs = CURRENT.Ib_abs;
	Ic_abs = CURRENT.Ic_abs;

	switch(LOGIC.State)
	{
	case  0:
	case  3:	theta_index 		= SRM.Phase/10;
				theta_ratio 		= SRM.Phase%10*10;			// 1 means 1%
				I_index 			= Ia_abs/100;
				I_ratio 			= Ia_abs%100;			// 1 means 1%
				I_abs				= Ia_abs;
				if(I_abs>=500)
					TORQUE_AB.Sample = GET_TORQUE_MAX;
				else
					TORQUE_AB.Sample = GET_TORQUE;
				theta_index			= SRM.Phase/10 + 15;
				I_index 			= Ic_abs/100;
				I_ratio 			= Ic_abs%100;			// 1 means 1%
				I_abs				= Ic_abs;
				if(I_abs>=500)
					TORQUE_BC.Sample = GET_TORQUE_MAX;
				else
					TORQUE_BC.Sample = GET_TORQUE;
				TORQUE_CA.Sample 	= 0;
				break;
	case  1:
	case  4:	theta_index 		= SRM.Phase/10 + 15;
				theta_ratio 		= SRM.Phase%10*10;			// 1 means 1%
				I_index 			= Ib_abs/100;
				I_ratio 			= Ib_abs%100;			// 1 means 1%
				I_abs				= Ib_abs;
				if(I_abs>=500)
					TORQUE_AB.Sample = GET_TORQUE_MAX;
				else
					TORQUE_AB.Sample = GET_TORQUE;
				TORQUE_BC.Sample 	= 0;
				theta_index 		= SRM.Phase/10;
				I_index 			= Ic_abs/100;
				I_ratio 			= Ic_abs%100;			// 1 means 1%
				I_abs				= Ic_abs;
				if(I_abs>=500)
					TORQUE_CA.Sample = GET_TORQUE_MAX;
				else
					TORQUE_CA.Sample = GET_TORQUE;
				break;
	case  2:
	case  5:	TORQUE_AB.Sample 	= 0;
				theta_index 		= SRM.Phase/10;
				theta_ratio 		= SRM.Phase%10*10;			// 1 means 1%
				I_index 			= Ib_abs/100;
				I_ratio 			= Ib_abs%100;			// 1 means 1%
				I_abs				= Ib_abs;
				if(I_abs>=500)
					TORQUE_BC.Sample = GET_TORQUE_MAX;
				else
					TORQUE_BC.Sample = GET_TORQUE;
				theta_index 		= SRM.Phase/10 + 15;
				I_index 			= Ia_abs/100;
				I_ratio 			= Ia_abs%100;			// 1 means 1%
				I_abs				= Ia_abs;
				if(I_abs>=500)
					TORQUE_CA.Sample = GET_TORQUE_MAX;
				else
					TORQUE_CA.Sample = GET_TORQUE;
				break;
	default:	DRV_UP;
	}
	//for test!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if(TORQUE_AB.Sample>TORQUE_AB_MAX)TORQUE_AB_MAX=TORQUE_AB.Sample;
	if(Ia_abs>Ia_max)Ia_max=Ia_abs;

}

void TORQUE_Distribution(void)
{
	if(SRM_FIRST_RUN)
	{
		TORQUE.Expect = 1500;
	}
	if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
	{
		TORQUE.NEXT = TORQUE.Expect*SRM.Phase/Theta_ov;
		TORQUE.LAST = TORQUE.Expect-TORQUE.NEXT;
		switch(LOGIC.State)
		{
		case  0:
		case  3:	TORQUE_AB.Expect = TORQUE.NEXT;
					TORQUE_BC.Expect = TORQUE.LAST;
					TORQUE_CA.Expect = 0;
					break;
		case  1:
		case  4:	TORQUE_AB.Expect = TORQUE.LAST;
					TORQUE_BC.Expect = 0;
					TORQUE_CA.Expect = TORQUE.NEXT;
					break;
		case  2:
		case  5:	TORQUE_AB.Expect = 0;
					TORQUE_BC.Expect = TORQUE.NEXT;
					TORQUE_CA.Expect = TORQUE.LAST;
					break;
		default:	DRV_UP;
		}
	}
	else
	{
		TORQUE.NEXT = TORQUE.LAST = 0;
		switch(LOGIC.State)
		{
		case  0:
		case  3:	TORQUE_AB.Expect = TORQUE.Expect;
					TORQUE_BC.Expect = 0;
					TORQUE_CA.Expect = 0;
					break;
		case  1:
		case  4:	TORQUE_AB.Expect = 0;
					TORQUE_BC.Expect = 0;
					TORQUE_CA.Expect = TORQUE.Expect;
					break;
		case  2:
		case  5:	TORQUE_AB.Expect = 0;
					TORQUE_BC.Expect = TORQUE.Expect;
					TORQUE_CA.Expect = 0;
					break;
		default:	DRV_UP;
		}
	}
}

void TORQUE_Control(void)
{
	if(LOGIC.Turn)
	{
		switch(LOGIC.State)
		{
		case  0:
		case  3:TORQUE_AB.Integral = 0;break;
		case  1:
		case  4:TORQUE_CA.Integral = 0;break;
		case  2:
		case  5:TORQUE_BC.Integral = 0;break;
		}
	}
	switch(LOGIC.State)
	{
	case  0:
	case  3:if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
			{
				TORQUE_AB.Error = TORQUE_AB.Expect - TORQUE_AB.Sample;
				TORQUE_AB.Integral += TORQUE_AB.Error;
				if(TORQUE_AB.Integral > TORQUE_AB.Integral_Max)	TORQUE_AB.Integral = TORQUE_AB.Integral_Max;
				else if(TORQUE_AB.Integral <0)	TORQUE_AB.Integral = 0;
				PWM.NEXT = TORQUE_AB.Kp * TORQUE_AB.Error/ 300 + TORQUE_AB.Ki * TORQUE_AB.Integral / 1000;
				if(PWM.NEXT > PWM.MAX)	PWM.NEXT = PWM.MAX;
				else if(PWM.NEXT <0)	PWM.NEXT = 0;

				TORQUE_BC.Error = TORQUE_BC.Expect - TORQUE_BC.Sample;
				TORQUE_BC.Integral += TORQUE_BC.Error;
				if(TORQUE_BC.Integral > TORQUE_BC.Integral_Max)	TORQUE_BC.Integral = TORQUE_BC.Integral_Max;
				else if(TORQUE_BC.Integral <0)	TORQUE_BC.Integral = 0;
				PWM.LAST = TORQUE_BC.Kp * TORQUE_BC.Error/ 300 + TORQUE_BC.Ki * TORQUE_BC.Integral / 1000;
				if(PWM.LAST > PWM.MAX)	PWM.LAST = PWM.MAX;
				else if(PWM.LAST < 0)	PWM.LAST = 0;

				if(TORQUE_BC.Error >= 0)PWM.COM = 1000;
				else if(TORQUE_BC.Error < TORQUE.Hysteresis)PWM.COM = 0;
			}
			else
			{
				TORQUE_AB.Error = TORQUE_AB.Expect - TORQUE_AB.Sample;
				TORQUE_AB.Integral += TORQUE_AB.Error;
				if(TORQUE_AB.Integral > TORQUE_AB.Integral_Max)	TORQUE_AB.Integral = TORQUE_AB.Integral_Max;
				else if(TORQUE_AB.Integral <0)	TORQUE_AB.Integral = 0;
				if(TORQUE_AB.Integral > TORQUE_AB.Integral_Max)TORQUE_AB.Integral = TORQUE_AB.Integral_Max;
				PWM.NOW = TORQUE_AB.Kp * TORQUE_AB.Error/ 300 + TORQUE_AB.Ki * TORQUE_AB.Integral / 1000;
				if(PWM.NOW > PWM.MAX)	PWM.NOW = PWM.MAX;
				else if(PWM.NOW < 0)	PWM.NOW = 0;
			}
			break;
	case  1:
	case  4:if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
			{
				TORQUE_CA.Error = TORQUE_CA.Expect - TORQUE_CA.Sample;
				TORQUE_CA.Integral += TORQUE_CA.Error;
				if(TORQUE_CA.Integral > TORQUE_CA.Integral_Max)	TORQUE_CA.Integral = TORQUE_CA.Integral_Max;
				else if(TORQUE_CA.Integral <0)	TORQUE_CA.Integral = 0;
				PWM.NEXT = TORQUE_CA.Kp * TORQUE_CA.Error/ 300 + TORQUE_CA.Ki * TORQUE_CA.Integral / 1000;
				if(PWM.NEXT > PWM.MAX)	PWM.NEXT = PWM.MAX;
				else if(PWM.NEXT < 0)	PWM.NEXT = 0;

				TORQUE_AB.Error = TORQUE_AB.Expect - TORQUE_AB.Sample;
				TORQUE_AB.Integral += TORQUE_AB.Error;
				if(TORQUE_AB.Integral > TORQUE_AB.Integral_Max)	TORQUE_AB.Integral = TORQUE_AB.Integral_Max;
				else if(TORQUE_AB.Integral <0)	TORQUE_AB.Integral = 0;
				PWM.LAST = TORQUE_AB.Kp * TORQUE_AB.Error/ 300 + TORQUE_AB.Ki * TORQUE_AB.Integral / 1000;
				if(PWM.LAST > PWM.MAX)	PWM.LAST = PWM.MAX;
				else if(PWM.LAST < 0)	PWM.LAST = 0;

				if(TORQUE_AB.Error >= 0)PWM.COM = 1000;
				else if(TORQUE_AB.Error < TORQUE.Hysteresis)PWM.COM = 0;
			}
			else
			{
				TORQUE_CA.Error = TORQUE_CA.Expect - TORQUE_CA.Sample;
				TORQUE_CA.Integral += TORQUE_CA.Error;
				if(TORQUE_CA.Integral > TORQUE_CA.Integral_Max)	TORQUE_CA.Integral = TORQUE_CA.Integral_Max;
				else if(TORQUE_CA.Integral <0)	TORQUE_CA.Integral = 0;
				if(TORQUE_CA.Integral > TORQUE_CA.Integral_Max)TORQUE_CA.Integral = TORQUE_CA.Integral_Max;
				PWM.NOW = TORQUE_CA.Kp * TORQUE_CA.Error/ 300 + TORQUE_CA.Ki * TORQUE_CA.Integral / 1000;
				if(PWM.NOW > PWM.MAX)	PWM.NOW = PWM.MAX;
				else if(PWM.NOW < 0)	PWM.NOW = 0;
			}
			break;
	case  2:
	case  5:if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
			{
				TORQUE_BC.Error = TORQUE_BC.Expect - TORQUE_BC.Sample;
				TORQUE_BC.Integral += TORQUE_BC.Error;
				if(TORQUE_BC.Integral > TORQUE_BC.Integral_Max)	TORQUE_BC.Integral = TORQUE_BC.Integral_Max;
				else if(TORQUE_BC.Integral <0)	TORQUE_BC.Integral = 0;
				PWM.NEXT = TORQUE_BC.Kp * TORQUE_BC.Error/ 300 + TORQUE_BC.Ki * TORQUE_BC.Integral / 1000;
				if(PWM.NEXT > PWM.MAX)	PWM.NEXT = PWM.MAX;
				else if (PWM.NEXT <0)	PWM.NEXT = 0;

				TORQUE_CA.Error = TORQUE_CA.Expect - TORQUE_CA.Sample;
				TORQUE_CA.Integral += TORQUE_CA.Error;
				if(TORQUE_CA.Integral > TORQUE_CA.Integral_Max)	TORQUE_CA.Integral = TORQUE_CA.Integral_Max;
				else if(TORQUE_CA.Integral <0)	TORQUE_CA.Integral = 0;
				PWM.LAST = TORQUE_CA.Kp * TORQUE_CA.Error/ 300 + TORQUE_CA.Ki * TORQUE_CA.Integral / 1000;
				if(PWM.LAST > PWM.MAX)	PWM.LAST = PWM.MAX;
				else if(PWM.LAST < 0)	PWM.LAST = 0;

				if(TORQUE_CA.Error >= 0)PWM.COM = 1000;
				else if(TORQUE_CA.Error < TORQUE.Hysteresis)PWM.COM = 0;
			}
			else
			{
				TORQUE_BC.Error = TORQUE_BC.Expect - TORQUE_BC.Sample;
				TORQUE_BC.Integral += TORQUE_BC.Error;
				if(TORQUE_BC.Integral > TORQUE_BC.Integral_Max)	TORQUE_BC.Integral = TORQUE_BC.Integral_Max;
				else if(TORQUE_BC.Integral <0)	TORQUE_BC.Integral = 0;
				if(TORQUE_BC.Integral > TORQUE_BC.Integral_Max)TORQUE_BC.Integral = TORQUE_BC.Integral_Max;
				PWM.NOW = TORQUE_BC.Kp * TORQUE_BC.Error/ 300 + TORQUE_BC.Ki * TORQUE_BC.Integral / 1000;
				if(PWM.NOW > PWM.MAX)	PWM.NOW = PWM.MAX;
				else if(PWM.NOW < 0)	PWM.NOW = 0;
			}
			break;
	default:	DRV_UP;
	}

}

Uint16 alpha[6];
void IGBT_Control(void)
{
	switch(LOGIC.State)
	{
		case 0:	if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
				{
					alpha[0] 	= (Uint16)PWM.NEXT;
					alpha[1]	= 0;
					alpha[2] 	= 0;
					alpha[3]	= (Uint16)PWM.COM;
					alpha[4] 	= (Uint16)PWM.LAST;
					alpha[5]	= 0;
				}
				else
				{
					alpha[0] 	= (Uint16)PWM.NOW;
					alpha[1]	= 0;
					alpha[2] 	= 0;
					alpha[3]	= (Uint16)PWM.NOW;
					alpha[4] 	= 0;
					alpha[5]	= 0;
				}
				break;
	case 1:		if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
				{
					alpha[0] 	= (Uint16)PWM.COM;
					alpha[1]	= 0;
					alpha[2] 	= 0;
					alpha[3]	= (Uint16)PWM.LAST;
					alpha[4] 	= 0;
					alpha[5]	= (Uint16)PWM.NEXT;
				}
				else
				{
					alpha[0] 	= (Uint16)PWM.NOW;
					alpha[1]	= 0;
					alpha[2] 	= 0;
					alpha[3]	= 0;
					alpha[4] 	= 0;
					alpha[5]	= (Uint16)PWM.NOW;
				}
				break;
	case 2:		if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
				{
					alpha[0] 	= (Uint16)PWM.LAST;
					alpha[1]	= 0;
					alpha[2] 	= (Uint16)PWM.NEXT;
					alpha[3]	= 0;
					alpha[4] 	= 0;
					alpha[5]	= (Uint16)PWM.COM;
				}
				else
				{
					alpha[0] 	= 0;
					alpha[1]	= 0;
					alpha[2] 	= (Uint16)PWM.NOW;
					alpha[3]	= 0;
					alpha[4] 	= 0;
					alpha[5]	= (Uint16)PWM.NOW;
				}
				break;
	case 3:		if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
				{
					alpha[0] 	= 0;
					alpha[1]	= (Uint16)PWM.NEXT;
					alpha[2] 	= (Uint16)PWM.COM;
					alpha[3]	= 0;
					alpha[4] 	= 0;
					alpha[5]	= (Uint16)PWM.LAST;
				}
				else
				{
					alpha[0] 	= 0;
					alpha[1]	= (Uint16)PWM.NOW;
					alpha[2] 	= (Uint16)PWM.NOW;
					alpha[3]	 = 0;
					alpha[4] 	= 0;
					alpha[5]	= 0;
				}
				break;
	case 4:		if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
				{
					alpha[0] 	= 0;
					alpha[1]	= (Uint16)PWM.COM;
					alpha[2] 	= (Uint16)PWM.LAST;
					alpha[3]	= 0;
					alpha[4] 	= (Uint16)PWM.NEXT;
					alpha[5]	= 0;
				}
				else
				{
					alpha[0] 	= 0;
					alpha[1]	= (Uint16)PWM.NOW;
					alpha[2] 	= 0;
					alpha[3]	= 0;
					alpha[4] 	= (Uint16)PWM.NOW;
					alpha[5]	= 0;
				}
				break;
	case 5:		if(SRM.Phase<=Theta_ov&&!SRM_FIRST_RUN)
				{
					alpha[0] 	= 0;
					alpha[1]	= (Uint16)PWM.LAST;
					alpha[2] 	= 0;
					alpha[3]	= (Uint16)PWM.NEXT;
					alpha[4] 	= (Uint16)PWM.COM;
					alpha[5]	= 0;
				}
				else
				{
					alpha[0] 	= 0;
					alpha[1]	= 0;
					alpha[2] 	= 0;
					alpha[3]	= (Uint16)PWM.NOW;
					alpha[4] 	= (Uint16)PWM.NOW;
					alpha[5]	= 0;
				}
				break;
	default :	DRV_UP;
	}
	if(alpha[0] && alpha[1] || alpha[2] && alpha[3] || alpha[4] && alpha[5])
	{
		DRV_UP;
		while(1);	// error!
	}
	else
	{
		EPwm1Regs.CMPA.half.CMPA = alpha[0];
		EPwm1Regs.CMPB			 = alpha[1];
		EPwm2Regs.CMPA.half.CMPA = alpha[2];
		EPwm2Regs.CMPB			 = alpha[3];
		EPwm3Regs.CMPA.half.CMPA = alpha[4];
		EPwm3Regs.CMPB			 = alpha[5];
	}
}

//===========================================================================
// No more.
//===========================================================================
