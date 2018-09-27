/*
 * 	FileName:	srm_control.c
 * 	Project:	SRM
 *
 *  Created on: 2018/7/18
 *  Author: 	yangxiaosheng
 */
#include "SRM_Project.h"        // User's Funtions
#include <stdio.h>

void Init_SRM(void);
void Control_SRM_internal_loop(void);
void Control_SRM_external_loop(void);

void My_Init_Control(void);
void LOGIC_Control(void);
void SPEED_Control(void);
void TORQUE_Calculate(void);
void TORQUE_Distribution(void);
void TORQUE_Control(void);
void IGBT_Control(void);

int16 Enable_Identification = 0;
int16 Enable_AdaptControl = 0;

#define SPEED_Expect 		800			// 1000 means 1000	r/min
#define SPEED_MAX			1500		// 1000 means 1000	r/min
#define SPEED_Kp 			3
#define SPEED_Ki			0.015
#define SPEED_Kd 			0
#define SPEED_MAX 			1500		// 1500 means 1500r/min
#define TORQUE_MAX 			600			// 500 means 5A
#define TORQUE_Kp			300
#define TORQUE_Ki			10
#define TORQUE_Hysteresis	30			// used in Theta_ov
#define PWM_MAX				1000		// PWM_MAX	means 100% duty pwm
#define Theta_ov			10			// 10	means 1 deg

struct LOGIC_STRUCT			LOGIC;
struct SPEED_STRUCT 		SPEED;
struct TORQUE_STRUCT 		TORQUE;
struct TORQUE_STRUCT 		TORQUE_AB;
struct TORQUE_STRUCT 		TORQUE_BC;
struct TORQUE_STRUCT 		TORQUE_CA;
struct PWM_STRUCT			PWM;

void Control_SRM_external_loop(void)
{
	Get_Speed();
	if(Enable_Identification)
	{
//		System_Identification();
	}
	if(Enable_AdaptControl)
	{
//		Adapt_Control();
	}
	SPEED_Control();
}
void SPEED_Control(void)
{
	if(SPEED.Expect>SPEED.MAX)SPEED.Expect=SPEED.MAX;
	SPEED.Error = SPEED.Expect - SRM.Speed;
	SPEED.Integral += SPEED.Error;
	if(SPEED.Integral>SPEED.Integral_Max)SPEED.Integral = SPEED.Integral_Max;
	else if(SPEED.Integral<-SPEED.Integral_Max)SPEED.Integral = -SPEED.Integral_Max;
	TORQUE.Expect = SPEED.Kp * SPEED.Error + SPEED.Ki * SPEED.Integral;

//	if(Enable_AdaptControl)	TORQUE.Expect = u_adapt;

	if(TORQUE.Expect > TORQUE.MAX) TORQUE.Expect = TORQUE.MAX;
	else if(TORQUE.Expect<0) TORQUE.Expect = 0;

//	TORQUE.Expect = 50;
}

void Init_SRM(void)
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


#ifdef  ENABLE_iostring
        printf("Hi,YangXiaoSheng!\nPress \"Enter\" to run the SRM.\n");
        getchar();

    #ifdef	ENABLE_CONTROL
        printf("SRM is Runing.\n");
    #else
        printf("SRM is NOT Runing.\n");
    #endif
#endif

	SS_DN;					//L is Enable
	FAN_DN;					//L is Enable

	DELAY_US(2000000L);
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

