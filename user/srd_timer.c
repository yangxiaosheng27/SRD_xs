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
   ConfigCpuTimer(&CpuTimer1, 90, 400);		// Configure CPU-Timer1, 90 is CPUFreq, 1000 is uSeconds
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

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

__interrupt void cpu_timer1_isr(void)
{
/*	if(CpuTimer1.InterruptCount<=4)
	{
		CpuTimer1.InterruptCount++;
		CURRENT.Expect += 5;
	}
	else
	{
		CpuTimer1.InterruptCount = 0;
		CURRENT.Expect = 40;
	}
*/
}


<<<<<<< HEAD
#pragma DATA_SECTION(SampData1,"SampData1");
volatile int16 SampData1[2000]={0};
#pragma DATA_SECTION(SampData2,"SampData2");
volatile int16 SampData2[2000]={0};
#pragma DATA_SECTION(SampData3,"SampData3");
volatile int16 SampData3[2000]={0};
=======
#pragma DATA_SECTION(Samp_Data1,"SampData1");
volatile int16 Samp_Data1[500]={0};
#pragma DATA_SECTION(Samp_Data2,"SampData2");
volatile int16 Samp_Data2[500]={0};
>>>>>>> parent of 6244d68... finish current control
void Sample1()
{
	static int16 Samp_count=0;
	if(Samp_count<=500)
	{
		Samp_count++;
<<<<<<< HEAD
		SampData1[Samp_count] = CURRENT.Error;
=======
		Samp_Data1[Samp_count] = CURRENT.Error;
>>>>>>> parent of 6244d68... finish current control
	}
//	else Samp_count = 0;
}
void Sample2()
{
	static int16 Samp_count=0;
<<<<<<< HEAD
	if(Samp_count<=2000)
	{
		Samp_count++;
		SampData2[Samp_count] = PWM.Duty;
	}
//	else Samp_count = 0;
}
void Sample3()
{
	static int16 Samp_count=0;
	if(Samp_count<=2000)
	{
		Samp_count++;
		SampData3[Samp_count] = CURRENT.Integral;
=======
	if(Samp_count<=500)
	{
		Samp_count++;
		Samp_Data2[Samp_count] = IU_ad;
>>>>>>> parent of 6244d68... finish current control
	}
//	else Samp_count = 0;
}


//===========================================================================
// No more.
//===========================================================================

