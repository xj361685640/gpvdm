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

int zlen=800;
int ylen=800;
int dump_number=0;
float **Ex;
float **Ey;
float **Ez;
float **Hx;
float **Hy;
float **Hz;
float **Ex_last;
float **Ey_last;
float **Ez_last;
float **Ex_last_last;
float **Ey_last_last;
float **Ez_last_last;
float **Hx_last;
float **Hy_last;
float **Hz_last;
float **epsilon_r;
float **z_ang;
float dt=0.0;
float dt2=0.0;
float xsize=0;
float zsize=0;
float ysize=0;

float *gEx;
float *gEy;
float *gEz;
float *gHx;
float *gHy;
float *gHz;
float *gEx_last;
float *gEy_last;
float *gEz_last;
float *gHx_last;
float *gHy_last;
float *gHz_last;
float *gepsilon_r;
float *gy;

int plot=0;

FILE *gnuplot=NULL;
FILE *gnuplot2=NULL;

float c=3e8;
float epsilon0f=8.85418782e-12;
float mu0=1.25663706e-6;
float pi=3.141592653;
float lambda=0.0;
float src_start=0.0;
float src_stop=0.0;
double *far_avg=NULL;
double *near_right_avg=NULL;
double *near_top_avg=NULL;
#define len_far 500
double far_steps=0;
float *xfar=NULL;
float dx=0.0;//xsize/((float)zlen);
float dy=0.0;//ysize/((float)ylen);
float dz=0.0;//zsize/((float)zlen);

cl_mem ggEx;
cl_mem ggEy;
cl_mem ggEz;

cl_mem ggHx;
cl_mem ggHy;
cl_mem ggHz;

cl_mem ggEx_last;
cl_mem ggEy_last;
cl_mem ggEz_last;

cl_mem ggHx_last;
cl_mem ggHy_last;
cl_mem ggHz_last;

cl_mem ggepsilon_r;

cl_mem ggy;
cl_mem ggC;

cl_kernel cal_E;
cl_kernel update_E;
cl_kernel cal_H;
cl_kernel update_H;
cl_program prog;
cl_command_queue cq;
cl_context context;
cl_device_id device;

void lam(float *y,float *z,float **epsilon_r,float ybtm,float ytop,double on_width,double off_width)
{
int i;
int j;
double dx=z[1]-z[0];
int lcount=0;
int on=0;
double pos=0.0;
if (off_width==0.0) return;
for (i=0;i<zlen;i++)
{
	for (j=0;j<ylen;j++)
	{
		if ((on==1)&&(y[j]<ytop)&&(y[j]>ybtm))
		{
			epsilon_r[i][j]=14.0;
		}
	}

	if (on==0)
	{
		if (pos>off_width)
		{
		on=1;
		pos=0.0;
		}
	}

	if (on==1)
	{
		if (pos>on_width)
		{
		on=0;
		pos=0.0;
		}
	}

pos+=dx;
}
}

void dump_all(struct simulation *sim,float* y, float* z)
{
int i;
int j;
printf_log(sim,"Dumping\n");
FILE *out=fopen("./Ex_final.dat","w");

for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		printf_log(out,"%le %le %le\n",z[i],y[j],Ex[i][j]);
	}

printf_log(out,"\n");
}
fclose(out);
printf_log(sim,"Dumping\n");
out=fopen("./Ey_final.dat","w");

for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		printf_log(out,"%le %le %le\n",z[i],y[j],Ey[i][j]);
	}

printf_log(out,"\n");
}
fclose(out);
printf_log(sim,"Dumping\n");
out=fopen("./Ez_final.dat","w");
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		printf_log(out,"%le %le %le\n",z[i],y[j],Ex[i][j]);
	}

printf_log(out,"\n");
}
fclose(out);
printf_log(sim,"Dumping\n");
out=fopen("./epsilon_r_final.dat","w");

for (j=0;j<ylen;j++)
{

	for (i=0;i<zlen;i++)
	{
		printf_log(out,"%le %le %le\n",z[i],y[j],epsilon_r[i][j]);
	}

printf_log(out,"\n");
}
fclose(out);
}

