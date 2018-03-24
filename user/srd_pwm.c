/*
 * 	FileName:	srd_pwm.c
 * 	Project:	SRD_xs
 *
 *  Created on: 2018/3/25
 *      Author: yangxiaosheng
 */


#include "SRD_Project.h"        			// User's Funtions

void my_init_pwm(void)
{
	InitEPwm1Gpio();
	InitEPwm2Gpio();
	InitEPwm3Gpio();
}
