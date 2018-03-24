/*
 * 	FileName:	srd_timer.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/1/16
 *  Author: 	yangxiaosheng
 */
#include "SRD_Project.h"        			// User's Funtions

__interrupt void cpu_timer0_isr(void);		// Prototype statements for functions found within this file.
__interrupt void cpu_timer1_isr(void);		// Prototype statements for functions found within this file.

int16 done=0;
int16 data[1000]={0};
//int16 data1[3000]={0};

void my_init_cputimer()
{
   EALLOW;  								// This is needed to write to EALLOW protected registers
   PieVectTable.TINT0 = &cpu_timer0_isr;
   PieVectTable.TINT1 = &cpu_timer1_isr;
   EDIS;    								// This is needed to disable write to EALLOW protected registers
   InitCpuTimers();   						// For this example, only initialize the Cpu Timers
   ConfigCpuTimer(&CpuTimer0, 90, 20);		// Configure CPU-Timer0, 90 is CPUFreq, 20 is uSeconds
   ConfigCpuTimer(&CpuTimer1, 90, 1000);	// Configure CPU-Timer1, 90 is CPUFreq, 1000 is uSeconds
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

	get_state();
	get_position();
//	phase_control();
//	pid_control();
//	hysteresis_control();


	error_checking();


	// Acknowledge this interrupt to receive more interrupts from group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


__interrupt void cpu_timer1_isr(void)
{
   if(done==0&&FIRST_RUN==0)
   {
	   CpuTimer1.InterruptCount++;
	   data[CpuTimer1.InterruptCount] 	= I_abs;
//	   data1[CpuTimer1.InterruptCount] 	= SRM_STATE;
   }
   if(CpuTimer1.InterruptCount>=5000)done=1;

}
//===========================================================================
// No more.
//===========================================================================

