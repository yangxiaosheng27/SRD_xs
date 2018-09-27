/*
 * 	FileName:	srm_adc.c
 * 	Project:	SRM
 *
 *  Created on: 2018/3/26
 *  Author: 	yangxiaosheng
 */
#include "SRM_Project.h"        // User's Funtions
#define ADC_usDELAY  1000L

void My_Init_ADC();
void Get_AD_Offset(void);
void My_AdcChanSelect(Uint16 ch_no);	//be used in get_AD_offset(void);
Uint16 My_AdcConversion(void);			//be used in get_AD_offset(void);

void My_Init_ADC()
{
    InitAdc();
    AdcOffsetSelfCal();
	Get_AD_Offset();

// Configure ADC
	EALLOW;

    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP 	= 1;	// Enable non-overlap mode
	AdcRegs.ADCCTL1.bit.INTPULSEPOS		= 1;	// ADCINTs trigger at end of conversion
	AdcRegs.INTSEL1N2.bit.INT1E     	= 1;	// Enabled ADCINT1
	AdcRegs.INTSEL1N2.bit.INT1CONT  	= 0;	// Disable ADCINT1 Continuous mode
    AdcRegs.INTSEL1N2.bit.INT1SEL 		= 15;   // setup EOC15 to trigger ADCINT1 to fire
    AdcRegs.ADCCTL1.bit.ADCREFSEL 		= 0;    // Select internal reference mode
    AdcRegs.ADCCTL1.bit.VREFLOCONV 		= 1;    // Select VREFLO internal connection on B5

    AdcRegs.ADCSOC0CTL.bit.CHSEL	= 7;		// Select to ADCINA7(IU)
    AdcRegs.ADCSOC1CTL.bit.CHSEL	= 7;
    AdcRegs.ADCSOC2CTL.bit.CHSEL	= 7;
    AdcRegs.ADCSOC3CTL.bit.CHSEL	= 7;
    AdcRegs.ADCSOC4CTL.bit.CHSEL	= 9;		// Select to ADCINB1(IV)
    AdcRegs.ADCSOC5CTL.bit.CHSEL	= 9;
    AdcRegs.ADCSOC6CTL.bit.CHSEL	= 9;
    AdcRegs.ADCSOC7CTL.bit.CHSEL	= 9;
    AdcRegs.ADCSOC8CTL.bit.CHSEL	= 2;		// Select to ADCINA2(DBVD)
    AdcRegs.ADCSOC9CTL.bit.CHSEL	= 2;
    AdcRegs.ADCSOC10CTL.bit.CHSEL	= 2;
    AdcRegs.ADCSOC11CTL.bit.CHSEL	= 2;
    AdcRegs.ADCSOC12CTL.bit.CHSEL	= 10;		// Select to ADCINB2(Ref3V)
    AdcRegs.ADCSOC13CTL.bit.CHSEL	= 10;
    AdcRegs.ADCSOC14CTL.bit.CHSEL	= 13;		// Select to ADCINB5(Ref0V)
    AdcRegs.ADCSOC15CTL.bit.CHSEL	= 13;

    AdcRegs.ADCSOC0CTL.bit.ACQPS  = 6;			// Set SOC1 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)
    AdcRegs.ADCSOC1CTL.bit.ACQPS  = 6;
    AdcRegs.ADCSOC2CTL.bit.ACQPS  = 6;
    AdcRegs.ADCSOC3CTL.bit.ACQPS  = 6;
    AdcRegs.ADCSOC4CTL.bit.ACQPS  = 6;
    AdcRegs.ADCSOC5CTL.bit.ACQPS  = 6;
    AdcRegs.ADCSOC6CTL.bit.ACQPS  = 6;
    AdcRegs.ADCSOC7CTL.bit.ACQPS  = 6;
    AdcRegs.ADCSOC8CTL.bit.ACQPS  = 6;
    AdcRegs.ADCSOC9CTL.bit.ACQPS  = 6;
    AdcRegs.ADCSOC10CTL.bit.ACQPS = 6;
    AdcRegs.ADCSOC11CTL.bit.ACQPS = 6;
    AdcRegs.ADCSOC12CTL.bit.ACQPS = 6;
    AdcRegs.ADCSOC13CTL.bit.ACQPS = 6;
    AdcRegs.ADCSOC14CTL.bit.ACQPS = 6;
    AdcRegs.ADCSOC15CTL.bit.ACQPS = 6;

    AdcRegs.ADCSOC0CTL.bit.TRIGSEL 	= 1;   		// start trigger on CPUTimer0
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL 	= 1;
    AdcRegs.ADCSOC2CTL.bit.TRIGSEL 	= 1;
    AdcRegs.ADCSOC3CTL.bit.TRIGSEL 	= 1;
    AdcRegs.ADCSOC4CTL.bit.TRIGSEL 	= 1;
    AdcRegs.ADCSOC5CTL.bit.TRIGSEL 	= 1;
    AdcRegs.ADCSOC6CTL.bit.TRIGSEL 	= 1;
    AdcRegs.ADCSOC7CTL.bit.TRIGSEL 	= 1;
    AdcRegs.ADCSOC8CTL.bit.TRIGSEL 	= 1;
    AdcRegs.ADCSOC9CTL.bit.TRIGSEL 	= 1;
    AdcRegs.ADCSOC10CTL.bit.TRIGSEL	= 1;
    AdcRegs.ADCSOC11CTL.bit.TRIGSEL = 1;
    AdcRegs.ADCSOC12CTL.bit.TRIGSEL = 1;
    AdcRegs.ADCSOC13CTL.bit.TRIGSEL = 1;
    AdcRegs.ADCSOC14CTL.bit.TRIGSEL = 1;
    AdcRegs.ADCSOC15CTL.bit.TRIGSEL = 1;

	EDIS;
}


