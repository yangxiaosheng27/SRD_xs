/*
 * srd_gpio.c
 *
 *  Created on: 2017Äê12ÔÂ28ÈÕ
 *      Author: yangxiaosheng
 */
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "SRD_Project.h"        // User's Funtions

void my_init_gpio()
{
	InitGpio();

	EALLOW;

	// PU_H
	GpioCtrlRegs.GPAPUD.bit.GPIO0  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO0  = 1;   	// GPIO = output
	// PU_L
	GpioCtrlRegs.GPAPUD.bit.GPIO1  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO1  = 1;   	// GPIO = output
	// PV_H
	GpioCtrlRegs.GPAPUD.bit.GPIO2  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO2  = 1;   	// GPIO = output
	// PV_L
	GpioCtrlRegs.GPAPUD.bit.GPIO3  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO3  = 1;   	// GPIO = output
	// PW_H
	GpioCtrlRegs.GPAPUD.bit.GPIO4  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO4  = 1;   	// GPIO = output
	// PW_L
	GpioCtrlRegs.GPAPUD.bit.GPIO5  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO5  = 1;   	// GPIO = output
	// BR
	GpioCtrlRegs.GPAPUD.bit.GPIO13  = 0;   	// Enable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO13  = 1;   	// GPIO = output

	// FAN
	GpioCtrlRegs.GPAPUD.bit.GPIO6  = 0;   	// Enable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO6  = 1;   	// GPIO = output
	// SS
	GpioCtrlRegs.GPAPUD.bit.GPIO7  = 0;   	// Enable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO7  = 1;   	// GPIO = output
	// DRV
	GpioCtrlRegs.GPAPUD.bit.GPIO8  = 0;   	// Enable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO8  = 1;   	// GPIO = output

	// HOT
	GpioCtrlRegs.GPAPUD.bit.GPIO9  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO9  = 0;   	// GPIO = input
	// BRV
	GpioCtrlRegs.GPAPUD.bit.GPIO10  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO10  = 0;   	// GPIO = input

	GpioCtrlRegs.GPACTRL.bit.QUALPRD3=1;  	// Qual period = SYSCLKOUT/2
	// PA
	GpioCtrlRegs.GPAQSEL2.bit.GPIO24= 2;   	// 6 samples
	GpioCtrlRegs.GPAPUD.bit.GPIO24  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO24  = 0;   	// GPIO = input
	// PB
	GpioCtrlRegs.GPBQSEL2.bit.GPIO52= 2;   	// 6 samples
	GpioCtrlRegs.GPBPUD.bit.GPIO52  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;  	// GPIO mode
	GpioCtrlRegs.GPBDIR.bit.GPIO52  = 0;   	// GPIO = input
	// PC
	GpioCtrlRegs.GPAQSEL2.bit.GPIO25= 2;   	// 6 samples
	GpioCtrlRegs.GPAPUD.bit.GPIO25  = 1;   	// Disable pullup on GPIO
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;  	// GPIO mode
	GpioCtrlRegs.GPADIR.bit.GPIO25  = 0;   	// GPIO = input

	EDIS;
}

//===========================================================================
// No more.
//===========================================================================
