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
void SPEED_Control(void);
void TORQUE_Control(void);
void CURRENT_Control(void);
void OUTPUT_Control(void);

#define SPEED_Expect 	500		// 1000 means 1000	r/min
#define SPEED_MAX		1500	// 1000 means 1000	r/min
#define SPEED_Kp 		5
#define SPEED_Kd 		0
#define TORQUE_MAX 		2000	// 1000 means 1.000 N*m
#define CURRENT_MAX		100		// 100 	means 6 	A
#define CURRENT_Kp		5
#define CURRENT_Ki		0


struct SPEED_STRUCT 		SPEED;
struct TORQUE_STRUCT 	TORQUE;
struct CURRENT_STRUCT	CURRENT;

void Init_SRD(void)
{
	My_Init_GPIO();			//fist run
	DRV_UP;					//H is disaable
	BR_UP;					//H is disaable
	SS_UP;					//H is disaable
	FAN_UP;					//H is disaable
	My_Init_PWM();

	//Init Control Parameters
	SPEED.Expect 	= SPEED_Expect;
	SPEED.MAX 		= SPEED_MAX;
	SPEED.Kp 		= SPEED_Kp;
	SPEED.Kd 		= SPEED_Kd;
	TORQUE.MAX 		= TORQUE_MAX;
	CURRENT.MAX 	= CURRENT_MAX;
	CURRENT.Kp		= CURRENT_Kp;
	CURRENT.Ki	 	= CURRENT_Ki;

	printf("Hi,YangXiaoSheng!\nPlease press enter to start!\n");
	getchar();
	printf("Starting!\n");

	SS_DN;					//L is enable
	FAN_DN;					//L is enable

	DELAY_US(1000000L);
	My_Init_ADC();
	Error_Checking();
	My_Init_Cputimer();
}

void Control_SRD(void)
{
	Get_State();
	Get_Position();
	SPEED_Control();
	TORQUE_Control();
	CURRENT_Control();
	Error_Checking();
	OUTPUT_Control();
}

void SPEED_Control(void)
{
	SPEED.Error = SPEED.Expect - SRM_SPEED;
	TORQUE.Expect = SPEED.Kp * SPEED.Error;

	if(TORQUE.Expect > TORQUE.MAX) TORQUE.Expect= TORQUE.MAX;
}

void TORQUE_Control(void)
{
	CURRENT.Expect = TORQUE.Expect*2;

	if(CURRENT.Expect > CURRENT.MAX) CURRENT.Expect= CURRENT.MAX;
}

void CURRENT_Control(void)		// CURRENT:0-100 means 0-6A, PWM_Duty:0-1000 means 0-100%
{
	CURRENT.Expect = 30;//for test

	CURRENT.Sample = (int16)(IU_ad - IU_offset);

	CURRENT.Error = CURRENT.Expect - CURRENT.Sample;
	CURRENT.PWM_Duty = CURRENT.Kp * CURRENT.Error;
	if(CURRENT.PWM_Duty<0) CURRENT.PWM_Duty = 0;
	if(CURRENT.PWM_Duty>50) CURRENT.PWM_Duty = 50;
}

void OUTPUT_Control(void)
{
	EPwm1Regs.CMPA.half.CMPA = (Uint16)CURRENT.PWM_Duty;
	EPwm2Regs.CMPA.half.CMPA = 0;
	EPwm3Regs.CMPA.half.CMPA = 0;
	EPwm1Regs.CMPB			 = 0;
	EPwm2Regs.CMPB			 = 1000;
	EPwm3Regs.CMPB			 = 0;

	DRV_DN;					//L is enable
}

//===========================================================================
// No more.
//===========================================================================