void free_all(struct simulation *sim)
{
int i;


for (i=0;i<zlen;i++)
{
	free(Ex[i]);
	free(Ey[i]);
	free(Ez[i]);
	free(Hx[i]);
	free(Hy[i]);
	free(Hz[i]);
	free(Ex_last[i]);
	free(Ey_last[i]);
	free(Ez_last[i]);
	free(Hx_last[i]);
	free(Hy_last[i]);
	free(Hz_last[i]);
	free(epsilon_r[i]);
}

free(Ex);
free(Ey);
free(Ez);
free(Hx);
free(Hy);
free(Hz);
free(Ex_last);
free(Ey_last);
free(Ez_last);
free(Hx_last);
free(Hy_last);
free(Hz_last);
free(epsilon_r);


free(gEx);
free(gEy);
free(gEz);
free(gHx);
free(gHy);
free(gHz);
free(gEx_last);
free(gEy_last);
free(gEz_last);
free(gHx_last);
free(gHy_last);
free(gHz_last);
free(gepsilon_r);
free(gy);

if (gnuplot!=NULL)
{
fprintf(gnuplot, "exit\n");
fflush(gnuplot);
pclose(gnuplot);
}

if (gnuplot2!=NULL)
{
fprintf(gnuplot2, "exit\n");
fflush(gnuplot2);
pclose(gnuplot2);
}

printf_log(sim,"Freeingall\n");
cl_int l_success;
l_success=clReleaseMemObject(ggEx);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggEy);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggEz);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggHx);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggHy);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggHz);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggEx_last);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggEy_last);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggEz_last);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggHx_last);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggHy_last);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggHz_last);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggepsilon_r);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggy);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseMemObject(ggC);

l_success=clReleaseProgram(prog);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseKernel(cal_E);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseKernel(cal_H);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseKernel(update_E);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");
l_success=clReleaseKernel(update_H);
if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

clReleaseCommandQueue(cq);
clReleaseContext(context);
}

/*void my_handler(int s)
{
free_all();
           printf_log(sim,"Caught signal %d\n",s);
           exit(1); 

}*/



int do_fdtd(struct simulation *sim)
{
struct fdtd_data data;

int max_ittr=0;
float height=16e-10;
float start=1e-10;
float stop=16e-10;
float sithick=0.0;
char temp[100];
int lam_jmax;
float gap;

struct inp_file inp;

inp_init(&sim,&inp);
if (inp_load_from_path(&sim,&inp,sim->input_path,"fdtd.inp")!=0)
{
	printf_log(sim,"can't find file the fdtd config file",sim->input_path);
	exit(0);
}
inp_check(sim,&inp,1.0);
inp_search_float(sim,&inp,&gap,"#gap");
inp_search_int(sim,&inp,&gap,"#max_ittr");
inp_search_int(sim,&inp,&gap,"#ylen");
printf_log(sim,"ylen=%d\n",ylen);
inp_search_int(sim,&inp,&gap,"#zlen");
printf_log(sim,"zlen=%d\n",zlen);
inp_search_float(sim,&inp,&ysize,"#ysize");
printf_log(sim,"ysize=%e\n",ysize);
inp_search_float(sim,&inp,&ysize,"#zsize");
printf_log(sim,"zsize=%e\n",zsize);
inp_search_int(sim,&inp,&lam_jmax,"#lam_jmax");
printf_log(sim,"lam_jmax=%d\n",lam_jmax);
inp_search_float(sim,&inp,&sithick,"#sithick");
printf_log(sim,"sithick=%e\n",sithick);
inp_search_int(sim,&inp,&plot,"#plot");
inp_free(sim,&inp);


int i;
int pos=0;
size_t srcsize;
cl_int error;
cl_platform_id platform;
cl_uint platforms, devices;

// Fetch the Platform and Device IDs; we only want one.
error=clGetPlatformIDs(1, &platform, &platforms);
size_t size = 0;
cl_int l_success = clGetPlatformInfo(platform,CL_PLATFORM_VENDOR, 0, NULL, &size);

if( l_success != CL_SUCCESS)
{
	printf_log(sim,"Failed getting vendor name size.\n");
	return -1;
}
  printf_log(sim,"l_success = %d, size = %d\n", l_success, size);
  char* vendor = NULL;
  vendor = malloc(size);
  if( vendor )
  {
    l_success = clGetPlatformInfo(platform,CL_PLATFORM_VENDOR, size, vendor, &size);
    if( l_success != CL_SUCCESS )
    {
      printf_log(sim,"Failed getting vendor name.\n");
      return -1;
    }
    printf_log(sim,"Vendor name is '%s', length is %d\n", vendor, strlen(vendor));
  } else {
    printf_log(sim,"malloc failed.\n");
    return -1;
  }

/* query devices for information */


	error=clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &devices);
	cl_context_properties properties[]={CL_CONTEXT_PLATFORM, (cl_context_properties)platform,0};
