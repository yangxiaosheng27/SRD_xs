/*
 * 	FileName:	main.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */
#include "SRD_Project.h"    // User's Funtions and Definitions

main()
{
	InitSysCtrl();			// PLL, WatchDog, enable Peripheral Clocks
    DINT;    				// Disable Interrupts at the CPU level:
	InitPieCtrl();			// Initialize the PIE control registers to their default state.
	IER = 0x0000;			// Disable CPU interrupts
	IFR = 0x0000;			// clear all CPU interrupt flags
	InitPieVectTable();		// Initialize the PIE vector table with pointers to the shell Interrupt	my_init_srd();
	Init_SRD();				// Initialize the SRM
	EINT;          			// Enable Global interrupt INTM
	ERTM;          			// Enable Global realtime interrupt DBGM

	while(1)
	{
							//Control_SRD() will run in cpu_timer0_isr() and eqep2_isr()
	}
}
//===========================================================================
// No more.
//===========================================================================
