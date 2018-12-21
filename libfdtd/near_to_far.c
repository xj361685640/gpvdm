//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
//
//	https://www.gpvdm.com
//	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.

/** @file near_to_far.c
	@brief Fear to far filed generator, not currently linked to.
*/

void near_to_far(double *x, double *E,int len,double *y, double *E_right,int len_right)
{
int i;
double dx=x[1]-x[0];
double dy=y[1]-y[0];
double mid=x[len/2];
struct vec start;
struct vec stop;
double left=-0.1;
double right=0.1;
double dist=0.01;
double pos=left;
double *far=NULL;
far=malloc(sizeof(double)*len_far);
double dfar=(right-left)/((double)len_far);
int j;

if (far_avg==NULL)
{
far_avg=malloc(sizeof(double)*len_far);
near_top_avg=malloc(sizeof(double)*len);
near_right_avg=malloc(sizeof(double)*len_right);
xfar=malloc(sizeof(double)*len_far);
	for (j=0;j<len_far;j++)
	{
		far_avg[j]=0.0;
	}

	for (j=0;j<len;j++)
	{
		near_top_avg[j]=0.0;
	}

	for (j=0;j<len_right;j++)
	{
		near_right_avg[j]=0.0;
	}

}

pos=left;
double k=(2.0*pi/lambda);
double modr;
double complex cresult;
double result;
int n;

//FILE *out=fopen("./near_top.dat","w");
for (j=0;j<len;j++)
{
	near_top_avg[j]+=E[j];//pow(E[j],2.0);
	//	near_right_avg[j]=0.0;fprintf_log(sim,out,"%le %le\n",x[j], );
}
//fclose(out);

//out=fopen("./near_right.dat","w");
for (j=0;j<len_right;j++)
{
	near_right_avg[j]+=pow(E_right[j],2.0);
	//fprintf_log(sim,out,"%le %le\n",y[j], pow(E_right[j],2.0));
}
//fclose(out);

double theta;

struct vec dr;

pos=-80;
double dtheta=160/(double)len_far;
for (j=0;j<len_far;j++)
{
xfar[j]=pos;
pos+=dtheta;
}

for (j=0;j<len_far;j++)
{
result=0.0;
	//for (n=0;n<8;n++)
	{
		pos=dist*tan((2*pi/360.0)*xfar[j]);
		complex cresult=0+0*I;
		for (i=0;i<len;i++)
		{
			vec_set(&start,0.0,0.0,x[i]-mid);
			vec_set(&stop,0.0,pos,dist);
			//vec_set(&start,0.0,x[i]-mid-((double)n)*x[len-1],0.0);
			//vec_set(&stop,0.0,pos,dist);
			vec_cpy(&dr,&stop);
			vec_sub(&dr,&start);
			modr=vec_mod(&dr);
			cresult+=dx*E[i]*cexp(I*k*modr*-1.0)/modr;
		}

		for (i=len_right;i<len_right;i++)
		{
			vec_set(&start,0.0,y[i]-y[len_right-1],x[len-1]-mid);
			vec_set(&stop,0.0,pos,dist);
			//vec_set(&start,0.0,x[i]-mid-((double)n)*x[len-1],0.0);
			//vec_set(&stop,0.0,pos,dist);
			vec_cpy(&dr,&stop);
			vec_sub(&dr,&start);
			modr=vec_mod(&dr);
			cresult+=dy*E_right[i]*cexp(I*k*modr*-1.0)/modr;
		}

		result=cabs(cresult);

	}
theta=(360.0/(2.0*pi))*atan(pos/dist);
far[j]=fabs(result);

}

pos=left;


//out=fopen("./far.dat","w");
for (j=0;j<len_far;j++)
{
	far_avg[j]+=far[j];
//	fprintf_log(sim,out,"%lf %le\n",xfar[j], far_avg[j]/((double)far_steps));
}
//fclose(out);



far_steps+=1.0;
free(far);

}