char dname[500];
cl_ulong long_entries;
size_t p_size;
cl_uint entries;
		clGetDeviceInfo(device, CL_DEVICE_NAME, 500, dname,NULL);
		printf_log(sim,"Device name = %s\n", dname);
		clGetDeviceInfo(device,CL_DRIVER_VERSION, 500, dname,NULL);
		printf_log(sim,"\tDriver version = %s\n", dname);
		clGetDeviceInfo(device,CL_DEVICE_GLOBAL_MEM_SIZE,sizeof(cl_ulong),&long_entries,NULL);
		printf_log(sim,"\tGlobal Memory (MB):\t%llu\n",long_entries/1024/1024);
		clGetDeviceInfo(device,CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,sizeof(cl_ulong),&long_entries,NULL);
		printf_log(sim,"\tGlobal Memory Cache (MB):\t%llu\n",long_entries/1024/1024);
		clGetDeviceInfo(device,CL_DEVICE_LOCAL_MEM_SIZE,sizeof(cl_ulong),&long_entries,NULL);
		printf_log(sim,"\tLocal Memory (KB):\t%llu\n",long_entries/1024);
		clGetDeviceInfo(device,CL_DEVICE_MAX_CLOCK_FREQUENCY,sizeof(cl_ulong),&long_entries,NULL);
		printf_log(sim,"\tMax clock (MHz) :\t%llu\n",long_entries);
		clGetDeviceInfo(device,CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_t),&p_size,NULL);
		printf_log(sim,"\tMax Work Group Size:\t%d\n",p_size);
		clGetDeviceInfo(device,CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cl_uint),&entries,NULL);
		printf_log(sim,"\tNumber of parallel compute cores:\t%d\n",entries);


	// Note that nVidia's OpenCL requires the platform property
	context=clCreateContext(properties, 1, &device, NULL, NULL, &error);

	cq = clCreateCommandQueue(context, device, 0, &error);
        if ( error != CL_SUCCESS ) {
                printf_log(sim, "clCreateCommandQueue error" );
                printf_log(sim,"\n Error number %d", error);
		exit(0);
        }

	srcsize=fdtd_load_code(sim,&data);

	const char *srcptr[]={data.src_code};
	// Submit the source code of the rot13 kernel to OpenCL
	prog=clCreateProgramWithSource(context,1, srcptr, &srcsize, &error);
	// and compile it (after this we could extract the compiled version)
	error=clBuildProgram(prog, 0, NULL, "", NULL, NULL);
        if ( error != CL_SUCCESS )
		{
			char build_c[20000];
			printf_log(sim, "Error on buildProgram " );
			printf_log(sim,"\n Error number %d", error);
			fprintf( stdout, "\nRequestingInfo\n" );
			clGetProgramBuildInfo( prog, device, CL_PROGRAM_BUILD_LOG, 20000, build_c, NULL );
			printf_log(sim, "Build Log for %s_program:\n%s\n", "example", build_c );
			exit(0);
        }


dt=1e-15;
dt2=dt/2.0;
lambda=520e-9;
src_start=10e-9;
src_stop=20e-9;

xsize=1e-5;
dx=xsize/((float)zlen);
dy=ysize/((float)ylen);
dz=zsize/((float)zlen);
int far_count=0;
//int i;	
int j;

