/*
 * 	FileName:	srd_error.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/22
 *  Author: 	yangxiaosheng
 */

#include "SRD_Project.h"        // User's Funtions
#include <stdio.h>

void Error_Checking(void);

Uint16 ERROR		= 0;
int16  MAX_SPEED	= 1500;		// 1 means the MAX Speed is 1r/min
int16  MAX_U		= 1447;		// 1447 means the MAX U is about 350V
int16  MAX_I		= 150;		// 50 means the MAX I is about 3A

void Error_Checking(void)
{
	if(IU_offset<1800||IU_offset>1900)											ERROR |= 0x0001;
	if(IV_offset<1800||IV_offset>1900)											ERROR |= 0x0002;
	if(DBVD_offset<1230||DBVD_offset>1350)										ERROR |= 0x0004;
	if(Ref3V_offset<3650||Ref3V_offset>3800)									ERROR |= 0x0008;
	if(IU_ad<(IU_offset-MAX_I)||IU_ad>(IU_offset+MAX_I))						ERROR |= 0x0010;
	if(IV_ad<(IV_offset-MAX_I)||IV_ad>(IV_offset+MAX_I))						ERROR |= 0x0020;
	if(DBVD_ad<(DBVD_offset-125)||DBVD_ad>MAX_U)								ERROR |= 0x0040;
	if(Ref3V_ad<(Ref3V_offset-150)||Ref3V_ad>(Ref3V_offset+150))				ERROR |= 0x0080;
	if((PieCtrlRegs.PIEIFR1.all | 0x40) == 0 && CpuTimer0Regs.TIM.all < 0X0100)	ERROR |= 0x0100;
	if(SRM_ANGLE<0||SRM_ANGLE>=3600)											ERROR |= 0x0200;
//	if(SRM_SPEED<0)																ERROR |= 0x0400;
	if(SRM_SPEED>MAX_SPEED||SRM_SPEED<-MAX_SPEED)								ERROR |= 0x0800;
//	if(HOT_state == 1)


	if(ERROR)
	{
		DINT;    				// Disable Interrupts at the CPU level:
		CpuTimer0Regs.TCR.bit.TSS = 1;
		DRV_UP;					//H is disable

		BR_UP;					//H is disable
		SS_UP;					//H is disable
		FAN_UP;					//H is disable
		printf("ERROR:0x%x!\n",ERROR);
		while(1);
	}
}