void Control_SRM_internal_loop(void)
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
	SPEED.Integral_Max	= TORQUE_MAX*10L;	// not use
	SPEED.Error			= 0;
	TORQUE.MAX 			= TORQUE_MAX;
	TORQUE.Hysteresis	= TORQUE_Hysteresis;
	TORQUE_AB.MAX 		= TORQUE_MAX;
	TORQUE_AB.Kp		= TORQUE_Kp;
	TORQUE_AB.Ki		= TORQUE_Ki;
	TORQUE_AB.Error		= 0;
	TORQUE_AB.Integral	= 0;
	TORQUE_AB.Integral_Max	= PWM_MAX*100L / TORQUE_Ki;
	TORQUE_BC.MAX 		= TORQUE_MAX;
	TORQUE_BC.Kp		= TORQUE_Kp;
	TORQUE_BC.Ki		= TORQUE_Ki;
	TORQUE_BC.Error		= 0;
	TORQUE_BC.Integral	= 0;
	TORQUE_BC.Integral_Max	= PWM_MAX*100L / TORQUE_Ki;
	TORQUE_CA.MAX 		= TORQUE_MAX;
	TORQUE_CA.Kp		= TORQUE_Kp;
	TORQUE_CA.Ki		= TORQUE_Ki;
	TORQUE_CA.Error		= 0;
	TORQUE_CA.Integral	= 0;
	TORQUE_CA.Integral_Max	= PWM_MAX*100L / TORQUE_Ki;
}

#pragma CODE_SECTION(LOGIC_Control, "ramfuncs");       // for real-time operation , so must be run in the RAM
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

#pragma CODE_SECTION(TORQUE_Calculate, "ramfuncs");       // for real-time operation , so must be run in the RAM
void TORQUE_Calculate(void)
{
	switch(LOGIC.State)
	{
	case  0:
	case  3:	TORQUE_AB.Sample 	= CURRENT.Ia_abs;
				TORQUE_BC.Sample 	= CURRENT.Ic_abs;
				TORQUE_CA.Sample 	= 0;
				TORQUE.Sample		= TORQUE_AB.Sample;
				break;
	case  1:
	case  4:	TORQUE_AB.Sample 	= CURRENT.Ib_abs;
				TORQUE_BC.Sample 	= 0;
				TORQUE_CA.Sample 	= CURRENT.Ic_abs;
				TORQUE.Sample		= TORQUE_CA.Sample;
				break;
	case  2:
	case  5:	TORQUE_AB.Sample 	= 0;
				TORQUE_BC.Sample 	= CURRENT.Ib_abs;
				TORQUE_CA.Sample 	= CURRENT.Ia_abs;
				TORQUE.Sample		= TORQUE_BC.Sample;
				break;
	default:	DRV_UP;
	}
	TORQUE.Sample = (TORQUE.Sample-TORQUE.Expect)*0.3+TORQUE.Expect;
}

#pragma CODE_SECTION(TORQUE_Distribution, "ramfuncs");       // for real-time operation , so must be run in the RAM
void TORQUE_Distribution(void)
{
	if(SRM_FIRST_RUN)
	{
		TORQUE.Expect = TORQUE.MAX;
	}
	if(SRM.Phase<=Theta_ov && !SRM_FIRST_RUN)
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

#pragma CODE_SECTION(TORQUE_Control, "ramfuncs");       // for real-time operation , so must be run in the RAM
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

				if(TORQUE_BC.Error >= 0)PWM.COM = PWM.MAX;
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

				if(TORQUE_AB.Error >= 0)PWM.COM = PWM.MAX;
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

				if(TORQUE_CA.Error >= 0)PWM.COM = PWM.MAX;
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
#pragma CODE_SECTION(IGBT_Control, "ramfuncs");       // for real-time operation , so must be run in the RAM
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
					alpha[0] 	= (Uint16)PWM.MAX;
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
					alpha[5]	= (Uint16)PWM.MAX;
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
					alpha[2] 	= (Uint16)PWM.MAX;
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
					alpha[1]	= (Uint16)PWM.MAX;
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
					alpha[4] 	= (Uint16)PWM.MAX;
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
					alpha[3]	= (Uint16)PWM.MAX;
					alpha[4] 	= (Uint16)PWM.NOW;
					alpha[5]	= 0;
				}
				break;
	default :	DRV_UP;
	}
	if(alpha[0] && alpha[1] || alpha[2] && alpha[3] || alpha[4] && alpha[5])
	{
		DRV_UP;
		while(1);	// danger! stop control singal!
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
