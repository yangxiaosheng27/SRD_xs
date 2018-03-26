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
   ConfigCpuTimer(&CpuTimer1, 90, 500);		// Configure CPU-Timer1, 90 is CPUFreq, 1000 is uSeconds
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
//	Sample3();
}


#pragma DATA_SECTION(Ierror_Data,"SampData1");
volatile int16 Ierror_Data[2000]={0};
#pragma DATA_SECTION(IU_Data,"SampData2");
volatile int16 IU_Data[2000]={0};
#pragma DATA_SECTION(Speed_Data,"SampData3");
volatile int16 Speed_Data[2000]={0};
void Sample1()
{
	static int16 Samp_count=0;
	if(Samp_count<=2000)
	{
		Samp_count++;
		Ierror_Data[Samp_count] = CURRENT.Error;
	}
	else Samp_count = 0;
}
void Sample2()
{
	static int16 Samp_count=0;
	if(Samp_count<=2000)
	{
		Samp_count++;
		IU_Data[Samp_count] = IU_ad;
	}
	else Samp_count = 0;
}
void Sample3()
{
	static int16 Samp_count=0;
	if(Samp_count<=2000)
	{
		Samp_count++;
		Speed_Data[Samp_count] = SRM_PHASE;
	}
	else Samp_count = 0;
}


//===========================================================================
// No more.
//===========================================================================

