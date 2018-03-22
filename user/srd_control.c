/*
 * 	FileName:	srd_control.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/1/16
 *      Author: yangxiaosheng
 */
#include "SRD_Project.h"        			// User's Funtions

// Global variables
int16  IGBT_H		= -1;		// control after change phase
int16  IGBT_L		= -1;		// control after change phase
int16  IGBT_H_switch= 0;		// 0 means close IGBT, 1 means open IGBT
int16  IGBT_L_switch= 0;		// 0 means close IGBT, 1 means open IGBT
int16  FIRST_RUN	= 1;		// 1 means first run
int16  I_error		= 0;
int16  I_abs		= 0;

int16  Expect_SPEED	= 500;		// 1 means the Expect Speed is 1r/min
int16  Expect_I		= 0;		// be used in pid_control()
int16  MAX_SPEED	= 1500;		// 1 means the MAX Speed is 1r/min
int16  MAX_U		= 1447;		// 1447 means the MAX U is about 350V
int16  MAX_I		= 50;		// 50 means the MAX I is about 3A
int16  Start_I		= 25;		// for first run
int16  hysteresis 	= 2;		// be used in hysteresis_control()


void Init_SRD(void);
void phase_control(void);
void pid_control(void);
void hysteresis_control(void);
void output_control(void);

void Init_SRD(void)
{
	my_init_gpio();			//fist run

	DRV_UP;					//H is disaable
	PU_H_DN;				//L is disaable
	PU_L_DN;				//L is disaable
	PV_H_DN;				//L is disaable
	PV_L_DN;				//L is disaable
	PW_H_DN;				//L is disaable
	PW_L_DN;				//L is disaable
	BR_UP;					//H is disaable

	SS_UP;					//H is disaable
	FAN_UP;					//H is disaable

	DELAY_US(1000*1000);
	SS_DN;					//L is enable
	FAN_DN;					//L is enable

	DELAY_US(1000000L);
	my_init_adc();
	error_checking();
	my_init_cputimer();
}

void phase_control(void)
{
	//NOW_state=1 means La max, NOW_state=3 means Lb max, NOW_state=5 means Lc max, phase A must be connected to U, B to V, C to W.
	if(SRM_SPEED>=0)
	{
		if		(NOW_state == 0){ IGBT_H = -1; 		IGBT_L = -1; }
		else if	(NOW_state == 1){ IGBT_H = PU_H; 	IGBT_L = PV_L; }
		else if	(NOW_state == 2){ IGBT_H = -1; 		IGBT_L = -1; }
		else if	(NOW_state == 3){ IGBT_H = PW_H; 	IGBT_L = PU_L; }
		else if	(NOW_state == 4){ IGBT_H = -1; 		IGBT_L = -1; }
		else if	(NOW_state == 5){ IGBT_H = PV_H; 	IGBT_L = PW_L; }
		else IGBT_H = IGBT_L = -1;
	}
}

void pid_control(void)
{
	int16 speed_error = 0;
	int16 Kp = 10;
	speed_error =Expect_SPEED - SRM_SPEED;
	Expect_I = speed_error*Kp/10;
	if(Expect_SPEED>0&&speed_error>Expect_SPEED/3)
	{
		if(Expect_I > Start_I)	Expect_I = Start_I;
	}
	if(Expect_I > MAX_I)	Expect_I = MAX_I;
	if(Expect_I < 0) 		Expect_I = 0;
}