void Get_AD_Offset(void)
{
    EALLOW;
    My_AdcChanSelect(7);                                //Select ADCINA7(IU) for all SOC
    IU_ad = IU_offset = My_AdcConversion();                     //Capture ADC conversion on ADCINA7(IU)
    My_AdcChanSelect(9);                                //Select ADCINB1(IV) for all SOC
    IV_ad = IV_offset = My_AdcConversion();                     //Capture ADC conversion on ADCINB1(IV)
    My_AdcChanSelect(2);                                //Select ADCINA2(DBVD) for all SOC
    DBVD_ad = DBVD_offset = My_AdcConversion();                   //Capture ADC conversion on ADCINA2(DBVD)
    My_AdcChanSelect(10);                               //Select ADCINB2(Ref3V) for all SOC
    Ref3V_ad = Ref3V_offset = My_AdcConversion();                  //Capture ADC conversion on ADCINB2(Ref3V)
    EDIS;
}

void My_AdcChanSelect(Uint16 ch_no)
{
    AdcRegs.ADCSOC0CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC1CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC2CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC3CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC4CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC5CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC6CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC7CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC8CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC9CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC10CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC11CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC12CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC13CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC14CTL.bit.CHSEL= ch_no;
    AdcRegs.ADCSOC15CTL.bit.CHSEL= ch_no;
} //end AdcChanSelect

