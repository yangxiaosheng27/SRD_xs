/*
 * 	FileName:	SRD_Project.h
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */

#define ENABLE_CONTROL	// Enable the IGBT control signal. Be careful to use it.

#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "F2806x_Examples.h"   	// F2806x Examples Include File
#include <stdio.h>

#ifndef USER_SRD_PROJECT_H_
#define USER_SRD_PROJECT_H_

extern void My_Init_ADC(void);
extern void My_Init_Cputimer(void);
extern void My_Init_GPIO(void);
extern void My_Init_EQEP(void);
extern void My_Init_PWM(void);
extern void My_Init_Sensor(void);
extern void My_Init_Error(void);

extern void Error_Checking(void);

extern void Get_Sensor(void);
extern void Get_Speed(void);

extern void Init_SRD(void);
extern void Control_SRD_internal_loop(void);
extern void Control_SRD_external_loop(void);

// for test_control
extern void SPEED_Control(void);
extern void TORQUE_Calculate(void);
extern void TORQUE_Distribution(void);
extern void TORQUE_Control(void);
extern void IGBT_Control(void);

struct LOGIC_STRUCT{
	int16	State;
	int16	State_1;		// State_n means State*z^(-n)
	int16	Turn;
	int16	Count;
};
struct SPEED_STRUCT{
	int32	Expect;
	int32	MAX;
	int32	Kp;
	int32 	Kd;
	int32 	Error;
	int32 	Error_1;		// Error_n means Error*z^(-n)
	int32 	Differential;
	int32	Count;
};
struct TORQUE_STRUCT{
	int32  	Expect;
	int32	NEXT;
	int32	LAST;
	int32  	MAX;
	int32 	Kp;
	int32 	Ki;
	int32 	Error;
	int32 	Sample;
	int32 	Integral;
	int32 	Integral_Max;
	int32 	Hysteresis;
};
struct PWM_STRUCT{
	int32  	MAX;
	int32  	NOW;
	int32  	NEXT;
	int32 	LAST;
	int32 	COM;
};

struct CURRENT_STRUCT{
	float32	Ia;			// Ia(filtter)
	float32	Ib;
	float32	Ic;
	float32	Ia_abs;		// |Ia(filtter)|
	float32	Ib_abs;
	float32	Ic_abs;
	float32	Ia_sam;		// Ia(sample)
	float32	Ib_sam;
	float32	Ic_sam;
	float32	Ia_sam_1;	// Ia(sample)*z^-1
	float32	Ib_sam_1;
	float32	Ic_sam_1;
	float32	Ia_1;		// Ia(filtter)*z^-1
	float32	Ib_1;
	float32	Ic_1;
};

struct ERROR_STRUCT{
	Uint16	Code;
	Uint16	Check_Count;
	Uint16	CPU_Timer0_Remain;
};

struct SRM_STRUCT{
	float32	Speed;
	Uint16	Position;
	Uint16	Angle;
	Uint16	Phase;
	Uint16	Phase_Bias;
	int16	Direction;
};

extern struct LOGIC_STRUCT			LOGIC;
extern struct SPEED_STRUCT 			SPEED;
extern struct TORQUE_STRUCT 		TORQUE;
extern struct TORQUE_STRUCT 		TORQUE_AB;
extern struct TORQUE_STRUCT 		TORQUE_BC;
extern struct TORQUE_STRUCT 		TORQUE_CA;
extern struct PWM_STRUCT			PWM;
extern struct CURRENT_STRUCT		CURRENT;
extern struct SRM_STRUCT 			SRM;
extern struct ERROR_STRUCT 			ERROR;

extern Uint16 alpha[6];

extern Uint16 IU_ad;
extern Uint16 IV_ad;
extern Uint16 DBVD_ad;
extern Uint16 Ref3V_ad;
extern Uint16 Ref0V_ad;

extern Uint16 HOT_state;
extern Uint16 BRV_state;
extern Uint16 PA_state;
extern Uint16 PB_state;
extern Uint16 PC_state;
extern int16  DRV_state;
extern int16  NOW_state;
extern int16  LAST_state;

extern Uint16 IU_offset;
extern Uint16 IV_offset;
extern Uint16 DBVD_offset;
extern Uint16 Ref3V_offset;

extern int16  SRM_Direction;
extern int16  SRM_SPEED;
extern int16  SRM_ANGLE;
extern int16  SRM_PHASE;
extern int16  SRM_FIRST_RUN;

#define PU_H 	1
#define PU_L 	2
#define PV_H 	3
#define PV_L 	4
#define PW_H 	5
#define PW_L 	6


#define BR_UP 	GpioDataRegs.GPASET.bit.GPIO13=1
#define BR_DN 	GpioDataRegs.GPACLEAR.bit.GPIO13=1

#define FAN_UP 	GpioDataRegs.GPASET.bit.GPIO6=1
#define FAN_DN 	GpioDataRegs.GPACLEAR.bit.GPIO6=1

#define SS_UP	GpioDataRegs.GPASET.bit.GPIO7=1
#define SS_DN 	GpioDataRegs.GPACLEAR.bit.GPIO7=1

#define DRV_UP 	GpioDataRegs.GPASET.bit.GPIO8=1
#define DRV_DN 	GpioDataRegs.GPACLEAR.bit.GPIO8=1

#define HOT_GET()	GpioDataRegs.GPADAT.bit.GPIO9
#define BRV_GET()	GpioDataRegs.GPADAT.bit.GPIO10

#define PA_GET()	GpioDataRegs.GPADAT.bit.GPIO24
#define PB_GET()	GpioDataRegs.GPBDAT.bit.GPIO52
#define PC_GET()	GpioDataRegs.GPADAT.bit.GPIO25

#define IU_GET()		(AdcResult.ADCRESULT0 + AdcResult.ADCRESULT1 + AdcResult.ADCRESULT2 + AdcResult.ADCRESULT3)/4
#define IV_GET()		(AdcResult.ADCRESULT4 + AdcResult.ADCRESULT5 + AdcResult.ADCRESULT6 + AdcResult.ADCRESULT7)/4
#define DBVD_GET()	(AdcResult.ADCRESULT8 + AdcResult.ADCRESULT9 + AdcResult.ADCRESULT10 + AdcResult.ADCRESULT11)/4
#define Ref3V_GET()	(AdcResult.ADCRESULT12 + AdcResult.ADCRESULT13)/2
#define Ref0V_GET()	(AdcResult.ADCRESULT14 + AdcResult.ADCRESULT15)/2

#endif /* USER_SRD_PROJECT_H_ */

//===========================================================================
// No more.
//===========================================================================
