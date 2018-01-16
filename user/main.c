/*
 * 	FileName:	main.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2017Äê12ÔÂ28ÈÕ
 *  Author: 	yangxiaosheng
 */
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "SRD_Project.h"        // User's Funtions

main()
{

	InitSysCtrl();			// PLL, WatchDog, enable Peripheral Clocks
    DINT;    				// Disable Interrupts at the CPU level:
	InitPieCtrl();			// Initialize the PIE control registers to their default state.
	IER = 0x0000;			// Disable CPU interrupts
	IFR = 0x0000;			// clear all CPU interrupt flags
	InitPieVectTable();		// Initialize the PIE vector table with pointers to the shell Interrupt	my_init_srd();
	my_init_srd();			// Initialize the SRM
	EINT;          			// Enable Global interrupt INTM
	ERTM;          			// Enable Global realtime interrupt DBGM

	while(1);
}
//===========================================================================
// No more.
//===========================================================================