float *x=malloc(sizeof(float)*zlen);//[zlen];
float *y=malloc(sizeof(float)*ylen);//[ylen];
float *z=malloc(sizeof(float)*zlen);//[zlen];


float min=1.0/(c*sqrt(pow(1.0/dy,2.0)+pow(1.0/dz,2.0)));
//dt=min/10.0;
printf ("dy=%lf nm, dz=%lf nm min_dt=%le dt=%le\n min_dx=%lf",dy*1e9,dz*1e9,min,dt,lambda*1e9/10.0);

//getchar();


float f=c/lambda;
float omega=2.0*3.14159*f;

Ex=(float **)malloc(sizeof(float*)*zlen);
Ey=(float **)malloc(sizeof(float*)*zlen);
Ez=(float **)malloc(sizeof(float*)*zlen);
Hx=(float **)malloc(sizeof(float*)*zlen);
Hy=(float **)malloc(sizeof(float*)*zlen);
Hz=(float **)malloc(sizeof(float*)*zlen);
Ex_last=(float **)malloc(sizeof(float*)*zlen);
Ey_last=(float **)malloc(sizeof(float*)*zlen);
Ez_last=(float **)malloc(sizeof(float*)*zlen);
Ex_last_last=(float **)malloc(sizeof(float*)*zlen);
Ey_last_last=(float **)malloc(sizeof(float*)*zlen);
Ez_last_last=(float **)malloc(sizeof(float*)*zlen);
Hx_last=(float **)malloc(sizeof(float*)*zlen);
Hy_last=(float **)malloc(sizeof(float*)*zlen);
Hz_last=(float **)malloc(sizeof(float*)*zlen);
epsilon_r=(float **)malloc(sizeof(float*)*zlen);
z_ang=(float **)malloc(sizeof(float*)*zlen);

for (i=0;i<zlen;i++)
{
	Ex[i]=(float *)malloc(sizeof(float)*ylen);
	Ey[i]=(float *)malloc(sizeof(float)*ylen);
	Ez[i]=(float *)malloc(sizeof(float)*ylen);
	Hx[i]=(float *)malloc(sizeof(float)*ylen);
	Hy[i]=(float *)malloc(sizeof(float)*ylen);
	Hz[i]=(float *)malloc(sizeof(float)*ylen);
	Ex_last[i]=(float *)malloc(sizeof(float)*ylen);
	Ey_last[i]=(float *)malloc(sizeof(float)*ylen);
	Ez_last[i]=(float *)malloc(sizeof(float)*ylen);
	Ex_last_last[i]=(float *)malloc(sizeof(float)*ylen);
	Ey_last_last[i]=(float *)malloc(sizeof(float)*ylen);
	Ez_last_last[i]=(float *)malloc(sizeof(float)*ylen);
	Hx_last[i]=(float *)malloc(sizeof(float)*ylen);
	Hy_last[i]=(float *)malloc(sizeof(float)*ylen);
	Hz_last[i]=(float *)malloc(sizeof(float)*ylen);
	epsilon_r[i]=(float *)malloc(sizeof(float)*ylen);
	z_ang[i]=(float *)malloc(sizeof(float)*ylen);
}


gEx=(float *)malloc(sizeof(float)*zlen*ylen);
gEy=(float *)malloc(sizeof(float)*zlen*ylen);
gEz=(float *)malloc(sizeof(float)*zlen*ylen);
gHx=(float *)malloc(sizeof(float)*zlen*ylen);
gHy=(float *)malloc(sizeof(float)*zlen*ylen);
gHz=(float *)malloc(sizeof(float)*zlen*ylen);
gEx_last=(float *)malloc(sizeof(float)*zlen*ylen);
gEy_last=(float *)malloc(sizeof(float)*zlen*ylen);
gEz_last=(float *)malloc(sizeof(float)*zlen*ylen);
gHx_last=(float *)malloc(sizeof(float)*zlen*ylen);
gHy_last=(float *)malloc(sizeof(float)*zlen*ylen);
gHz_last=(float *)malloc(sizeof(float)*zlen*ylen);
gepsilon_r=(float *)malloc(sizeof(float)*zlen*ylen);
gy=(float *)malloc(sizeof(float)*ylen);


