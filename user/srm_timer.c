/*
 * 	FileName:	srm_timer.c
 * 	Project:	SRM
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */
#include "SRM_Project.h"        			// User's Funtions

// SRM.Speed	TORQUE_AB.Expect	CURRENT.Ia_sam	TORQUE_AB.Error	alpha[1-1]
#define Samp_Data1 CURRENT.Ia_sam
#define Samp_Data2 SRM.Speed
#define Samp_Data3 TORQUE.Sample
//#define Samp_Data4 alpha[1-1]
//#define Samp_Data1 a1
//#define Samp_Data2 a2
//#define Samp_Data3 b0
//#define Samp_Data4 b1

#define Data_Length1 7000
#define Data_Length2 1000
#define Data_Length3 1000
#define Data_Length4 0
#define Data_Type int16
//#define Sample_Loop

void My_Init_Cputimer();
void Sample1();
void Sample2();
void Sample3();
void Sample4();
void Test_SRM();
__interrupt void cpu_timer0_isr(void);		// Prototype statements for functions found within this file.
__interrupt void cpu_timer1_isr(void);		// Prototype statements for functions found within this file.

#pragma CODE_SECTION(cpu_timer0_isr, "ramfuncs");       // for real-time operation , so must be run in the RAM
__interrupt void cpu_timer0_isr(void)
{
	CpuTimer0.InterruptCount++;
	while (AdcRegs.ADCINTFLG.bit.ADCINT1 == 0){}		//Wait for ADCINT1
	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;   			//Must clear ADCINT1 flag since INT1CONT = 0

	Control_SRM_internal_loop();
//	Test_SRM();		//for test
/*	Sample1();
	Sample2();
	Sample3();
	Sample4();*/
	Sample1();

	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	ERROR.CPU_Timer0_Remain = CpuTimer0Regs.TIM.all;
}

#pragma CODE_SECTION(cpu_timer1_isr, "ramfuncs");       // for real-time operation , so must be run in the RAM
__interrupt void cpu_timer1_isr(void)
{
//	static int16 SPEED_UP = 0;
	IER |= M_INT1;                 // Set "global" priority for cputimer0_isr()
	EINT;
	CpuTimer1.InterruptCount++;
	if(CpuTimer1.InterruptCount>=3000)Enable_Identification=1;
	if(CpuTimer1.InterruptCount>=3000)Enable_AdaptControl=1;
	if(CpuTimer1.InterruptCount>=1000000)CpuTimer1.InterruptCount = 0;

	Sample2();
	Sample3();
}


#pragma CODE_SECTION(eqep2_isr, "ramfuncs");      // for real-time operation , so must be run in the RAM
Uint32 EQEP2_InterruptCount = 0;
void eqep2_isr(void)	// 1kHz
{
	IER |= M_INT1;                 // Set "global" priority for cputimer0_isr()
	IER |= M_INT13;                // Set "global" priority for cputimer1_isr()
	EINT;
	EQEP2_InterruptCount++;

	Control_SRM_external_loop();
/*	if(Enable_Identification)
	{
		Sample1();
		Sample2();
		Sample3();
		Sample4();
	}*/

    EQep2Regs.QCLR.bit.UTO=1;                   // Clear interrupt flag
    EQep2Regs.QCLR.bit.INT=1;                   // Clear interrupt flag
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
}


void My_Init_Cputimer()
{
   EALLOW;  								// This is needed to write to EALLOW protected registers
   PieVectTable.TINT0 = &cpu_timer0_isr;
   PieVectTable.TINT1 = &cpu_timer1_isr;
   EDIS;    								// This is needed to disable write to EALLOW protected registers
   InitCpuTimers();   						// For this example, only initialize the Cpu Timers
   ConfigCpuTimer(&CpuTimer0, 90, 50);		// Configure CPU-Timer0, 90 is CPUFreq(MHz), 50 is Time(uSeconds)
   ConfigCpuTimer(&CpuTimer1, 90, 1000);	// Configure CPU-Timer1, 90 is CPUFreq(MHz), 1000 is Time(uSeconds)
   IER |= M_INT1;							// Enable CPU int1 which is connected to CPU-Timer 0
   IER |= M_INT13;							// Enable CPU int13 which is connected to CPU-Timer 1
   PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		// Enable TINT0 in the PIE: Group 1 interrupt 7
   CpuTimer0Regs.TCR.all = 0x4000; 			// Use write-only instruction to set TSS bit = 0
   CpuTimer1Regs.TCR.all = 0x4000; 			// Use write-only instruction to set TSS bit = 0
}

#ifdef Samp_Data1
#pragma DATA_SECTION(SampData1,"SampData1");
volatile Data_Type SampData1[Data_Length1]={0};
#endif

#ifdef Samp_Data2
#pragma DATA_SECTION(SampData2,"SampData2");
volatile Data_Type SampData2[Data_Length2]={0};
#endif

#ifdef Samp_Data3
#pragma DATA_SECTION(SampData3,"SampData3");
volatile Data_Type SampData3[Data_Length3]={0};
#endif

#ifdef Samp_Data4
#pragma DATA_SECTION(SampData4,"SampData4");
volatile Data_Type SampData4[Data_Length4]={0};
#endif

void Sample1()
{
#ifdef Samp_Data1
	static int16 Samp_count = 0;
#ifdef Sample_Loop
	if(Samp_count>Data_Length1)	Samp_count -= Data_Length;
#endif
#endif

#ifdef Samp_Data1
	if(Samp_count<=Data_Length1)
	{
		Samp_count++;
		SampData1[Samp_count] = Samp_Data1;
	}
#endif
}
void Sample2()
{
#ifdef Samp_Data2
	static int16 Samp_count = 0;
#ifdef Sample_Loop
	if(Samp_count>Data_Length2)	Samp_count -= Data_Length;
#endif
#endif

#ifdef Samp_Data2
	if(Samp_count<=Data_Length2)
	{
		Samp_count++;
		SampData2[Samp_count] = Samp_Data2;
	}
#endif
}
void Sample3()
{
#ifdef Samp_Data3
	static int16 Samp_count = 0;
#ifdef Sample_Loop
	if(Samp_count>Data_Length3)	Samp_count -= Data_Length;
#endif
#endif

#ifdef Samp_Data3
	if(Samp_count<=Data_Length3)
	{
		Samp_count++;
		SampData3[Samp_count] = Samp_Data3;
	}
#endif
}

void Sample4()
{
#ifdef Samp_Data4
	static int16 Samp_count = 0;
#ifdef Sample_Loop
	if(Samp_count>Data_Length4)	Samp_count -= Data_Length;
#endif
#endif

#ifdef Samp_Data4
	if(Samp_count<=Data_Length4)
	{
		Samp_count++;
		SampData4[Samp_count] = Samp_Data4;
	}
#endif
}

/*void Test_SRM()
{
//	SRM_FIRST_RUN = 0;
	LOGIC.State = 0;
	Get_Sensor();
	SRM_PHASE =	SRM.Phase;
	SRM.Phase =	30;
	TORQUE_Calculate();
	TORQUE.Expect = 1000;
	TORQUE_Distribution();
	TORQUE_Control();
	SRM.Speed =	1;
	Error_Checking();
	IGBT_Control();
}*/

//===========================================================================
// No more.
//===========================================================================

