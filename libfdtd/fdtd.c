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

#include <math.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <inp.h>
#include <sim.h>
#include <log.h>
#include <fdtd.h>

#include "vec.h"

int dump_number=0;


float c=3e8;
float epsilon0f=8.85418782e-12;
float mu0=1.25663706e-6;
float pi=3.141592653;
double *far_avg=NULL;
double *near_right_avg=NULL;
double *near_top_avg=NULL;
#define len_far 500
double far_steps=0;
float *xfar=NULL;
float dx=0.0;//xsize/((float)zlen);
float dy=0.0;//ysize/((float)ylen);
float dz=0.0;//zsize/((float)zlen);



/*void my_handler(int s)
{
free_all();
           printf_log(sim,"Caught signal %d\n",s);
           exit(1); 

}*/


int do_fdtd(struct simulation *sim)
{
int i;
int pos=0;
size_t srcsize;
cl_int error;

struct fdtd_data data;

float height=16e-10;
float start=1e-10;
float stop=16e-10;
char temp[100];

	fdtd_load_config(sim,&data);

	opencl_init(sim,&data);


	data.cq = clCreateCommandQueue(data.context, data.device, 0, &error);
	if ( error != CL_SUCCESS )
	{
		printf_log(sim, "clCreateCommandQueue error\n" );
		printf_log(sim,"\n Error number %d", error);
		exit(0);
     }

	srcsize=fdtd_load_code(sim,&data);

	const char *srcptr[]={data.src_code};
	// Submit the source code of the rot13 kernel to OpenCL
	data.prog=clCreateProgramWithSource(data.context,1, srcptr, &srcsize, &error);
	// and compile it (after this we could extract the compiled version)
	error=clBuildProgram(data.prog, 0, NULL, "", NULL, NULL);

    if ( error != CL_SUCCESS )
	{
		char build_c[20000];
		printf_log(sim, "Error on buildProgram " );
		printf_log(sim,"\n Error number %d", error);
		fprintf( stdout, "\nRequestingInfo\n" );
		clGetProgramBuildInfo( data.prog, data.device, CL_PROGRAM_BUILD_LOG, 20000, build_c, NULL );
		printf_log(sim, "Build Log for %s_program:\n%s\n", "example", build_c );
		exit(0);
    }


data.dt=1e-15;
data.dt2=data.dt/2.0;
data.lambda=520e-9;
data.src_start=10e-9;
data.src_stop=20e-9;

data.xsize=1e-5;
dx=data.xsize/((float)data.zlen);
dy=data.ysize/((float)data.ylen);
dz=data.zsize/((float)data.zlen);
int far_count=0;
//int i;	
int j;



float min=1.0/(c*sqrt(pow(1.0/dy,2.0)+pow(1.0/dz,2.0)));
//dt=min/10.0;
printf ("dy=%lf nm, dz=%lf nm min_dt=%le dt=%le\n min_dx=%lf",dy*1e9,dz*1e9,min,data.dt,data.lambda*1e9/10.0);

//getchar();


float f=c/data.lambda;
float omega=2.0*3.14159*f;

//struct sigaction sigIntHandler;

//sigIntHandler.sa_handler = my_handler;
//sigemptyset(&sigIntHandler.sa_mask);
//sigIntHandler.sa_flags = 0;

//sigaction(SIGINT, &sigIntHandler, NULL);
	
// get a handle and map parameters for the kernel
fdtd_kernel_init(sim, &data);
/*

float xpos=0.0;
float ypos=0.0;
float zpos=0.0;

float bEx=1.0;
float bEy=1.0;
float bEz=1.0;

float bHx=1.0;
float bHy=1.0;
float bHz=1.0;

float Exc=1.0;
float Eyc=1.0;
float Ezc=1.0;

float Hxc=1.0;
float Hyc=1.0;
float Hzc=1.0;

zpos=dz/2.0;
ypos=dy/2.0;

for (j=0;j<ylen;j++)
{
zpos=0.0;
	for (i=0;i<zlen;i++)
	{
		z[i]=zpos;

		zpos+=dz;

		Ex[i][j]=0.0;
		Ey[i][j]=0.0;
		Ez[i][j]=0.0;
		Hx[i][j]=0.0;
		Hy[i][j]=0.0;
		Hz[i][j]=0.0;
		Ex_last[i][j]=0.0;
		Ey_last[i][j]=0.0;
		Ez_last[i][j]=0.0;
		Ex_last_last[i][j]=0.0;
		Ey_last_last[i][j]=0.0;
		Ez_last_last[i][j]=0.0;
		Hx_last[i][j]=0.0;
		Hy_last[i][j]=0.0;
		Hz_last[i][j]=0.0;
		epsilon_r[i][j]=1.0;
		z_ang[i][j]=1.0;


	}
y[j]=ypos;
ypos+=dy;
}

pos=0;
int step=0;
float Exl=0.0;
float Hyl=0.0;
gnuplot = popen("gnuplot","w");
fprintf(gnuplot, "set terminal x11 title 'Solarsim' \n");
fflush(gnuplot);

if (plot==1)
{
gnuplot2 = popen("gnuplot","w");
fprintf(gnuplot2, "set terminal x11 title 'Solarsim' \n");
fflush(gnuplot2);
}




for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		if (y[j]<sithick) epsilon_r[i][j]=14.0;
	}

}

memcpy(gy, y, sizeof(float)*ylen );
error=clEnqueueWriteBuffer(cq, ggy, CL_FALSE, 0, ylen*sizeof(float), gy, 0, NULL, NULL);
if (error!=CL_SUCCESS)
{
	printf_log(sim,"error!!!!!!!!!!!!!!!!!!\n");
}

for (i=0;i<zlen;i++)
{
	memcpy ( &gEx[i*ylen], Ex[i], sizeof(float)*ylen );
	memcpy ( &gEy[i*ylen], Ey[i], sizeof(float)*ylen );
	memcpy ( &gEz[i*ylen], Ez[i], sizeof(float)*ylen );
	memcpy ( &gHx[i*ylen], Hx[i], sizeof(float)*ylen );
	memcpy ( &gHy[i*ylen], Hy[i], sizeof(float)*ylen );
	memcpy ( &gHz[i*ylen], Hz[i], sizeof(float)*ylen );

	memcpy ( &gEx_last[i*ylen], Ex_last[i], sizeof(float)*ylen );
	memcpy ( &gEy_last[i*ylen], Ey_last[i], sizeof(float)*ylen );
	memcpy ( &gEz_last[i*ylen], Ez_last[i], sizeof(float)*ylen );

	memcpy ( &gHx_last[i*ylen], Hx_last[i], sizeof(float)*ylen );
	memcpy ( &gHy_last[i*ylen], Hy_last[i], sizeof(float)*ylen );
	memcpy ( &gHz_last[i*ylen], Hz_last[i], sizeof(float)*ylen );

	memcpy ( &gepsilon_r[i*ylen], epsilon_r[i], sizeof(float)*ylen );
}

error=clEnqueueWriteBuffer(cq, ggEx, CL_FALSE, 0, zlen*ylen*sizeof(float), gEx, 0, NULL, NULL);
error=clEnqueueWriteBuffer(cq, ggEy, CL_FALSE, 0, zlen*ylen*sizeof(float), gEy, 0, NULL, NULL);
error=clEnqueueWriteBuffer(cq, ggEz, CL_FALSE, 0, zlen*ylen*sizeof(float), gEz, 0, NULL, NULL);

error=clEnqueueWriteBuffer(cq, ggHx, CL_FALSE, 0, zlen*ylen*sizeof(float), gHx, 0, NULL, NULL);
error=clEnqueueWriteBuffer(cq, ggHy, CL_FALSE, 0, zlen*ylen*sizeof(float), gHy, 0, NULL, NULL);
error=clEnqueueWriteBuffer(cq, ggHz, CL_FALSE, 0, zlen*ylen*sizeof(float), gHz, 0, NULL, NULL);

error=clEnqueueWriteBuffer(cq, ggEx_last, CL_FALSE, 0, zlen*ylen*sizeof(float), gEx_last, 0, NULL, NULL);
error=clEnqueueWriteBuffer(cq, ggEy_last, CL_FALSE, 0, zlen*ylen*sizeof(float), gEy_last, 0, NULL, NULL);
error=clEnqueueWriteBuffer(cq, ggEz_last, CL_FALSE, 0, zlen*ylen*sizeof(float), gEz_last, 0, NULL, NULL);

error=clEnqueueWriteBuffer(cq, ggHx_last, CL_FALSE, 0, zlen*ylen*sizeof(float), gHx_last, 0, NULL, NULL);
error=clEnqueueWriteBuffer(cq, ggHy_last, CL_FALSE, 0, zlen*ylen*sizeof(float), gHy_last, 0, NULL, NULL);
error=clEnqueueWriteBuffer(cq, ggHz_last, CL_FALSE, 0, zlen*ylen*sizeof(float), gHz_last, 0, NULL, NULL);

error=clEnqueueWriteBuffer(cq, ggepsilon_r, CL_FALSE, 0, zlen*ylen*sizeof(float), gepsilon_r, 0, NULL, NULL);

float Cx=(dt2/(epsilon0f*dx));
float Cy=(dt2/(epsilon0f*dy));
float Cz=(dt2/(epsilon0f*dz));
float Cmy=(dt2/(mu0*dy));
float Cmx=(dt2/(mu0*dx));
float Cmz=(dt2/(mu0*dy));
float time=0.0;
src_stop=(float)stop;
src_start=(float)sithick;
float gC[17];
gC[0]=Cx;
gC[1]=Cy;
gC[2]=Cz;
gC[3]=(float)ylen;
gC[4]=(float)zlen;
gC[5]=(float)time;
gC[6]=(float)omega;
gC[7]=(float)lambda;
gC[8]=(float)dx;
gC[9]=(float)dy;
gC[10]=(float)dz;
gC[11]=(float)dt2;
gC[12]=(float)Cmx;
gC[13]=(float)Cmy;
gC[14]=(float)Cmz;
gC[15]=(float)src_start;
gC[16]=(float)src_stop;

error=clEnqueueWriteBuffer(cq, ggC, CL_FALSE, 0, 17*sizeof(float), gC, 0, NULL, NULL);

far_steps=0;
do
{
	pos=0;

	float Hy_last_r=0.0;

	int err;

	size_t global;
	global = (size_t)zlen*ylen;
	size_t local;
	local = (size_t)250;
	FILE *out;

	error=clEnqueueNDRangeKernel(cq, cal_E, 1, NULL, &global, &local, 0, NULL, NULL);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"Run error in E kernel\n");
	}
	error=clFinish(cq);



	error=clEnqueueNDRangeKernel(cq, update_E, 1, NULL, &global, &local, 0, NULL, NULL);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"Run error in E kernel\n");
	}
	error=clFinish(cq);



	error=clEnqueueNDRangeKernel(cq, cal_H, 1, NULL, &global, &local, 0, NULL, NULL);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"Run error in H kernel\n");
	}
	error=clFinish(cq);


	error=clEnqueueNDRangeKernel(cq, update_H, 1, NULL, &global, &local, 0, NULL, NULL);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"Run error in E kernel\n");
	}
	error=clFinish(cq);


	if ((step%20)==0)
	{

		error=clEnqueueReadBuffer(cq, ggHx, CL_FALSE, 0, zlen*ylen*sizeof(float), gHx, 0, NULL, NULL);
		error=clEnqueueReadBuffer(cq, ggHy, CL_FALSE, 0, zlen*ylen*sizeof(float), gHy, 0, NULL, NULL);
		error=clEnqueueReadBuffer(cq, ggHz, CL_FALSE, 0, zlen*ylen*sizeof(float), gHz, 0, NULL, NULL);

		error=clEnqueueReadBuffer(cq, ggEx, CL_FALSE, 0, zlen*ylen*sizeof(float), gEx, 0, NULL, NULL);
		error=clEnqueueReadBuffer(cq, ggEy, CL_FALSE, 0, zlen*ylen*sizeof(float), gEy, 0, NULL, NULL);
		error=clEnqueueReadBuffer(cq, ggEz, CL_FALSE, 0, zlen*ylen*sizeof(float), gEz, 0, NULL, NULL);

		for (i=0;i<zlen;i++)
		{
			memcpy ( Ex[i],&gEx[i*ylen], sizeof(float)*ylen );
			memcpy ( Ey[i],&gEy[i*ylen], sizeof(float)*ylen );
			memcpy ( Ez[i],&gEz[i*ylen], sizeof(float)*ylen );
		}

		for (i=0;i<zlen;i++)
		{
			memcpy ( Hx[i],&gHx[i*ylen], sizeof(float)*ylen );
			memcpy ( Hy[i],&gHy[i*ylen], sizeof(float)*ylen );
			memcpy ( Hz[i],&gHz[i*ylen], sizeof(float)*ylen );
		}

		error=clFinish(cq);

		char name[100];
		sprintf(name,"./Ex.dat");
		out=fopen(name,"w");
		int delta_y=1;
		int delta_z=1;
		if (ylen>100) delta_y=ylen/400;
		if (zlen>100) delta_z=zlen/400;
		j=0;
		do
		{
			i=0;
			do
			{
				fprintf(out,"%le %le %le\n",z[i],y[j],Ex[i][j]);
				i+=delta_z;
			}while(i<zlen);

			fprintf(out,"\n");
			j+=delta_y;
		}while(j<ylen);
		fclose(out);

		i=0;
		j=0;
		out=fopen("./epsilon_r.dat","w");
		do
		{
			i=0;
			do
			{
				fprintf(out,"%le %le %le\n",z[i],y[j],epsilon_r[i][j]);
				i+=delta_z;
			}while(i<zlen);

		fprintf(out,"\n");
		j+=delta_y;
		}while(j<ylen);
		fclose(out);

		
		printf_log(sim,"plot! %ld\n",step);


		if (plot==1)
		{
			fprintf(gnuplot2, "load 'Ex.plot'\n");
			fflush(gnuplot2);
		}

	}

	time+=dt;
	//printf_log(sim,"%ld\n",step);
}while(step<max_ittr);//


free_all(sim);
*/
return 0;
}