ggEx=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);
if (error!=CL_SUCCESS)
{
	printf_log(sim,"error Ex\n");
	exit(0);
}

ggEy=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);

ggEz=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);

ggHx=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);
ggHy=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);
ggHz=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);

ggEx_last=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);
ggEy_last=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);
ggEz_last=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);

ggHx_last=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);
ggHy_last=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);
ggHz_last=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);

ggepsilon_r=clCreateBuffer(context, CL_MEM_READ_WRITE, zlen*ylen*sizeof(float), NULL, &error);

ggy=clCreateBuffer(context, CL_MEM_READ_WRITE, ylen*sizeof(float), NULL, &error);
ggC=clCreateBuffer(context, CL_MEM_READ_WRITE, 17*sizeof(float), NULL, &error);

struct sigaction sigIntHandler;

//sigIntHandler.sa_handler = my_handler;
//sigemptyset(&sigIntHandler.sa_mask);
//sigIntHandler.sa_flags = 0;

sigaction(SIGINT, &sigIntHandler, NULL);
	
// get a handle and map parameters for the kernel
cal_E=clCreateKernel(prog, "cal_E", &error);
if (error!=CL_SUCCESS)
{
	printf_log(sim,"Can not make E kernel\n");
	exit(0);
}

update_E=clCreateKernel(prog, "update_E", &error);
if (error!=CL_SUCCESS)
{
	printf_log(sim,"Can not make E kernel\n");
	exit(0);
}

cal_H=clCreateKernel(prog, "cal_H", &error);
if (error!=CL_SUCCESS)
{
	printf_log(sim,"Can not make H kernel\n");
	exit(0);
}

update_H=clCreateKernel(prog, "update_H", &error);
if (error!=CL_SUCCESS)
{
	printf_log(sim,"Can not make E kernel\n");
	exit(0);
}

clSetKernelArg(cal_E, 0, sizeof(cl_mem), &ggEx);
clSetKernelArg(cal_E, 1, sizeof(cl_mem), &ggEy);
clSetKernelArg(cal_E, 2, sizeof(cl_mem), &ggEz);

clSetKernelArg(cal_E, 3, sizeof(cl_mem), &ggHx);
clSetKernelArg(cal_E, 4, sizeof(cl_mem), &ggHy);
clSetKernelArg(cal_E, 5, sizeof(cl_mem), &ggHz);

clSetKernelArg(cal_E, 6, sizeof(cl_mem), &ggEx_last);
clSetKernelArg(cal_E, 7, sizeof(cl_mem), &ggEy_last);
clSetKernelArg(cal_E, 8, sizeof(cl_mem), &ggEz_last);

clSetKernelArg(cal_E, 9, sizeof(cl_mem), &ggHx_last);
clSetKernelArg(cal_E, 10, sizeof(cl_mem), &ggHy_last);
clSetKernelArg(cal_E, 11, sizeof(cl_mem), &ggHz_last);

clSetKernelArg(cal_E, 12, sizeof(cl_mem), &ggepsilon_r);
clSetKernelArg(cal_E, 13, sizeof(cl_mem), &ggy);
error=clSetKernelArg(cal_E, 14, sizeof(cl_mem), &ggC);

if (error!=CL_SUCCESS)
{
	printf_log(sim,"error!!!!!!!!!!!!!!!!!!\n");
}

clSetKernelArg(update_E, 0, sizeof(cl_mem), &ggEx);
clSetKernelArg(update_E, 1, sizeof(cl_mem), &ggEy);
clSetKernelArg(update_E, 2, sizeof(cl_mem), &ggEz);

clSetKernelArg(update_E, 3, sizeof(cl_mem), &ggHx);
clSetKernelArg(update_E, 4, sizeof(cl_mem), &ggHy);
clSetKernelArg(update_E, 5, sizeof(cl_mem), &ggHz);

