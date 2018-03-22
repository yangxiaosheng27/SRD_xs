/*
 * 	FileName:	srd_sensor.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/22
 *      Author: yangxiaosheng
 */

#include "SRD_Project.h"        // User's Funtions

void get_state(void);
void get_position(void);

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
int16  SRM_SPEED	= 0;		// 1 menas 1r/min, -1 menas -1r/min, 150 means 150r/min
int16  SRM_ANGLE	= 0;		// 10 means 1deg, 1800 means 180deg
int16  SRM_STATE	= 0;		// 10 means 1deg, 1800 means 180deg
int16  NOW_state	= -1;		// -1 means first run
int16  LAST_state	= -1;		// -1 means first run
int16  LAST_state_test;			// for test
int16  DRV_state	= -1;

void get_state(void)
{
	IU_ad	  =  IU_GET;
	IV_ad	  =  IV_GET;
	DBVD_ad	  =  DBVD_GET;
	Ref3V_ad  =  Ref3V_GET;
	Ref0V_ad  =  Ref0V_GET;
	HOT_state =  HOT_GET;
	BRV_state =  BRV_GET;
	PA_state  =  PA_GET;
	PB_state  =  PB_GET;
	PC_state  =  PC_GET;
}

void get_position(void)
{
	static int16 dir 			= 0;
	static int16 temp_state 	= 0;
	static int16 last_temp 		= 0;
	static int16 count_temp 	= 0;
/*	static int16 dir_test 		= 0;		//test by yxs
	static int16 count_dir_test = 0;		//test by yxs
	static int16 max_dir_test 	= 0;		//test by yxs
	static int16 time_test 		= 0;		//test by yxs*/

	if 		(PA_state == 0 && PB_state == 0 && PC_state == 1)temp_state = 0;	//CA
	else if (PA_state == 1 && PB_state == 0 && PC_state == 1)temp_state = 1;	//A
	else if (PA_state == 1 && PB_state == 0 && PC_state == 0)temp_state = 2;	//AB
	else if (PA_state == 1 && PB_state == 1 && PC_state == 0)temp_state = 3;	//B
	else if (PA_state == 0 && PB_state == 1 && PC_state == 0)temp_state = 4;	//BC
	else if (PA_state == 0 && PB_state == 1 && PC_state == 1)temp_state = 5;	//C

/*	else
	{	//error state, but sometime it is exist...
		if (PA_state == 1 && PB_state == 1 && PC_state == 1)
		{
			NOW_state = last_temp+1;
			LAST_state = last_temp;
		}
		if (temp_state>5)temp_state-=6;
	}*/

	//when first run
	if (LAST_state == -1)	NOW_state = LAST_state = temp_state;

	//determine the temp_state is valid
	if(temp_state!=LAST_state && temp_state==last_temp)
	{
		count_temp++;
		if(count_temp>=25)
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

	//get the direction of SRM at the jump time
	dir = NOW_state - LAST_state;

/*	//test for debug, can be delete
	if(dir) {dir_test = dir;LAST_state_test=LAST_state;}											//dir_test must be 1 or -1 when runing
	if(dir_test<=-5)		dir_test += 6;
	else if(dir_test>=5)	dir_test -= 6;
	if(NOW_state != LAST_state)count_dir_test++;
	else if(++time_test>100)time_test=count_dir_test=0;
	if(count_dir_test > max_dir_test)max_dir_test=count_dir_test;	//max_dir_test must be 1 when runing
	//end test
*/
	//calculate the SRM_STATE
	if(dir)
	{
		if(dir<=-5)		dir += 6;				//ensure dir == 1 or 0 or -1, dir means the direction of SRM at the jump time
		else if(dir>=5)	dir -= 6;

		SRM_STATE += 75 * dir;
		if(SRM_STATE >= 3600)	SRM_STATE -= 3600;
		if(SRM_STATE < 0)		SRM_STATE += 3600;
		LAST_state = NOW_state;
		FIRST_RUN  = 0;
	}

	//if the sensor of SRM is error, set error flag
	if(PA_state == 0 && PB_state == 0 && PC_state == 0)	SRM_STATE = -1;
//	if(PA_state == 1 && PB_state == 1 && PC_state == 1)	SRM_STATE = -2;
	if(dir <= -2 || dir >= 2)							SRM_STATE = -3;

	//calculate the SRM_SPEED
	if(dir)
	{
		//one T0count means 20us, one dir means 7.5deg, so speed(r/min)=62500/T0count, SRM_SPEED=speed:
		if(CpuTimer0.InterruptCount>30000)CpuTimer0.InterruptCount=30000;
		if(dir>0)	{SRM_SPEED = 62500/(int16)(CpuTimer0.InterruptCount);}
		else		{SRM_SPEED = -62500/(int16)(CpuTimer0.InterruptCount);}
		if(dir>0&&SRM_SPEED<0){SRM_STATE = -4;}			//set error flag
		CpuTimer0.InterruptCount = 0;
	}
	else if(CpuTimer0.InterruptCount>30000)	//50000 means 1s
	{
		SRM_SPEED = 0;
	}

	//filtering the SRM_SPEED
	static int16 SRM_SPEED_last = 0;
	SRM_SPEED = SRM_SPEED/2 + SRM_SPEED_last/2;
	SRM_SPEED_last = SRM_SPEED;


	//calculate the SRM_ANGLE
	if(dir)
	{
		SRM_ANGLE = SRM_STATE;
	}
	else
	{
		if(SRM_SPEED>0)
			SRM_ANGLE = SRM_STATE + SRM_SPEED*3*CpuTimer0.InterruptCount/50000;		// for SRM_ANGLE, 10 means 1deg; for SRM_SPEED, 20 menas 1r/min.
		else
			SRM_ANGLE = SRM_STATE - (-SRM_SPEED)*3*CpuTimer0.InterruptCount/50000;	// there are some problem in the sign of data type

		if		(SRM_ANGLE > (SRM_STATE + 75))		SRM_ANGLE = (SRM_STATE + 75);
		else if	(SRM_ANGLE < (SRM_STATE - 75))		SRM_ANGLE = (SRM_STATE - 75);
		if		(SRM_STATE >= 3600)					SRM_ANGLE -= 3600;
		else if	(SRM_STATE < 0)						SRM_ANGLE += 3600;
	}
}
