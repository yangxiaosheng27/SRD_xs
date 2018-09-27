/*
 * 	FileName:	srm_pwm.c
 * 	Project:	SRM
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */


#include "SRM_Project.h"        			// User's Funtions

void My_Init_PWM();
void InitEPwm1Regs();
void InitEPwm2Regs();
void InitEPwm3Regs();

void My_Init_PWM()
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

	EPwm1Regs.CMPA.half.CMPA = 0;
	EPwm1Regs.CMPB			 = 0;
	EPwm2Regs.CMPA.half.CMPA = 0;
	EPwm2Regs.CMPB			 = 0;
	EPwm3Regs.CMPA.half.CMPA = 0;
	EPwm3Regs.CMPB			 = 0;

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}

#define PWM_MAX 1000	// Set timer period (2250 means PWM period = 20kHz, 1000 means 45kHz)
void InitEPwm1Regs()
{
   // Setup TBCLK
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm1Regs.TBPRD = PWM_MAX;                    // Set timer period
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT (90MHz)
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
	// Setup TBCLK
	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
	EPwm2Regs.TBPRD = PWM_MAX;                    // Set timer period
	EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
	EPwm2Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
	EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
	EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV2;

	// Setup shadow register load on ZERO
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	// Set Compare values
	EPwm2Regs.CMPA.half.CMPA = 0;    // Set compare A value
	EPwm2Regs.CMPB = 0;              // Set Compare B value

	// Set actions
	EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM1A on Zero
	EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A, up count

	EPwm2Regs.AQCTLB.bit.ZRO = AQ_SET;            // Set PWM1B on Zero
	EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Clear PWM1B on event B, up count
}

void InitEPwm3Regs()
{
	// Setup TBCLK
	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
	EPwm3Regs.TBPRD = PWM_MAX;       			   // Set timer period
	EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
	EPwm3Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
	EPwm3Regs.TBCTR = 0x0000;                  // Clear counter
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
	EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV2;

	// Setup shadow register load on ZERO
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	// Set Compare values
	EPwm3Regs.CMPA.half.CMPA = 0;    // Set compare A value
	EPwm3Regs.CMPB = 0;              // Set Compare B value

	// Set actions
	EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM1A on Zero
	EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A, up count

	EPwm3Regs.AQCTLB.bit.ZRO = AQ_SET;            // Set PWM1B on Zero
	EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Clear PWM1B on event B, up count

}