clSetKernelArg(update_E, 6, sizeof(cl_mem), &ggEx_last);
clSetKernelArg(update_E, 7, sizeof(cl_mem), &ggEy_last);
clSetKernelArg(update_E, 8, sizeof(cl_mem), &ggEz_last);

clSetKernelArg(update_E, 9, sizeof(cl_mem), &ggHx_last);
clSetKernelArg(update_E, 10, sizeof(cl_mem), &ggHy_last);
clSetKernelArg(update_E, 11, sizeof(cl_mem), &ggHz_last);

clSetKernelArg(update_E, 12, sizeof(cl_mem), &ggepsilon_r);
clSetKernelArg(update_E, 13, sizeof(cl_mem), &ggy);
error=clSetKernelArg(update_E, 14, sizeof(cl_mem), &ggC);

if (error!=CL_SUCCESS)
{
	printf_log(sim,"error!!!!!!!!!!!!!!!!!!\n");
}

clSetKernelArg(cal_H, 0, sizeof(cl_mem), &ggEx);
clSetKernelArg(cal_H, 1, sizeof(cl_mem), &ggEy);
clSetKernelArg(cal_H, 2, sizeof(cl_mem), &ggEz);

clSetKernelArg(cal_H, 3, sizeof(cl_mem), &ggHx);
clSetKernelArg(cal_H, 4, sizeof(cl_mem), &ggHy);
clSetKernelArg(cal_H, 5, sizeof(cl_mem), &ggHz);

clSetKernelArg(cal_H, 6, sizeof(cl_mem), &ggEx_last);
clSetKernelArg(cal_H, 7, sizeof(cl_mem), &ggEy_last);
clSetKernelArg(cal_H, 8, sizeof(cl_mem), &ggEz_last);

clSetKernelArg(cal_H, 9, sizeof(cl_mem), &ggHx_last);
clSetKernelArg(cal_H, 10, sizeof(cl_mem), &ggHy_last);
clSetKernelArg(cal_H, 11, sizeof(cl_mem), &ggHz_last);

clSetKernelArg(cal_H, 12, sizeof(cl_mem), &ggepsilon_r);
clSetKernelArg(cal_H, 13, sizeof(cl_mem), &ggy);
error=clSetKernelArg(cal_H, 14, sizeof(cl_mem), &ggC);

if (error!=CL_SUCCESS)
{
	printf_log(sim,"error!!!!!!!!!!!!!!!!!!\n");
}

clSetKernelArg(update_H, 0, sizeof(cl_mem), &ggEx);
clSetKernelArg(update_H, 1, sizeof(cl_mem), &ggEy);
clSetKernelArg(update_H, 2, sizeof(cl_mem), &ggEz);

clSetKernelArg(update_H, 3, sizeof(cl_mem), &ggHx);
clSetKernelArg(update_H, 4, sizeof(cl_mem), &ggHy);
clSetKernelArg(update_H, 5, sizeof(cl_mem), &ggHz);

clSetKernelArg(update_H, 6, sizeof(cl_mem), &ggEx_last);
clSetKernelArg(update_H, 7, sizeof(cl_mem), &ggEy_last);
clSetKernelArg(update_H, 8, sizeof(cl_mem), &ggEz_last);

clSetKernelArg(update_H, 9, sizeof(cl_mem), &ggHx_last);
clSetKernelArg(update_H, 10, sizeof(cl_mem), &ggHy_last);
clSetKernelArg(update_H, 11, sizeof(cl_mem), &ggHz_last);

clSetKernelArg(update_H, 12, sizeof(cl_mem), &ggepsilon_r);
clSetKernelArg(update_H, 13, sizeof(cl_mem), &ggy);
error=clSetKernelArg(update_H, 14, sizeof(cl_mem), &ggC);

if (error!=CL_SUCCESS)
{
	printf_log(sim,"error!!!!!!!!!!!!!!!!!!\n");
}




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

for (i=0;i<lam_jmax;i++)
{
	lam(y,z,epsilon_r,start,stop,2e-10,gap);
	start+=18e-10;
	stop+=18e-10;
}
start-=22e-10;
stop-=22e-10;





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

return 0;
}

