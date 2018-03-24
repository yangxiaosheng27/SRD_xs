/*
 * 	FileName:	srd_pwm.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/25
 *      Author: yangxiaosheng
 */


#include "SRD_Project.h"        			// User's Funtions

void my_init_pwm();
void InitEPwm1Regs();
void InitEPwm2Regs();
void InitEPwm3Regs();

void my_init_pwm()
{
	InitEPwm1Gpio();
	InitEPwm2Gpio();
	InitEPwm3Gpio();

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;

	InitEPwm1Regs();
	InitEPwm2Regs();
	InitEPwm3Regs();

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}

void InitEPwm1Regs()
{

   // Setup TBCLK
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm1Regs.TBPRD = 1000;       // Set timer period
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
   EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV2;

   // Setup shadow register load on ZERO
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   // Set Compare values
   EPwm1Regs.CMPA.half.CMPA = 0;    // Set compare A value
   EPwm1Regs.CMPB = 0;              // Set Compare B value

   // Set actions
   EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM1A on Zero
   EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A, up count

   EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;            // Set PWM1B on Zero
   EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Clear PWM1B on event B, up count
}

void InitEPwm2Regs()
{
}

void InitEPwm3Regs()
{

}
