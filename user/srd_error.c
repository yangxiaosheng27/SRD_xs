/*
 * 	FileName:	srd_error.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/22
 *      Author: yangxiaosheng
 */

#include "SRD_Project.h"        // User's Funtions

void error_checking(void);

Uint16 ERROR		= 0;

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