#pragma CODE_SECTION(My_AdcConversion, "ramfuncs");       // for real-time operation , so must be run in the RAM
Uint16 My_AdcConversion(void)
{
    Uint16 index, SampleSize, Mean, ACQPS_Value;
    Uint32 Sum;

    index       = 0;            //initialize index to 0
    SampleSize  = 256;          //set sample size to 256 (**NOTE: Sample size must be multiples of 2^x where is an integer >= 4)
    Sum         = 0;            //set sum to 0
    Mean        = 999;          //initialize mean to known value

    //Set the ADC sample window to the desired value (Sample window = ACQPS + 1)
    ACQPS_Value = 6;
    AdcRegs.ADCSOC0CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC1CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC2CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC3CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC4CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC5CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC6CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC7CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC8CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC9CTL.bit.ACQPS  = ACQPS_Value;
    AdcRegs.ADCSOC10CTL.bit.ACQPS = ACQPS_Value;
    AdcRegs.ADCSOC11CTL.bit.ACQPS = ACQPS_Value;
    AdcRegs.ADCSOC12CTL.bit.ACQPS = ACQPS_Value;
    AdcRegs.ADCSOC13CTL.bit.ACQPS = ACQPS_Value;
    AdcRegs.ADCSOC14CTL.bit.ACQPS = ACQPS_Value;
    AdcRegs.ADCSOC15CTL.bit.ACQPS = ACQPS_Value;


    //Enable ping-pong sampling

    // Enabled ADCINT1 and ADCINT2
    AdcRegs.INTSEL1N2.bit.INT1E = 1;
    AdcRegs.INTSEL1N2.bit.INT2E = 1;

    // Disable continuous sampling for ADCINT1 and ADCINT2
    AdcRegs.INTSEL1N2.bit.INT1CONT = 0;
    AdcRegs.INTSEL1N2.bit.INT2CONT = 0;

    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;    //ADCINTs trigger at end of conversion

    // Setup ADCINT1 and ADCINT2 trigger source
    AdcRegs.INTSEL1N2.bit.INT1SEL = 6;      //EOC6 triggers ADCINT1
    AdcRegs.INTSEL1N2.bit.INT2SEL = 14;     //EOC14 triggers ADCINT2

    // Setup each SOC's ADCINT trigger source
    AdcRegs.ADCINTSOCSEL1.bit.SOC0  = 2;    //ADCINT2 starts SOC0-7
    AdcRegs.ADCINTSOCSEL1.bit.SOC1  = 2;
    AdcRegs.ADCINTSOCSEL1.bit.SOC2  = 2;
    AdcRegs.ADCINTSOCSEL1.bit.SOC3  = 2;
    AdcRegs.ADCINTSOCSEL1.bit.SOC4  = 2;
    AdcRegs.ADCINTSOCSEL1.bit.SOC5  = 2;
    AdcRegs.ADCINTSOCSEL1.bit.SOC6  = 2;
    AdcRegs.ADCINTSOCSEL1.bit.SOC7  = 2;
    AdcRegs.ADCINTSOCSEL2.bit.SOC8  = 1;    //ADCINT1 starts SOC8-15
    AdcRegs.ADCINTSOCSEL2.bit.SOC9  = 1;
    AdcRegs.ADCINTSOCSEL2.bit.SOC10 = 1;
    AdcRegs.ADCINTSOCSEL2.bit.SOC11 = 1;
    AdcRegs.ADCINTSOCSEL2.bit.SOC12 = 1;
    AdcRegs.ADCINTSOCSEL2.bit.SOC13 = 1;
    AdcRegs.ADCINTSOCSEL2.bit.SOC14 = 1;
    AdcRegs.ADCINTSOCSEL2.bit.SOC15 = 1;

    DELAY_US(ADC_usDELAY);                  // Delay before converting ADC channels


    //ADC Conversion

    AdcRegs.ADCSOCFRC1.all = 0x00FF;  // Force Start SOC0-7 to begin ping-pong sampling

    while( index < SampleSize ){

        //Wait for ADCINT1 to trigger, then add ADCRESULT0-7 registers to sum
        while (AdcRegs.ADCINTFLG.bit.ADCINT1 == 0){}
        AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;   //Must clear ADCINT1 flag since INT1CONT = 0
        Sum += AdcResult.ADCRESULT0;
        Sum += AdcResult.ADCRESULT1;
        Sum += AdcResult.ADCRESULT2;
        Sum += AdcResult.ADCRESULT3;
        Sum += AdcResult.ADCRESULT4;
        Sum += AdcResult.ADCRESULT5;
        Sum += AdcResult.ADCRESULT6;
        // Wait for SOC9 conversion to start, which gives time for SOC7 conversion result
        while( AdcRegs.ADCSOCFLG1.bit.SOC9 == 1 ){}
        Sum += AdcResult.ADCRESULT7;

        //Wait for ADCINT2 to trigger, then add ADCRESULT8-15 registers to sum
        while (AdcRegs.ADCINTFLG.bit.ADCINT2 == 0){}
        AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;   //Must clear ADCINT2 flag since INT2CONT = 0
        Sum += AdcResult.ADCRESULT8;
        Sum += AdcResult.ADCRESULT9;
        Sum += AdcResult.ADCRESULT10;
        Sum += AdcResult.ADCRESULT11;
        Sum += AdcResult.ADCRESULT12;
        Sum += AdcResult.ADCRESULT13;
        Sum += AdcResult.ADCRESULT14;
        // Wait for SOC1 conversion to start, which gives time for SOC15 conversion result
        while( AdcRegs.ADCSOCFLG1.bit.SOC1 == 1 ){}
        Sum += AdcResult.ADCRESULT15;

        index+=16;

    } // end data collection

    //Disable ADCINT1 and ADCINT2 to STOP the ping-pong sampling
    AdcRegs.INTSEL1N2.bit.INT1E = 0;
    AdcRegs.INTSEL1N2.bit.INT2E = 0;

    while(AdcRegs.ADCSOCFLG1.all != 0){} // Wait for any pending SOCs to complete

    // Clear any pending interrupts
    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;
    AdcRegs.ADCINTOVFCLR.bit.ADCINT1 = 1;
    AdcRegs.ADCINTOVFCLR.bit.ADCINT2 = 1;

    //reset RR pointer to 32, so that next SOC is SOC0
    AdcRegs.SOCPRICTL.bit.SOCPRIORITY = 1;
    while( AdcRegs.SOCPRICTL.bit.SOCPRIORITY != 1 );
    AdcRegs.SOCPRICTL.bit.SOCPRIORITY = 0;
    while( AdcRegs.SOCPRICTL.bit.SOCPRIORITY != 0 );

    Mean = Sum / SampleSize;    //Calculate average ADC sample value

    return Mean;                //return the average

}//end AdcConversion

//===========================================================================
// No more.
//===========================================================================
