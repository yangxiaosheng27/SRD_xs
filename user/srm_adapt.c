/*
 * 	FileName:	srm_control.c
 * 	Project:	SRM
 *
 *  Created on: 2018/6/28
 *  Author: 	yangxiaosheng
 */
#include "SRM_Project.h"        // User's Funtions

Parameter_Type a1=0,a2=0,b0=0,b1=0,d1=0,d2=0;// A*y=(z^-1)*Bu, A=1+a1(z^-1)+a2(z^-2), B=b0+b1(z^-1)
Parameter_Type am1=1.987,am2=0.878;
Parameter_Type f1,g0,g1,g2,u_adapt;

void System_Identification(void);
void Adapt_Control(void);

#pragma CODE_SECTION(System_Identification, "ramfuncs");       // for real-time operation , so must be run in the RAM
void System_Identification(void)
{
#define Identif_Dim 6
	static Parameter_Type y=0,y_1=0,y_2=0,u=0,u_1=0,u_2=0,e=0,e_1=0,e_2=0;
	static Parameter_Type first_run=1,p=0.99;
	static Mat fai_now,G_now,theta_now,P_now,theta_last,P_last;
	static Mat temp1,temp2,temp3,temp4;
	static Parameter_Type fai_element[Identif_Dim];
	static Parameter_Type theta_0[]={
			0.1,
			0.1,
			0.1,
			0.1,
			0.1,
			0.1
	};
	if(first_run)
	{
		first_run = 0;
		MatCreate(&fai_now, Identif_Dim, 1);
		MatCreate(&theta_now, Identif_Dim, 1);
		MatCreate(&P_now, Identif_Dim, Identif_Dim);
		MatCreate(&G_now, Identif_Dim, 1);
		MatCreate(&theta_last, Identif_Dim, 1);
		MatSetVal(&theta_last, theta_0);
		MatCreate(&temp1, Identif_Dim, Identif_Dim);
		MatEye(&temp1);
		MatCreate(&P_last, Identif_Dim, Identif_Dim);
		MatAmplify(&temp1, 1000.0, &P_last);
		MatDelete(&temp1);
	}

	// update u,y
	u = TORQUE.Expect;
	y = SRM.Speed;

	// update fai_now
	fai_element[0] = -y_1;
	fai_element[1] = -y_2;
	fai_element[2] = u_1;
	fai_element[3] = u_2;
	fai_element[4] = e_1;
	fai_element[5] = e_2;
	MatSetVal(&fai_now,fai_element);

	// update G_now
	MatCreate(&temp1, Identif_Dim, 1);
	MatMul(&P_last, &fai_now, &temp1);
	MatCreate(&temp2, 1, Identif_Dim);
	MatTrans(&fai_now, &temp2);	// temp2=fai_now' will be used later
	MatCreate(&temp3, 1, Identif_Dim);
	MatMul(&temp2, &P_last, &temp3);
	MatCreate(&temp4, 1, 1);
	MatMul(&temp3, &fai_now, &temp4);
	MatDiv(&temp1, **temp4.element+p, &G_now);
	MatDelete(&temp1);
	MatDelete(&temp3);
	MatDelete(&temp4);

	// update P_now
	MatCreate(&temp1, 1, Identif_Dim);
	MatMul(&temp2, &P_last, &temp1);	// temp2=fai_now' will be used later
	MatCreate(&temp3, Identif_Dim, Identif_Dim);
	MatMul(&G_now, &temp1, &temp3);
	MatCreate(&temp4, Identif_Dim, Identif_Dim);
	MatSub(&P_last, &temp3, &temp4);
	MatDiv(&temp4, p, &P_now);
	MatDelete(&temp1);
	MatDelete(&temp3);
	MatDelete(&temp4);

	// update theta_now
	MatCreate(&temp1, 1, 1);
	MatMul(&temp2, &theta_last, &temp1);	// temp2=fai_now' will be used later
	MatCreate(&temp3, Identif_Dim, 1);
	MatAmplify(&G_now, y-**(temp1.element), &temp3);
	MatAdd(&theta_last, &temp3, &theta_now);
	MatDelete(&temp1);
	MatDelete(&temp3);

	// update e
	MatCreate(&temp1, 1, 1);
	MatMul(&temp2,&theta_now,&temp1);
	e = y - **(temp1.element);
	MatDelete(&temp1);
	MatDelete(&temp2);

	// save for next time
	MatCopy(&P_now, &P_last);
	MatCopy(&theta_now, &theta_last);
	y_2 = y_1;
	y_1 = y;
	u_2 = u_1;
	u_1 = u;
	e_2 = e_1;
	e_1 = e;

	// A*y=(z^-1)*Bu, A=1+a1(z^-1)+a2(z^-2), B=b0+b1(z^-1)
	a1 = **(theta_now.element+0);
	a2 = **(theta_now.element+1);
	b0 = **(theta_now.element+2);
	b1 = **(theta_now.element+3);
	d1 = **(theta_now.element+4);
	d2 = **(theta_now.element+5);

}

#pragma CODE_SECTION(Adapt_Control, "ramfuncs");       // for real-time operation , so must be run in the RAM
void Adapt_Control(void)
{
	static Parameter_Type num_f1,num_g0,num_g1,num_g2,dem_f1,dem_g0,dem_g1,dem_g2;
	static Parameter_Type u_adapt_1=0, u_adapt_2=0,r,y,y_1=0,y_2=0;

//#define eps 1.4E-45
#define eps 0.000001

	r = SPEED.Expect;
	y = SRM.Speed;

	num_f1 = (am2*b1*b1*b0-a2*b0*b1*b1+a1*b1*b1*b0-a2*b0*b0*b1-b1*b1*b1*am1+a1*b1*b1-b1*b1);
	dem_f1 = (a1*b0*b0*b1+a1*b1*b1*b0-b1*b1*b0-a2*b0*b0*b0-b1*b1);
	if(dem_f1==0)dem_f1 = eps;
	f1 = num_f1/dem_f1;

	num_g0 = (am1-f1-a1+1);
	dem_g0 = b0;
	if(dem_g0==0)dem_g0 = eps;
	g0 = num_g0/dem_g0;

	num_g1 = (a2*b1+a1*b1*f1-a2*b0*f1);
	dem_g1 = b1*b1;
	if(dem_g1==0)dem_g1 = eps;
	g1 = num_g1/dem_g1;

	num_g2 = (a2*f1);
	dem_g2 = b1;
	if(dem_g2==0)dem_g2 = eps;
	g2 = num_g2/dem_g2;

	// caculate u_adpadt
	u_adapt = (g0+g1+g2)*r - g0*y -g1*y_1 - g2*y_2 + (1-f1)*u_adapt_1 + f1*u_adapt_2 ;
	u_adapt = u_adapt*0.35;

	if(u_adapt>2000)u_adapt=2000;
	else if(u_adapt<0)u_adapt=0;

	u_adapt_2 = u_adapt_1;
	u_adapt_1 = u_adapt;
	y_2 = y_1;
	y_1 = y;
}
