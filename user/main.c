/*
 * 	FileName:	main.c
 * 	Project:	SRM
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */
#include "SRM_Project.h"    // User's Funtions and Definitions

main()
{
	InitSysCtrl();			// PLL, WatchDog, enable Peripheral Clocks
    DINT;    				// Disable Interrupts at the CPU level.
	InitPieCtrl();			// Initialize the PIE control registers to their default state.
	IER = 0x0000;			// Disable CPU interrupts
	IFR = 0x0000;			// clear all CPU interrupt flags
	InitPieVectTable();		// Initialize the PIE vector table with pointers to the shell Interrupt	my_init_SRM();
	memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (Uint32)&RamfuncsLoadSize);
	                        // The  RamfuncsLoadStart, RamfuncsLoadSize, and RamfuncsRunStart
                            // symbols are created by the linker. Refer to the F2808.cmd file.
	InitFlash();            // Call Flash Initialization to setup flash waitstates
	                        // This function must reside in RAM
	Init_SRM();				// Initialize the SRM
	EINT;          			// Enable Global interrupt INTM
	ERTM;          			// Enable Global realtime interrupt DBGM

	while(1)
	{
							//Control_SRM() will run in cpu_timer0_isr()
	}
}
//===========================================================================
// No more.
//===========================================================================
