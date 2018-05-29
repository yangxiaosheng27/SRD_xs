/*
 * 	FileName:	SRD_Project.h
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */

#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "F2806x_Examples.h"   	// F2806x Examples Include File

#ifndef USER_SRD_PROJECT_H_
#define USER_SRD_PROJECT_H_

extern void Get_AD_Offset(void);
extern void My_Init_ADC(void);
extern void My_Init_Cputimer(void);
extern void My_Init_GPIO(void);
extern void My_Init_PWM(void);

extern void Error_Checking(void);

extern void Get_State(void);
extern void Get_Position(void);

extern void Init_SRD(void);
extern void Control_SRD(void);
extern void IGBT_Control(void);
extern void SPEED_Control(void);
extern void TORQUE_Control(void);
extern void CURRENT_Control(void);
extern void PWM_Control(void);

struct IGBT_STRUCT{
	int16	State;
	int16	State_1;		// State_n means State*z^(-n)
	int16	Turn;
	int16	Count;
};
struct SPEED_STRUCT{
	int16	Expect;
	int16	MAX;
	int16	Kp;
	int16 	Kd;
	int16 	Error;
	int16 	Error_1;		// Error_n means Error*z^(-n)
	int16 	Differential;
};
struct TORQUE_STRUCT{
	int16  	Expect;
	int16  	MAX;
};
struct CURRENT_STRUCT{
	int16  	Expect;
	int16  	MAX;
	int16 	Kp;
	int16 	Ki;
	int16 	Error;
	int16 	Error_1;
	int16 	Sample;
	int16 	Integral;
};
struct PWM_STRUCT{
	int16  	MAX;
	int16 	Duty;
};

extern struct SPEED_STRUCT 		SPEED;
extern struct TORQUE_STRUCT 	TORQUE;
extern struct CURRENT_STRUCT	CURRENT;
extern struct PWM_STRUCT		PWM;

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
