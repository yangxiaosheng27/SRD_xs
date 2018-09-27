/*
 * 	FileName:	srm_error.c
 * 	Project:	SRM
 *
 *  Created on: 2018/3/22
 *  Author: 	yangxiaosheng
 */

#include "SRM_Project.h"        // User's Funtions

void Error_Checking(void);

int16  MAX_SPEED	= 1500;		// 1 means the MAX Speed is 1r/min
int16  MAX_U		= 1447;		// 1447 means the MAX U is about 350V
int16  MAX_I		= 300;		// 50 means the MAX I is about 3A

struct ERROR_STRUCT ERROR;

void My_Init_Error(void)
{
	ERROR.Code = 0;
	ERROR.Check_Count = 0;
	ERROR.CPU_Timer0_Remain = 0xFFFF;
}

void Error_Checking(void)
{
#ifdef	ENABLE_CONTROL
	ERROR.Check_Count++;

	if(IU_offset<1800||IU_offset>1900)											ERROR.Code |= 0x0001;
	if(IV_offset<1800||IV_offset>1900)											ERROR.Code |= 0x0002;
	if(DBVD_offset<1280||DBVD_offset>1400)										ERROR.Code |= 0x0004;
	if(Ref3V_offset<3650||Ref3V_offset>3800)									ERROR.Code |= 0x0008;
	if(IU_ad<(IU_offset-MAX_I)||IU_ad>(IU_offset+MAX_I))						ERROR.Code |= 0x0010;
	if(IV_ad<(IV_offset-MAX_I)||IV_ad>(IV_offset+MAX_I))						ERROR.Code |= 0x0020;
	if(DBVD_ad<(DBVD_offset-125)||DBVD_ad>MAX_U)								ERROR.Code |= 0x0040;
	if(Ref3V_ad<(Ref3V_offset-150)||Ref3V_ad>(Ref3V_offset+150))				ERROR.Code |= 0x0080;
	if(ERROR.CPU_Timer0_Remain < 0x0010)										ERROR.Code |= 0x0100;
	if(SRM.Position>=2048*4)									ERROR.Code |= 0x0200;
	if(SRM.Speed<-10)															ERROR.Code |= 0x0400;
	if(SRM.Speed>MAX_SPEED||SRM.Speed<-MAX_SPEED)								ERROR.Code |= 0x0800;
	if(EPwm1Regs.CMPA.half.CMPA*EPwm1Regs.CMPB||EPwm2Regs.CMPA.half.CMPA*EPwm2Regs.CMPB||EPwm3Regs.CMPA.half.CMPA*EPwm3Regs.CMPB)
																				ERROR.Code |= 0x1000;
//	if(SRM_Direction!=0 && SRM_Direction!=1)									ERROR.Code |= 0x2000;
	if(LOGIC.State<-1 || LOGIC.State>5)											ERROR.Code |= 0x4000;

	if(ERROR.Code)
	{
		DRV_UP;									//H is disable
		DINT;    								// Disable Interrupts at the CPU level:
		CpuTimer0Regs.TCR.bit.TSS = 1;

		EPwm1Regs.CMPA.half.CMPA = 0;
		EPwm1Regs.CMPB			 = 0;
		EPwm2Regs.CMPA.half.CMPA = 0;
		EPwm2Regs.CMPB			 = 0;
		EPwm3Regs.CMPA.half.CMPA = 0;
		EPwm3Regs.CMPB			 = 0;

		BR_UP;									//H is disable
		SS_UP;									//H is disable
		FAN_UP;									//H is disable

#ifdef  ENABLE_iostring
		printf("\nERROR CODE : 0x%x !\n",ERROR.Code);

		if(IU_offset<1800||IU_offset>1900)											printf("IU_offset<1800||IU_offset>1900\n");
		if(IV_offset<1800||IV_offset>1900)											printf("IV_offset<1800||IV_offset>1900\n");
		if(DBVD_offset<1300||DBVD_offset>1400)										printf("DBVD_offset<1230||DBVD_offset>1350\n");
		if(Ref3V_offset<3650||Ref3V_offset>3800)									printf("Ref3V_offset<3650||Ref3V_offset>3800");
		if(IU_ad<(IU_offset-MAX_I)||IU_ad>(IU_offset+MAX_I))						printf("IU_ad<(IU_offset-MAX_I)||IU_ad>(IU_offset+MAX_I)\n");
		if(IV_ad<(IV_offset-MAX_I)||IV_ad>(IV_offset+MAX_I))						printf("IV_ad<(IV_offset-MAX_I)||IV_ad>(IV_offset+MAX_I)\n");
		if(DBVD_ad<(DBVD_offset-125)||DBVD_ad>MAX_U)								printf("DBVD_ad<(DBVD_offset-125)||DBVD_ad>MAX_U\n");
		if(Ref3V_ad<(Ref3V_offset-150)||Ref3V_ad>(Ref3V_offset+150))				printf("Ref3V_ad<(Ref3V_offset-150)||Ref3V_ad>(Ref3V_offset+150)\n");
		if(ERROR.CPU_Timer0_Remain < 0X0010)	printf("PieCtrlRegs.PIEIFR1.all | 0x40) == 0 && CpuTimer0Regs.TIM.all < 0X0010\n");
		if(SRM.Position>=2048*4)													printf("SRM_ANGLE>=3600\n");
		if(SRM.Speed<0)																printf("SRM.SPEED<0\n");
		if(SRM.Speed>MAX_SPEED||SRM.Speed<-MAX_SPEED)								printf("SRM.SPEED>MAX.SPEED||SRM.SPEED<-MAX.SPEED\n");
		if(EPwm1Regs.CMPA.half.CMPA*EPwm1Regs.CMPB||EPwm2Regs.CMPA.half.CMPA*EPwm2Regs.CMPB||EPwm3Regs.CMPA.half.CMPA*EPwm3Regs.CMPB)
			printf("EPwm1Regs.CMPA.half.CMPA*EPwm1Regs.CMPB||EPwm2Regs.CMPA.half.CMPA*EPwm2Regs.CMPB||EPwm3Regs.CMPA.half.CMPA*EPwm3Regs.CMPB\n");
		if(SRM_Direction!=0 && SRM_Direction!=1)									printf("SRM_Direction!=0 && SRM_Direction!=1\n");
		if(LOGIC.State<-1 || LOGIC.State>5)											printf("LOGIC.State<-1 || LOGIC.State>5\n");
#endif

		while(1);
	}
#else
	return;
#endif
}
