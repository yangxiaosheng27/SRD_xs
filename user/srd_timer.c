/*
 * 	FileName:	srd_timer.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */
#include "SRD_Project.h"        			// User's Funtions

void My_Init_Cputimer();
void Sample1();
void Sample2();
void Sample3();
void Test_SRD();
__interrupt void cpu_timer0_isr(void);		// Prototype statements for functions found within this file.
__interrupt void cpu_timer1_isr(void);		// Prototype statements for functions found within this file.

void My_Init_Cputimer()
{
   EALLOW;  								// This is needed to write to EALLOW protected registers
   PieVectTable.TINT0 = &cpu_timer0_isr;
   PieVectTable.TINT1 = &cpu_timer1_isr;
   EDIS;    								// This is needed to disable write to EALLOW protected registers
   InitCpuTimers();   						// For this example, only initialize the Cpu Timers
   ConfigCpuTimer(&CpuTimer0, 90, 20);		// Configure CPU-Timer0, 90 is CPUFreq, 20 is uSeconds
   ConfigCpuTimer(&CpuTimer1, 90, 50);		// Configure CPU-Timer1, 90 is CPUFreq, 1000 is uSeconds
   IER |= M_INT1;							// Enable CPU int1 which is connected to CPU-Timer 0
   IER |= M_INT13;							// Enable CPU int13 which is connected to CPU-Timer 1
   PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		// Enable TINT0 in the PIE: Group 1 interrupt 7
   CpuTimer0Regs.TCR.all = 0x4000; 			// Use write-only instruction to set TSS bit = 0
   CpuTimer1Regs.TCR.all = 0x4000; 			// Use write-only instruction to set TSS bit = 0
}


__interrupt void cpu_timer0_isr(void)
{
	CpuTimer0.InterruptCount++;
	while (AdcRegs.ADCINTFLG.bit.ADCINT1 == 0){}		//Wait for ADCINT1
	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;   			//Must clear ADCINT1 flag since INT1CONT = 0

	Control_SRD();
	Sample1();
	Sample2();
	Sample3();

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer1_isr(void)
{
	CpuTimer1.InterruptCount++;
//	Sample1();
//	Sample2();
//	Sample3();
}

#define Data_Length 3000
#pragma DATA_SECTION(SampData1,"SampData1");
volatile int16 SampData1[Data_Length]={0};
#pragma DATA_SECTION(SampData2,"SampData2");
volatile int16 SampData2[Data_Length]={0};
#pragma DATA_SECTION(SampData3,"SampData3");
volatile int16 SampData3[Data_Length]={0};
void Sample1()
{
	static int16 Samp_count=0;
	if(Samp_count<=Data_Length)
	{
		Samp_count++;
		SampData1[Samp_count] = SRM_PHASE;
	}
	else Samp_count = 0;
}
void Sample2()
{
	static int16 Samp_count=0;
	if(Samp_count<=Data_Length)
	{
		Samp_count++;
		SampData2[Samp_count] = TORQUE_AB.Expect;
	}
	else Samp_count = 0;
}
void Sample3()
{
	static int16 Samp_count=0;
	if(Samp_count<=Data_Length)
	{
		Samp_count++;
		SampData3[Samp_count] = TORQUE_AB.Sample;
	}
	else Samp_count = 0;
}

void Test_SRD()
{
	PWM.NOW		=	700;

	EPwm1Regs.CMPA.half.CMPA = PWM.NOW;
	EPwm1Regs.CMPB			 = 0;
	EPwm2Regs.CMPA.half.CMPA = 0;
	EPwm2Regs.CMPB			 = PWM.NOW;
	EPwm3Regs.CMPA.half.CMPA = 0;
	EPwm3Regs.CMPB			 = 0;
}

//===========================================================================
// No more.
//===========================================================================

