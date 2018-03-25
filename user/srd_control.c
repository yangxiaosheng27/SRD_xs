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
void IGBT_Control(void);
void SPEED_Control(void);
void TORQUE_Control(void);
void CURRENT_Control(void);
void PWM_Control(void);

#define SPEED_Expect 	500		// 1000 means 1000	r/min
#define SPEED_MAX		1500	// 1000 means 1000	r/min
#define SPEED_Kp 		2
#define SPEED_Kd 		0
#define TORQUE_MAX 		2000	// 1000 means 1.000 N*m
#define CURRENT_MAX		100		// 100 	means 6 	A
#define CURRENT_Kp		50
#define CURRENT_Ki		1
#define PWM_Duty_MAX	1000		// 1000	means 100%

struct IGBT_STRUCT		IGBT;
struct SPEED_STRUCT 	SPEED;
struct TORQUE_STRUCT 	TORQUE;
struct CURRENT_STRUCT	CURRENT;
struct PWM_STRUCT		PWM;

void Init_SRD(void)
{
	My_Init_GPIO();
	DRV_UP;					//H is disaable
	BR_UP;					//H is disaable
	SS_UP;					//H is disaable
	FAN_UP;					//H is disaable
	My_Init_PWM();

//	CURRENT.Expect = 50;//for test
	//Init Control Parameters
	IGBT.State 			= 0;
	IGBT.Turn 			= 0;
	IGBT.Count 			= 0;
	SPEED.Expect 		= SPEED_Expect;
	SPEED.MAX 			= SPEED_MAX;
	SPEED.Kp 			= SPEED_Kp;
	SPEED.Kd 			= SPEED_Kd;
	SPEED.Error			= 0;
	TORQUE.MAX 			= TORQUE_MAX;
	CURRENT.MAX 		= CURRENT_MAX;
	CURRENT.Kp			= CURRENT_Kp;
	CURRENT.Ki	 		= CURRENT_Ki;
	CURRENT.Error		= 0;
	CURRENT.Integral	= 0;
	PWM.MAX				= PWM_Duty_MAX;

	printf("Hi,YangXiaoSheng!\nPlease press enter to start!\n");
	getchar();
	printf("Starting!\n");

	SS_DN;					//L is enable
	FAN_DN;					//L is enable

	DELAY_US(1000000L);
	My_Init_ADC();
	Error_Checking();
//	DRV_DN;					//L is enable
	My_Init_Cputimer();
}

void Control_SRD(void)
{
	Get_State();
	Get_Position();
	IGBT_Control();
	SPEED_Control();
	TORQUE_Control();
	CURRENT_Control();
	Error_Checking();
	PWM_Control();
}

void IGBT_Control(void)
{
	IGBT.State_1 = IGBT.State;

	if(IGBT.Count)
	{
		switch(NOW_state)
		{
			case 1:
			case 2:		IGBT.State = 0;break;
			case 3:
			case 4:		IGBT.State = 1;break;
			case 5:
			case 0:		IGBT.State = 2;break;
			default:	DRV_UP;
		}
		if(IGBT.State == 2 && SRM_SPEED >= 0)	IGBT.Count = 0;
		else if(IGBT.State == 0 && SRM_SPEED <= 0)	IGBT.Count = 0;
	}
	else
	{
		switch(NOW_state)
		{
			case 1:
			case 2:		IGBT.State = 3;break;
			case 3:
			case 4:		IGBT.State = 4;break;
			case 5:
			case 0:		IGBT.State = 5;break;
			default:	DRV_UP;
		}
		if(IGBT.State == 5 && SRM_SPEED >= 0)	IGBT.Count = 1;
		else if(IGBT.State == 3 && SRM_SPEED <= 0)	IGBT.Count = 1;
	}

	if(IGBT.State_1 == IGBT.State)	IGBT.Turn = 0;
	else							IGBT.Turn = 1;
}

void SPEED_Control(void)
{
	SPEED.Error = SPEED.Expect - SRM_SPEED;
	TORQUE.Expect = SPEED.Kp * SPEED.Error;

	if(TORQUE.Expect > TORQUE.MAX) TORQUE.Expect= TORQUE.MAX;
}

