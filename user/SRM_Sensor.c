/*
 * 	FileName:	srd_sensor.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */

#include "SRD_Project.h"        // User's Funtions

void My_Init_Sensor(void);
void Get_Sensor(void);
void Get_State(void);
void Get_Hall(void);
void Get_Position(void);
void Get_Current(void);

#define CURRENT_Coefficient 6	// Ia = (IU_ad - IU_offset) * CURRENT_Coefficient
//	Filter: y = y_1*Filter_Coefficient1 + x*Filter_Coefficient2 + x_1*Filter_Coefficient3;
#define Filter_Coefficient1 0.928550968482019
#define Filter_Coefficient2 0.035724515758991
#define Filter_Coefficient3 0.035724515758991

struct CURRENT_STRUCT CURRENT;
struct SRM_STRUCT SRM;

Uint16 IU_ad			= 0;		// x+100 means y+6 A
Uint16 IV_ad			= 0;
Uint16 IU_offset		= 0;
Uint16 IV_offset		= 0;
Uint16 DBVD_offset		= 0;
Uint16 Ref3V_offset		= 0;
Uint16 DBVD_ad			= 0;
Uint16 Ref3V_ad			= 0;
Uint16 Ref0V_ad			= 0;
Uint16 HOT_state		= 0;
Uint16 BRV_state		= 0;
Uint16 PA_state			= 0;
Uint16 PB_state			= 0;
Uint16 PC_state			= 0;

int16  SRM_Direction	= 0;
int16  SRM_SPEED		= 0;		// 1 menas 1r/min, -1 menas -1r/min, 150 means 150r/min
int16  SRM_SPEED_1		= 0;		// last SRM_SPEED
int16  SRM_PHASE		= 74;		// 75 means 7.5deg , init 74 to calculate right Torque
int16  SRM_ANGLE		= 0;		// 10 means 1deg, 1800 means 180deg
int16  NOW_state		= -1;		// -1 means first run
int16  LAST_state		= -1;		// -1 means first run
int16  DRV_state		= -1;
int16  SRM_FIRST_RUN	= 1;

void My_Init_Sensor(void)
{
	CURRENT.Ia_1 = 0;
	CURRENT.Ib_1 = 0;
	CURRENT.Ic_1 = 0;
	CURRENT.Ia_sam_1 = 0;
	CURRENT.Ib_sam_1 = 0;
	CURRENT.Ic_sam_1 = 0;
	SRM.Speed = 0;
	SRM.Position = 0;
	SRM.Phase = 0;
	SRM.Direction = 0;
}

void Get_Sensor(void)
{
	Get_State();
	Get_Current();
	Get_Hall();
	Get_Position();
}

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

void Get_Current(void)
{
	CURRENT.Ia_sam = ((int16)IU_ad - (int16)IU_offset) * CURRENT_Coefficient ;
	CURRENT.Ib_sam = ((int16)IV_ad - (int16)IU_offset) * CURRENT_Coefficient ;
	CURRENT.Ic_sam = - CURRENT.Ia_sam - CURRENT.Ib_sam;

	CURRENT.Ia = CURRENT.Ia_1 * Filter_Coefficient1 + CURRENT.Ia_sam * Filter_Coefficient2 + CURRENT.Ia_sam_1 * Filter_Coefficient3;
	CURRENT.Ib = CURRENT.Ib_1 * Filter_Coefficient1 + CURRENT.Ib_sam * Filter_Coefficient2 + CURRENT.Ib_sam_1 * Filter_Coefficient3;
	CURRENT.Ic = - CURRENT.Ia - CURRENT.Ib;
	CURRENT.Ia_1 = CURRENT.Ia;
	CURRENT.Ib_1 = CURRENT.Ib;
	CURRENT.Ic_1 = CURRENT.Ic;
	CURRENT.Ia_sam_1 = CURRENT.Ia_sam;
	CURRENT.Ib_sam_1 = CURRENT.Ib_sam;
	CURRENT.Ic_sam_1 = CURRENT.Ic_sam;

	CURRENT.Ia_abs = CURRENT.Ia>0?CURRENT.Ia:-CURRENT.Ia;
	CURRENT.Ib_abs = CURRENT.Ib>0?CURRENT.Ib:-CURRENT.Ib;
	CURRENT.Ic_abs = CURRENT.Ic>0?CURRENT.Ic:-CURRENT.Ic;
/*	CURRENT.Ia_abs = CURRENT.Ia_sam>0?CURRENT.Ia_sam:-CURRENT.Ia_sam;
	CURRENT.Ib_abs = CURRENT.Ib_sam>0?CURRENT.Ib_sam:-CURRENT.Ia_sam;
	CURRENT.Ic_abs = CURRENT.Ic_sam>0?CURRENT.Ic_sam:-CURRENT.Ia_sam;*/
}

void Get_Hall(void)
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
	if (NOW_state == -1)	NOW_state = LAST_state = temp_state;

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
		else if(SRM_ANGLE < 0)		SRM_ANGLE += 3600;
		LAST_state = NOW_state;
	}

	//if the sensor of SRM is error, set error flag
	if(PA_state == 0 && PB_state == 0 && PC_state == 0)	SRM_ANGLE = -1;
	if(SRM_Direction <= -2 || SRM_Direction >= 2)				SRM_ANGLE = -2;

	//calculate the SRM_SPEED
	if(SRM_Direction)
	{
		SRM_SPEED_1=SRM_SPEED;
		//one T0count means 20us, one SRM_Direction means 7.5deg, so speed(r/min)=62500/T0count, SRM_SPEED=speed:
		if(CpuTimer0.InterruptCount>30000)CpuTimer0.InterruptCount=30000;
		if(SRM_Direction>0)	{SRM_SPEED = 62500/(int16)(CpuTimer0.InterruptCount);}
		else				{SRM_SPEED = -62500/(int16)(CpuTimer0.InterruptCount);}

		if(SRM_Direction>0&&SRM_SPEED<0){SRM_ANGLE = -4;}			//set error flag
		CpuTimer0.InterruptCount = 0;
	}
	else if(CpuTimer0.InterruptCount>30000)	//50000 means 1s
	{
		SRM_SPEED = 0;
	}

	if(SRM_FIRST_RUN && SRM_SPEED)
	{
		SRM_SPEED = 0;
		SRM_FIRST_RUN = 0;
	}

	//calculate the SRM_PHASE(Estimated position)
	if(SRM_SPEED_1>0)
		SRM_PHASE = SRM_SPEED_1*CpuTimer0.InterruptCount*3/2500;		// for SRM_PHASE, 1000 means 1deg.
	else
		SRM_PHASE = -(-SRM_SPEED_1)/4*CpuTimer0.InterruptCount*4/25*3/100;	// there are some problem in the sign of data type

	if		(SRM_PHASE > 75)		SRM_PHASE = 75;
	else if	(SRM_PHASE < -75)		SRM_PHASE = -75;
//	if(!NOW_state%2)	SRM_PHASE += 75;
}

void Get_Position(void)
{
	SRM.Position = EQep2Regs.QPOSCNT;
}

void Get_Speed(void)
{
	SRM.Speed = 1;
}
