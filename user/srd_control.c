/*
 * 	FileName:	srd_control.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/1/16
 *      Author: yangxiaosheng
 */
#include "DSP28x_Project.h"     			// Device Headerfile and Examples Include File
#include "SRD_Project.h"        			// User's Funtions

// Global variables
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
Uint16 ERROR		= 0;
int16  SRM_SPEED	= 0;		// 1 menas 1r/min, -1 menas -1r/min, 150 means 150r/min
int16  SRM_ANGLE	= 0;		// 10 means 1deg, 1800 means 180deg
int16  SRM_STATE	= 0;		// 10 means 1deg, 1800 means 180deg
int16  NOW_state	= -1;		// -1 means first run
int16  LAST_state	= -1;		// -1 means first run
int16  LAST_state_test;			// for test
int16  DRV_state	= -1;
int16  IGBT_H		= -1;		// control after change phase
int16  IGBT_L		= -1;		// control after change phase
int16  IGBT_H_switch= 0;		// 0 means close IGBT, 1 means open IGBT
int16  IGBT_L_switch= 0;		// 0 means close IGBT, 1 means open IGBT
int16  FIRST_RUN	= 1;		// 1 means first run
int16  I_error		= 0;
int16  I_abs		= 0;

int16  Expect_SPEED	= 1500;		// 1 means the Expect Speed is 1r/min
int16  Expect_I		= 0;		// be used in pid_control()
int16  MAX_SPEED	= 2000;		// 1 means the MAX Speed is 1r/min
int16  MAX_U		= 1447;		// 1447 means the MAX U is about 350V
int16  MAX_I		= 100;		// 50 means the MAX I is about 3A
int16  Start_I		= 25;		// for first run
int16  hysteresis 	= 2;		// be used in hysteresis_control()
int16 SRM_PHASE,bit_speed=200,phase_on=50,phase_off=50;


void my_init_srd(void);
void init_position();
void error_checking(void);
void get_state(void);
void get_position(void);
void phase_control(void);
void pid_control(void);
void hysteresis_control(void);
void output_control(void);

void my_init_srd(void)
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

//	DELAY_US(1000*1000);
	SS_DN;					//L is enable
	FAN_DN;					//L is enable

	DELAY_US(3000000L);
	my_init_adc();
	error_checking();
	my_init_cputimer();
}

void error_checking(void)
{
	if(IU_offset<1800||IU_offset>1900)											ERROR |= 0x0001;
	if(IV_offset<1800||IV_offset>1900)											ERROR |= 0x0002;
	if(DBVD_offset<1230||DBVD_offset>1350)										ERROR |= 0x0004;
	if(Ref3V_offset<3650||Ref3V_offset>3800)									ERROR |= 0x0008;
	if(IU_ad<(IU_offset-MAX_I-100)||IU_ad>(IU_offset+MAX_I+100))				ERROR |= 0x0010;
	if(IV_ad<(IV_offset-MAX_I-100)||IV_ad>(IV_offset+MAX_I+100))				ERROR |= 0x0020;
	if(DBVD_ad<(DBVD_offset-125)||DBVD_ad>MAX_U)								ERROR |= 0x0040;
	if(Ref3V_ad<(Ref3V_offset-150)||Ref3V_ad>(Ref3V_offset+150))				ERROR |= 0x0080;
	if((PieCtrlRegs.PIEIFR1.all | 0x40) == 0 && CpuTimer0Regs.TIM.all < 0X0100)	ERROR |= 0x0100;
	if(SRM_STATE<0||SRM_STATE>=3600)											ERROR |= 0x0200;
	if(SRM_SPEED<0)																ERROR |= 0x0400;
	if(IGBT_H==PU_H && IGBT_L==PU_L)											ERROR |= 0x0800;
	if(IGBT_H==PV_H && IGBT_L==PV_L)											ERROR |= 0x1000;
	if(IGBT_H==PW_H && IGBT_L==PW_L)											ERROR |= 0x2000;
	if(SRM_SPEED>MAX_SPEED||SRM_SPEED<-MAX_SPEED)								ERROR |= 0x4000;

//	if(HOT_state == 1)
	if(ERROR)
	{
		DINT;    				// Disable Interrupts at the CPU level:
		CpuTimer0Regs.TCR.bit.TSS = 1;
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
		while(1);
	}
}

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
	static int16 dir_test 		= 0;		//test by yxs
	static int16 count_dir_test = 0;		//test by yxs
	static int16 max_dir_test 	= 0;		//test by yxs
	static int16 time_test 		= 0;		//test by yxs

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

	//test for debug, can be delete
	if(dir) {dir_test = dir;LAST_state_test=LAST_state;}											//dir_test must be 1 or -1 when runing
	if(dir_test<=-5)		dir_test += 6;
	else if(dir_test>=5)	dir_test -= 6;
	if(NOW_state != LAST_state)count_dir_test++;
	else if(++time_test>100)time_test=count_dir_test=0;
	if(count_dir_test > max_dir_test)max_dir_test=count_dir_test;	//max_dir_test must be 1 when runing
	//end test

	//calculate the SRM_STATE
	if(dir)
	{
		if(dir<=-5)		dir += 6;				//ensure dir == 1 or 0 or -1, means the direction of SRM at the jump time
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
			SRM_ANGLE = SRM_STATE + SRM_SPEED*3*CpuTimer0.InterruptCount/2000;		// for SRM_ANGLE, 10 means 1deg; for SRM_SPEED, 20 menas 1r/min.
		else
			SRM_ANGLE = SRM_STATE - (-SRM_SPEED)*3*CpuTimer0.InterruptCount/2000;	// there are some problem in the sign of data type

		if		(SRM_ANGLE > (SRM_STATE + 75))		SRM_ANGLE = (SRM_STATE + 75);
		else if	(SRM_ANGLE < (SRM_STATE - 75))		SRM_ANGLE = (SRM_STATE - 75);
		if		(SRM_STATE >= 3600)					SRM_ANGLE -= 3600;
		else if	(SRM_STATE < 0)						SRM_ANGLE += 3600;
	}
}