void TORQUE_Control(void)
{
	CURRENT.Expect = TORQUE.Expect / 20;

	if(CURRENT.Expect > CURRENT.MAX) CURRENT.Expect= CURRENT.MAX;
	else if(CURRENT.Expect <0) CURRENT.Expect= 0;
}

void CURRENT_Control(void)		// CURRENT:0-100 means 0-6A, PWM_Duty:0-1000 means 0-100%
{
	switch(IGBT.State)
	{
		case 0:
		case 1:		CURRENT.Sample = (int16)(IU_ad - IU_offset); break;
		case 2:
		case 3:		CURRENT.Sample = (int16)(IV_ad - IV_offset); break;
		case 4:
		case 5:		CURRENT.Sample = (int16)(IU_offset +  IV_offset - IU_ad - IV_ad); break;
		default :	DRV_UP;
	}

	CURRENT.Error_1 = CURRENT.Error;
	CURRENT.Error = CURRENT.Expect - CURRENT.Sample;
	if(IGBT.Turn)
		CURRENT.Integral = 0;
	else if(PWM.Duty!=0 && PWM.Duty!=PWM.MAX)
		CURRENT.Integral = CURRENT.Integral + CURRENT.Error_1;
	PWM.Duty = CURRENT.Kp * CURRENT.Error + CURRENT.Ki * CURRENT.Integral / 100;

	if(PWM.Duty<0) PWM.Duty = 0;
	else if(PWM.Duty>PWM.MAX) PWM.Duty = PWM.MAX;
}

void PWM_Control(void)
{
	switch(IGBT.State)
	{
		case 0:		EPwm1Regs.CMPA.half.CMPA = (Uint16)PWM.Duty;
					EPwm1Regs.CMPB			 = 0;
					EPwm2Regs.CMPA.half.CMPA = 0;
					EPwm2Regs.CMPB			 = 1000;
					EPwm3Regs.CMPA.half.CMPA = 0;
					EPwm3Regs.CMPB			 = 0;
					break;
		case 1:		EPwm1Regs.CMPA.half.CMPA = (Uint16)PWM.Duty;
					EPwm1Regs.CMPB			 = 0;
					EPwm2Regs.CMPA.half.CMPA = 0;
					EPwm2Regs.CMPB			 = 0;
					EPwm3Regs.CMPA.half.CMPA = 0;
					EPwm3Regs.CMPB			 = 1000;
					break;
		case 2:		EPwm1Regs.CMPA.half.CMPA = 0;
					EPwm1Regs.CMPB			 = 0;
					EPwm2Regs.CMPA.half.CMPA = (Uint16)PWM.Duty;
					EPwm2Regs.CMPB			 = 0;
					EPwm3Regs.CMPA.half.CMPA = 0;
					EPwm3Regs.CMPB			 = 1000;
					break;
		case 3:		EPwm1Regs.CMPA.half.CMPA = 0;
					EPwm1Regs.CMPB			 = 1000;
					EPwm2Regs.CMPA.half.CMPA = (Uint16)PWM.Duty;
					EPwm2Regs.CMPB			 = 0;
					EPwm3Regs.CMPA.half.CMPA = 0;
					EPwm3Regs.CMPB			 = 0;
					break;
		case 4:		EPwm1Regs.CMPA.half.CMPA = 0;
					EPwm1Regs.CMPB			 = 1000;
					EPwm2Regs.CMPA.half.CMPA = 0;
					EPwm2Regs.CMPB			 = 0;
					EPwm3Regs.CMPA.half.CMPA = (Uint16)PWM.Duty;
					EPwm3Regs.CMPB			 = 0;
					break;
		case 5:		EPwm1Regs.CMPA.half.CMPA = 0;
					EPwm1Regs.CMPB			 = 0;
					EPwm2Regs.CMPA.half.CMPA = 0;
					EPwm2Regs.CMPB			 = 1000;
					EPwm3Regs.CMPA.half.CMPA = (Uint16)PWM.Duty;
					EPwm3Regs.CMPB			 = 0;
					break;
		default :	DRV_UP;
	}
}

//===========================================================================
// No more.
//===========================================================================