void hysteresis_control(void)
{
	if(IGBT_H == PU_H)
	{
		I_abs = IU_ad - IU_offset;
		if(I_abs < 0)	I_abs = -I_abs;
		I_error = I_abs - Expect_I;
		if(I_error > hysteresis)		IGBT_H_switch = 0; //0 means close IGBT
		else if(I_error < -hysteresis)	IGBT_H_switch = 1; //1 means open IGBT
	}
	else if(IGBT_H == PV_H)
	{
		I_abs = IV_ad - IV_offset;
		if(I_abs < 0)	I_abs = -I_abs;
		I_error = I_abs - Expect_I;
		if(I_error > hysteresis)		IGBT_H_switch = 0; //0 means close IGBT
		else if(I_error < -hysteresis)	IGBT_H_switch = 1; //1 means open IGBT
	}
	else if(IGBT_H == PW_H)
	{
		I_abs = (IU_ad - IU_offset) - (IV_ad - IV_offset);
		if(I_abs < 0)	I_abs = -I_abs;
		I_error = I_abs - Expect_I;
		if(I_error > hysteresis)		IGBT_H_switch = 0; //0 means close IGBT
		else if(I_error < -hysteresis)	IGBT_H_switch = 1; //1 means open IGBT
	}

	if(IGBT_L == PU_L)
	{
		I_abs = IU_ad - IU_offset;
		if(I_abs < 0)	I_abs = -I_abs;
		I_error = I_abs - Expect_I;
		if(I_error > hysteresis)		IGBT_L_switch = 0; //0 means close IGBT
		else if(I_error < -hysteresis)	IGBT_L_switch = 1; //1 means open IGBT
	}
	else if(IGBT_L == PV_L)
	{
		I_abs = IV_ad - IV_offset;
		if(I_abs < 0)	I_abs = -I_abs;
		I_error = I_abs - Expect_I;
		if(I_error > hysteresis)		IGBT_L_switch = 0; //0 means close IGBT
		else if(I_error < -hysteresis)	IGBT_L_switch = 1; //1 means open IGBT
	}
	else if(IGBT_L == PW_L)
	{
		I_abs = (IU_ad - IU_offset) - (IV_ad - IV_offset);
		if(I_abs < 0)	I_abs = -I_abs;
		I_error = I_abs - Expect_I;
		if(I_error > hysteresis)		IGBT_L_switch = 0; //0 means close IGBT
		else if(I_error < -hysteresis)	IGBT_L_switch = 1; //1 means open IGBT
	}

}

void output_control(void)
{
	static int16 last_IGBT_L=-1;

	if(IGBT_H == PU_H && IGBT_L == PU_L)	{DRV_UP; PU_H_DN; PV_H_DN; PW_H_DN; PU_L_DN; PV_L_DN; PW_L_DN; FAN_UP; return;}
	if(IGBT_H == PV_H && IGBT_L == PV_L)	{DRV_UP; PU_H_DN; PV_H_DN; PW_H_DN; PU_L_DN; PV_L_DN; PW_L_DN; FAN_UP; return;}
	if(IGBT_H == PW_H && IGBT_L == PW_L)	{DRV_UP; PU_H_DN; PV_H_DN; PW_H_DN; PU_L_DN; PV_L_DN; PW_L_DN; FAN_UP; return;}


	if(IGBT_H_switch == 1)
	{
		DRV_DN;	DRV_state=1;
		if		(IGBT_H == PU_H)	{PV_H_DN; PW_H_DN; if(last_IGBT_L!=PU_L)PU_H_UP;}
		else if	(IGBT_H	== PV_H)	{PU_H_DN; PW_H_DN; if(last_IGBT_L!=PV_L)PV_H_UP;}
		else if	(IGBT_H	== PW_H)	{PU_H_DN; PV_H_DN; if(last_IGBT_L!=PW_L)PW_H_UP;}
		else
		{
			DRV_UP;	DRV_state=0;
			PV_H_DN; PW_H_DN; PU_H_DN;
		}
	}
	else
	{
		DRV_UP;	DRV_state=0;
		PV_H_DN; PW_H_DN; PU_H_DN;
	}


	if(IGBT_L_switch == 1)
	{
		DRV_DN;	DRV_state=1;
		if		(IGBT_L == PU_L)	{PV_L_DN; PW_L_DN; PU_L_UP;}
		else if	(IGBT_L	== PV_L)	{PU_L_DN; PW_L_DN; PV_L_UP;}
		else if	(IGBT_L	== PW_L)	{PU_L_DN; PV_L_DN; PW_L_UP;}
		else
		{
			DRV_UP;	DRV_state=0;
			PV_L_DN; PW_L_DN; PU_L_DN;
		}
		last_IGBT_L = IGBT_L;
	}
	else
	{
		DRV_UP;	DRV_state=0;
		PV_L_DN; PW_L_DN; PU_L_DN;

		last_IGBT_L = -1;
	}


}

//===========================================================================
// No more.
//===========================================================================
