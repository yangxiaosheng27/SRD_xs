/*
 * 	FileName:	srd_sensor.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */

#include "SRD_Project.h"        // User's Funtions

void Get_State(void);
void Get_Position(void);

Uint16 IU_ad		= 0;
Uint16 IV_ad		= 0;
Uint16 DBVD_ad		= 0;
Uint16 Ref3V_ad		= 0;
Uint16 Ref0V_ad		= 0;
Uint16 HOT_state	= 0;
Uint16 BRV_state	= 0;
Uint16 PA_state		= 0;
Uint16 PB_state		= 0;
Uint16 PC_state		= 0;
Uint16 IU_offset	= 0;
Uint16 IV_offset	= 0;
Uint16 DBVD_offset	= 0;
Uint16 Ref3V_offset	= 0;

int16  SRM_Direction= 0;
int16  SRM_SPEED	= 0;		// 1 menas 1r/min, -1 menas -1r/min, 150 means 150r/min
int16  SRM_PHASE	= 0;		// 1000 means 1deg, 7500 means 7.5deg
int16  SRM_ANGLE	= 0;		// 10 means 1deg, 1800 means 180deg
int16  NOW_state	= -1;		// -1 means first run
int16  LAST_state	= -1;		// -1 means first run
int16  DRV_state	= -1;
int16  FIRST_RUN	= 1;

void Get_State(void)
{
	IU_ad	  =  IU_GET();
	IV_ad	  =  IV_GET();
	DBVD_ad	  =  DBVD_GET();
	Ref3V_ad  =  Ref3V_GET();
	Ref0V_ad  =  Ref0V_GET();
	HOT_state =  HOT_GET();
	BRV_state =  BRV_GET();
	PA_state  =  PA_GET();
	PB_state  =  PB_GET();
	PC_state  =  PC_GET();
}

void Get_Position(void)
{
	static int16 temp_state 	= 0;
	static int16 last_temp 		= 0;
	static int16 count_temp 	= 0;

	if 		(PA_state == 0 && PB_state == 0 && PC_state == 1)temp_state = 5;	//CA
	else if (PA_state == 1 && PB_state == 0 && PC_state == 1)temp_state = 4;	//A
	else if (PA_state == 1 && PB_state == 0 && PC_state == 0)temp_state = 3;	//AB
	else if (PA_state == 1 && PB_state == 1 && PC_state == 0)temp_state = 2;	//B
	else if (PA_state == 0 && PB_state == 1 && PC_state == 0)temp_state = 1;	//BC
	else if (PA_state == 0 && PB_state == 1 && PC_state == 1)temp_state = 0;	//C


	//when first run
	if (LAST_state == -1)	NOW_state = LAST_state = temp_state;

	//determine the temp_state is valid
	if(temp_state!=LAST_state && temp_state==last_temp)
	{
		count_temp++;
		if(count_temp>=4)/////////////note!/////////////////////////////////////////////////////////
		{
			count_temp=0;
			NOW_state = temp_state;
		}
		last_temp = temp_state;
	}
	else
	{
		count_temp=0;
		last_temp = temp_state;
	}

	//get the SRM_Directionection of SRM at the jump time
	SRM_Direction = NOW_state - LAST_state;

	//calculate the SRM_ANGLE
	if(SRM_Direction)
	{
		if(SRM_Direction<=-5)		SRM_Direction += 6;				//ensure SRM_Direction == 1 or 0 or -1, SRM_Direction means the SRM_Directionection of SRM at the jump time
		else if(SRM_Direction>=5)	SRM_Direction -= 6;

		SRM_ANGLE += 75 * SRM_Direction;
		if(SRM_ANGLE >= 3600)	SRM_ANGLE -= 3600;
		if(SRM_ANGLE < 0)		SRM_ANGLE += 3600;
		LAST_state = NOW_state;
		FIRST_RUN  = 0;
	}

	//if the sensor of SRM is error, set error flag
	if(PA_state == 0 && PB_state == 0 && PC_state == 0)	SRM_ANGLE = -1;
	if(SRM_Direction <= -2 || SRM_Direction >= 2)				SRM_ANGLE = -2;

	//calculate the SRM_SPEED
	if(SRM_Direction)
	{
		//one T0count means 20us, one SRM_Direction means 7.5deg, so speed(r/min)=62500/T0count, SRM_SPEED=speed:
		if(CpuTimer0.InterruptCount>30000)CpuTimer0.InterruptCount=30000;
		if(SRM_Direction>0)	{SRM_SPEED = 62500/(int16)(CpuTimer0.InterruptCount);}
		else				{SRM_SPEED = -62500/(int16)(CpuTimer0.InterruptCount);}

		SRM_SPEED = SRM_SPEED*3/2;

		if(SRM_Direction>0&&SRM_SPEED<0){SRM_ANGLE = -4;}			//set error flag
		CpuTimer0.InterruptCount = 0;
	}
	else if(CpuTimer0.InterruptCount>30000)	//50000 means 1s
	{
		SRM_SPEED = 0;
	}

	//filtering the SRM_SPEED
	static int16 SRM_SPEED_1 = 0/*,SRM_SPEED_2 = 0,SRM_SPEED_3 = 0,SRM_SPEED_4 = 0*/;
	SRM_SPEED = (SRM_SPEED+SRM_SPEED_1)/2;
	SRM_SPEED_1 = SRM_SPEED;
/*	SRM_SPEED = (5*SRM_SPEED+3*SRM_SPEED_1+1*SRM_SPEED_2+0*SRM_SPEED_3+0*SRM_SPEED_4)/9;
	SRM_SPEED_4 = SRM_SPEED_3;
	SRM_SPEED_3 = SRM_SPEED_2;
	SRM_SPEED_2 = SRM_SPEED_1;
	SRM_SPEED_1 = SRM_SPEED;*/

	//calculate the Estimated_position
	if(SRM_Direction)
	{
		SRM_PHASE = 0;
	}
	else
	{
		if(SRM_SPEED>0)
			SRM_PHASE = SRM_SPEED*3*CpuTimer0.InterruptCount/50;		// for Estimated_position, 10 means 1deg; for SRM_SPEED, 20 menas 1r/min.
		else
			SRM_PHASE = -(-SRM_SPEED)*3*CpuTimer0.InterruptCount/50;	// there are some problem in the sign of data type

		if		(SRM_PHASE > 7500)		SRM_PHASE = 7500;
		else if	(SRM_PHASE < -7500)		SRM_PHASE = -7500;
	}
}
