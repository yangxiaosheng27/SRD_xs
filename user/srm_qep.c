/*
 * 	FileName:	srm_adc.c
 * 	Project:	SRM
 *
 *  Created on: 2018/6/24
 *  Author: 	yangxiaosheng
 */

#include "SRM_Project.h"        // User's Funtions

//__interrupt void eqep2_isr(void);		// Prototype statements for functions found within this file.

void My_Init_EQEP(void)
{
	InitEQep2Gpio();

    EQep2Regs.QUPRD=90000L-1;         	// Unit Timer for 1kHz at 90 MHz SYSCLKOUT

    EQep2Regs.QDECCTL.bit.QSRC=00;      // QEP quadrature count mode

    EQep2Regs.QEPCTL.bit.FREE_SOFT=2;	// 不受仿真影响
    EQep2Regs.QEPCTL.bit.PCRM=00;       // PCRM=00 mode - QPOSCNT reset on index event
    EQep2Regs.QEPCTL.bit.UTE=1;         // Unit Timeout Enable
    EQep2Regs.QEPCTL.bit.QCLM=0;        // Latch on reading QPOSCNT
    EQep2Regs.QPOSMAX= 2048*4-1;
    EQep2Regs.QEPCTL.bit.QPEN=1;        // QEP enable

    EQep2Regs.QCAPCTL.bit.UPPS=0;       // 1:1 for unit position
    EQep2Regs.QCAPCTL.bit.CCPS=0;       // 1:1 for CAP clock to SYS clock
    EQep2Regs.QCAPCTL.bit.CEN=1;        // QEP Capture Enable

    EQep2Regs.QEINT.bit.UTO = 1; 		// Enable interrupt
    EALLOW;  							// This is needed to write to EALLOW protected registers
    PieVectTable.EQEP2_INT = &eqep2_isr;
    EDIS;    							// This is needed to disable write to EALLOW protected registers
    IER |= M_INT5;							// Enable CPU int5 which is connected to EQEP2
    PieCtrlRegs.PIEIER5.bit.INTx2 = 1;		// Enable EQEP2 in the PIE: Group 5 interrupt 2
}

/*	move to SRM_timer.c
void eqep2_isr(void)
{
	static Uint16 EQEP2_InterruptCount = 0;
	IER |= M_INT1;                 // Set "global" priority for cputimer0_isr()
	EINT;
	EQEP2_InterruptCount++;
	Control_SRM_external_loop();
    EQep2Regs.QCLR.bit.UTO=1;                   // Clear interrupt flag
    EQep2Regs.QCLR.bit.INT=1;                   // Clear interrupt flag
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;
}*/