void phase_control(void)
{
	//NOW_state=1 means La max, NOW_state=3 means Lb max, NOW_state=5 means Lc max, phase A must be connected to U, B to V, C to W.
	if(SRM_SPEED>=0 && SRM_SPEED<=bit_speed)
	{
		if		(NOW_state == 0) IGBT_H = PU_H;
		else if	(NOW_state == 2) IGBT_H = PV_H;
		else if	(NOW_state == 4) IGBT_H = PW_H;
		else if	(FIRST_RUN ==1 && NOW_state == 5 ) 	IGBT_H = PU_H;
		else if	(FIRST_RUN ==1 && NOW_state == 1 ) 	IGBT_H = PV_H;
		else if	(FIRST_RUN ==1 && NOW_state == 3 ) 	IGBT_H = PW_H;
		else 										IGBT_H = -1;

		if		(NOW_state == 4) IGBT_L = PU_L;
		else if	(NOW_state == 0) IGBT_L = PV_L;
		else if	(NOW_state == 2) IGBT_L = PW_L;
		else if	(FIRST_RUN ==1 && NOW_state == 3 ) 	IGBT_L = PU_L;
		else if	(FIRST_RUN ==1 && NOW_state == 5 ) 	IGBT_L = PV_L;
		else if	(FIRST_RUN ==1 && NOW_state == 1 ) 	IGBT_L = PW_L;
		else 										IGBT_L = -1;
	}
	else if(SRM_SPEED>bit_speed)
	{
		SRM_PHASE = SRM_ANGLE%75;
		switch(NOW_state)
		{
		case 0:if(SRM_PHASE<=phase_off){IGBT_H=PU_H;IGBT_L=PV_L;}else IGBT_H=IGBT_L=-1;break;
		case 1:if(SRM_PHASE>=phase_on){IGBT_H=PV_H;IGBT_L=PW_L;}else IGBT_H=IGBT_L=-1;break;
		case 2:if(SRM_PHASE<=phase_off){IGBT_H=PV_H;IGBT_L=PW_L;}else IGBT_H=IGBT_L=-1;break;
		case 3:if(SRM_PHASE>=phase_on){IGBT_H=PW_H;IGBT_L=PU_L;}else IGBT_H=IGBT_L=-1;break;
		case 4:if(SRM_PHASE<=phase_off){IGBT_H=PW_H;IGBT_L=PU_L;}else IGBT_H=IGBT_L=-1;break;
		case 5:if(SRM_PHASE>=phase_on){IGBT_H=PU_H;IGBT_L=PV_L;}else IGBT_H=IGBT_L=-1;break;
		default:IGBT_H=IGBT_L=-1;break;
		}
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
	if(IGBT_H == PU_H && IGBT_L == PU_L)	{DRV_UP; PU_H_DN; PV_H_DN; PW_H_DN; PU_L_DN; PV_L_DN; PW_L_DN; FAN_UP; return;}
	if(IGBT_H == PV_H && IGBT_L == PV_L)	{DRV_UP; PU_H_DN; PV_H_DN; PW_H_DN; PU_L_DN; PV_L_DN; PW_L_DN; FAN_UP; return;}
	if(IGBT_H == PW_H && IGBT_L == PW_L)	{DRV_UP; PU_H_DN; PV_H_DN; PW_H_DN; PU_L_DN; PV_L_DN; PW_L_DN; FAN_UP; return;}


	if(IGBT_H_switch == 1)
	{
		DRV_DN;	DRV_state=1;
		if		(IGBT_H == PU_H)	{PV_H_DN; PW_H_DN; PU_H_UP;}
		else if	(IGBT_H	== PV_H)	{PU_H_DN; PW_H_DN; PV_H_UP;}
		else if	(IGBT_H	== PW_H)	{PU_H_DN; PV_H_DN; PW_H_UP;}
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
	}
	else
	{
		DRV_UP;	DRV_state=0;
		PV_L_DN; PW_L_DN; PU_L_DN;
	}
}

//===========================================================================
// No more.
//===========================================================================
