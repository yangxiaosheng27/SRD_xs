/*
 * 	FileName:	srd_adc.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/6/24
 *  Author: 	yangxiaosheng
 */

#include "SRD_Project.h"        // User's Funtions

__interrupt void eqep2_isr(void);		// Prototype statements for functions found within this file.

void My_Init_EQEP(void)
{
	InitEQep2Gpio();

    EQep2Regs.QUPRD=90000;         		// Unit Timer for 1kHz at 90 MHz SYSCLKOUT

    EQep2Regs.QDECCTL.bit.QSRC=00;      // QEP quadrature count mode

    EQep2Regs.QEPCTL.bit.FREE_SOFT=2;	// 不受仿真影响
    EQep2Regs.QEPCTL.bit.PCRM=00;       // PCRM=00 mode - QPOSCNT reset on index event
    EQep2Regs.QEPCTL.bit.UTE=1;         // Unit Timeout Enable
    EQep2Regs.QEPCTL.bit.QCLM=1;        // Latch on unit time out
    EQep2Regs.QPOSMAX= 2048*4-1;
    EQep2Regs.QEPCTL.bit.QPEN=1;        // QEP enable

    EQep2Regs.QCAPCTL.bit.UPPS=0;       // 1 for unit position
    EQep2Regs.QCAPCTL.bit.CCPS=1;       // 1/64 for CAP clock
    EQep2Regs.QCAPCTL.bit.CEN=1;        // QEP Capture Enable

    EQep2Regs.QEINT.bit.UTO = 1; 		// Enable interrupt
    EALLOW;  							// This is needed to write to EALLOW protected registers
    PieVectTable.EQEP2_INT = &eqep2_isr;
    EDIS;    							// This is needed to disable write to EALLOW protected registers
    IER |= M_INT5;							// Enable CPU int5 which is connected to EQEP2
    PieCtrlRegs.PIEIER5.bit.INTx2 = 1;		// Enable EQEP2 in the PIE: Group 5 interrupt 2
}

void eqep2_isr(void)
{
	Control_SRD_external_loop();
    EQep1Regs.QCLR.bit.UTO=1;                   // Clear interrupt flag
}

