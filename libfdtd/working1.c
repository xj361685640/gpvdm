#include <math.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <CL/cl.h>

#include "vec.h"
#define zlen 100
#define ylen 100
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
float *gepsilon_r;

float c=3e8;
float epsilon0=8.85418782e-12;
float mu0=1.25663706e-6;
float pi=3.141592653;
float lambda=0.0;
float *far_avg=NULL;
float *near_right_avg=NULL;
float *near_top_avg=NULL;
#define len_far 300
float far_steps=0;
float *xfar=NULL;
float dx=0.0;//xsize/((float)zlen);
float dy=0.0;//ysize/((float)ylen);
float dz=0.0;//zsize/((float)zlen);

#define threads 8
 pthread_t thread_id[threads];
void *thread_function(void *);

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int  counter = 0;

void rot13 (char *buf)
{
    int index=0;
    char c=buf[index];
    while (c!=0) {
        if (c<'A' || c>'z' || (c>'Z' && c<'a')) {
            buf[index] = buf[index];
        } else {
            if (c>'m' || (c>'M' && c<'a')) {
	        buf[index] = buf[index]-13;
	    } else {
	        buf[index] = buf[index]+13;
	    }
        }
	c=buf[++index];
    }
}

void *thread_function_E(void *dummyPtr)
{

int thread;
int i;
int j;
float Cx=(dt2/(epsilon0*dx));
float Cy=(dt2/(epsilon0*dy));
float Cz=(dt2/(epsilon0*dz));
int start=1;
int stop=(ylen-1);
int delta=0.0;
for (thread=0;thread<threads;thread++)
{
	if (thread_id[thread]==pthread_self())
	{
		//printf("I am thread %ld\n",thread);
		delta=(stop-start)/threads;
		start=delta*thread;
		stop=delta*(thread+1);
		//printf("%ld %ld %ld %ld %ld\n",thread,start,stop,delta,ylen);
		if (thread==threads-1) stop=(ylen-1);
		for (j=start;j<stop;j++)
		{
			for (i=1;i<(zlen-1);i++)
			{
				//Ex[i][j]=Ex_last[i][j]+(Hz[i][j+1-1]-Hz[i][j-1])*epsilon_r[i][j]*Cy-(Hy[i+1-1][j]-Hy[i-1][j])*epsilon_r[i][j]*Cz;
				//Ey[i][j]=Ey_last[i][j]+(Hx[i+1-1][j]-Hx[i-1][j])*epsilon_r[i][j]*Cz;
				//Ez[i][j]=Ez_last[i][j]-(Hx[i][j+1-1]-Hx[i][j-1])*epsilon_r[i][j]*Cy;
				Ex[i][j]=Ex_last[i][j]+(Hz[i][j+1-1]-Hz[i][j-1])*epsilon_r[i][j]*Cy-(Hy[i+1-1][j]-Hy[i-1][j])*epsilon_r[i][j]*Cz;
				Ey[i][j]=Ey_last[i][j]+(Hx[i+1-1][j]-Hx[i-1][j])*epsilon_r[i][j]*Cz;
				Ez[i][j]=Ez_last[i][j]-(Hx[i][j+1-1]-Hx[i][j-1])*epsilon_r[i][j]*Cy;
			}
		}
		break;
	}
}
   pthread_mutex_lock( &mutex1 );
   counter++;
   pthread_mutex_unlock( &mutex1 );
}



void *thread_function_H(void *dummyPtr)
{

int thread;
int i;
int j;
float Cy=(dt2/(mu0*dy));
float Cx=(dt2/(mu0*dx));
float Cz=(dt2/(mu0*dy));
int start=1;
int stop=(ylen-1);
int delta=0.0;
for (thread=0;thread<threads;thread++)
{
	if (thread_id[thread]==pthread_self())
	{
		//printf("I am thread %ld\n",thread);
		delta=(stop-start)/threads;
		start=delta*thread;
		stop=delta*(thread+1);
		if (thread==threads-1) stop=(ylen-1);
		for (j=start;j<stop;j++)
		{
			for (i=0;i<(zlen-1);i++)
			{
				Hx[i][j]=Hx_last[i][j]-(Ez[i][j+1]-Ez[i][j])*Cy+(Ey[i+1][j]-Ey[i][j])*Cz;
				Hy[i][j]=Hy_last[i][j]-(Ex[i+1][j]-Ex[i][j])*Cz;
				Hz[i][j]=Hz_last[i][j]+(Ex[i][j+1]-Ex[i][j])*Cy;
			}
		}
		break;
	}
}
   pthread_mutex_lock( &mutex1 );
   counter++;
   pthread_mutex_unlock( &mutex1 );
}



void near_to_far(float *x, float *E,int len,float *y, float *E_right,int len_right)
{
int i;
float dx=x[1]-x[0];
float dy=y[1]-y[0];
float mid=x[len/2];
struct vec start;
struct vec stop;
float left=-0.1;
float right=0.1;
float dist=0.01;
float pos=left;
float *far=NULL;
far=malloc(sizeof(float)*len_far);
float dfar=(right-left)/((float)len_far);
int j;

if (far_avg==NULL)
{
far_avg=malloc(sizeof(float)*len_far);
near_top_avg=malloc(sizeof(float)*len);
near_right_avg=malloc(sizeof(float)*len_right);
xfar=malloc(sizeof(float)*len_far);
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
float k=(2.0*pi/lambda);
float modr;
float complex cresult;
float result;
int n;

//FILE *out=fopen("./near_top.dat","w");
for (j=0;j<len;j++)
{
	near_top_avg[j]+=pow(E[j],2.0);
	//	near_right_avg[j]=0.0;fprintf(out,"%le %le\n",x[j], );
}
//fclose(out);

//out=fopen("./near_right.dat","w");
for (j=0;j<len_right;j++)
{
	near_right_avg[j]+=pow(E_right[j],2.0);
	//fprintf(out,"%le %le\n",y[j], pow(E_right[j],2.0));
}
//fclose(out);

float theta;

struct vec dr;
for (j=0;j<len_far;j++)
{
result=0.0;
	//for (n=0;n<8;n++)
	{
		complex cresult=0+0*I;
		for (i=0;i<len;i++)
		{
			set_vec(&start,0.0,0.0,x[i]-mid);
			set_vec(&stop,0.0,pos,dist);
			//set_vec(&start,0.0,x[i]-mid-((float)n)*x[len-1],0.0);
			//set_vec(&stop,0.0,pos,dist);
			cpy_vec(&dr,&stop);
			sub_vec(&dr,&start);
			modr=mod_vec(&dr);
			cresult+=dx*E[i]*cexp(I*k*modr*-1.0)/modr;
		}

		for (i=len_right;i<len_right;i++)
		{
			set_vec(&start,0.0,y[i]-y[len_right-1],x[len-1]-mid);
			set_vec(&stop,0.0,pos,dist);
			//set_vec(&start,0.0,x[i]-mid-((float)n)*x[len-1],0.0);
			//set_vec(&stop,0.0,pos,dist);
			cpy_vec(&dr,&stop);
			sub_vec(&dr,&start);
			modr=mod_vec(&dr);
			cresult+=dy*E_right[i]*cexp(I*k*modr*-1.0)/modr;
		}

		result=cabs(cresult);

	}
theta=(360.0/(2.0*pi))*atan(pos/dist);
far[j]=fabs(result);
xfar[j]=theta;
pos+=dfar;

}

pos=left;


//out=fopen("./far.dat","w");
for (j=0;j<len_far;j++)
{
	far_avg[j]+=far[j];
//	fprintf(out,"%lf %le\n",xfar[j], far_avg[j]/((float)far_steps));
}
//fclose(out);



far_steps+=1.0;
free(far);

}


int main()
{
int i;
int pos=0;
	char buf[]="Hello, World!";
	size_t srcsize, worksize=strlen(buf);
	
	cl_int error;
	cl_platform_id platform;
	cl_device_id device;
	cl_uint platforms, devices;

	// Fetch the Platform and Device IDs; we only want one.
	error=clGetPlatformIDs(1, &platform, &platforms);
  size_t size = 0;
 cl_int l_success = clGetPlatformInfo(platform,
                                CL_PLATFORM_VENDOR, 0, NULL, &size);

  if( l_success != CL_SUCCESS)
  {
    printf("Failed getting vendor name size.\n");
    return -1;
  }
  printf("l_success = %d, size = %d\n", l_success, size);
  char* vendor = NULL;
  vendor = malloc(size);
  if( vendor )
  {
    l_success = clGetPlatformInfo(platform,CL_PLATFORM_VENDOR, size, vendor, &size);
    if( l_success != CL_SUCCESS )
    {
      printf("Failed getting vendor name.\n");
      return -1;
    }
    printf("Vendor name is '%s', length is %d\n", vendor, strlen(vendor));
  } else {
    printf("malloc failed.\n");
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
		printf("Device name = %s\n", dname);
		clGetDeviceInfo(device,CL_DRIVER_VERSION, 500, dname,NULL);
		printf("\tDriver version = %s\n", dname);
		clGetDeviceInfo(device,CL_DEVICE_GLOBAL_MEM_SIZE,sizeof(cl_ulong),&long_entries,NULL);
		printf("\tGlobal Memory (MB):\t%llu\n",long_entries/1024/1024);
		clGetDeviceInfo(device,CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,sizeof(cl_ulong),&long_entries,NULL);
		printf("\tGlobal Memory Cache (MB):\t%llu\n",long_entries/1024/1024);
		clGetDeviceInfo(device,CL_DEVICE_LOCAL_MEM_SIZE,sizeof(cl_ulong),&long_entries,NULL);
		printf("\tLocal Memory (KB):\t%llu\n",long_entries/1024);
		clGetDeviceInfo(device,CL_DEVICE_MAX_CLOCK_FREQUENCY,sizeof(cl_ulong),&long_entries,NULL);
		printf("\tMax clock (MHz) :\t%llu\n",long_entries);
		clGetDeviceInfo(device,CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(size_t),&p_size,NULL);
		printf("\tMax Work Group Size:\t%d\n",p_size);
		clGetDeviceInfo(device,CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cl_uint),&entries,NULL);
		printf("\tNumber of parallel compute cores:\t%d\n",entries);


	// Note that nVidia's OpenCL requires the platform property
	cl_context context=clCreateContext(properties, 1, &device, NULL, NULL, &error);

	cl_command_queue cq = clCreateCommandQueue(context, device, 0, &error);
        if ( error != CL_SUCCESS ) {
                printf( "cq error" );
                printf("\n Error number %d", error);
		exit(0);
        }

	rot13(buf);	// scramble using the CPU
	puts(buf);	// Just to demonstrate the plaintext is destroyed

        char src[8192];
        FILE *fil=fopen("code.cl","r");
        srcsize=fread(src, sizeof src, 1, fil);
        fclose(fil);

	const char *srcptr[]={src};
	// Submit the source code of the rot13 kernel to OpenCL
	cl_program prog=clCreateProgramWithSource(context,1, srcptr, &srcsize, &error);
	// and compile it (after this we could extract the compiled version)
	error=clBuildProgram(prog, 0, NULL, "", NULL, NULL);
        if ( error != CL_SUCCESS ) {
		char build_c[2048];
                printf( "Error on buildProgram " );
                printf("\n Error number %d", error);
                fprintf( stdout, "\nRequestingInfo\n" );
                clGetProgramBuildInfo( prog, device, CL_PROGRAM_BUILD_LOG, 4096, build_c, NULL );
                printf( "Build Log for %s_program:\n%s\n", "example", build_c );
		exit(0);
        }


	// Allocate memory for the kernel to work with
	//cl_mem mem1, mem2;
	//mem1=clCreateBuffer(context, CL_MEM_READ_ONLY, worksize, NULL, &error);
	//mem2=clCreateBuffer(context, CL_MEM_WRITE_ONLY, worksize, NULL, &error);
	
	// get a handle and map parameters for the kernel
	//cl_kernel k_rot13=clCreateKernel(prog, "rot13", &error);
	//clSetKernelArg(k_rot13, 0, sizeof(mem1), &mem1);
	//clSetKernelArg(k_rot13, 1, sizeof(mem2), &mem2);

	// Target buffer just so we show we got the data from OpenCL
	//char buf2[sizeof buf];
	//buf2[0]='?';
	//buf2[worksize]=0;

	// Send input data to OpenCL (async, don't alter the buffer!)
	//error=clEnqueueWriteBuffer(cq, mem1, CL_FALSE, 0, worksize, buf, 0, NULL, NULL);
	// Perform the operation
	//error=clEnqueueNDRangeKernel(cq, k_rot13, 1, NULL, &worksize, &worksize, 0, NULL, NULL);
	// Read the result back into buf2
	//error=clEnqueueReadBuffer(cq, mem2, CL_FALSE, 0, worksize, buf2, 0, NULL, NULL);
	/// Await completion of all the above
	//error=clFinish(cq);
	
	// Finally, output out happy message.
	//printf("one\n");
	//puts(buf2);

//exit(0);
dt=4e-21;
dt2=dt/2.0;
lambda=10e-10;
xsize=1e-5;
zsize=32e-10;
ysize=40e-10;
dx=xsize/((float)zlen);
dy=ysize/((float)ylen);
dz=zsize/((float)zlen);
int far_count=0;
//int i;	
int j;

float x[zlen];
float y[ylen];
float z[zlen];


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

gEx=(float *)malloc(sizeof(float)*zlen*ylen);
gEy=(float *)malloc(sizeof(float)*zlen*ylen);
gEz=(float *)malloc(sizeof(float)*zlen*ylen);
gHx=(float *)malloc(sizeof(float)*zlen*ylen);
gHy=(float *)malloc(sizeof(float)*zlen*ylen);
gHz=(float *)malloc(sizeof(float)*zlen*ylen);
gEx_last=(float *)malloc(sizeof(float)*zlen*ylen);
gEy_last=(float *)malloc(sizeof(float)*zlen*ylen);
gEz_last=(float *)malloc(sizeof(float)*zlen*ylen);
gepsilon_r=(float *)malloc(sizeof(float)*zlen*ylen);

cl_mem ggEx=clCreateBuffer(context, CL_MEM_WRITE_ONLY, zlen*ylen*sizeof(float), NULL, &error);
if (error!=CL_SUCCESS)
{
printf("error Ex\n");
exit(0);
}
cl_mem ggEy=clCreateBuffer(context, CL_MEM_WRITE_ONLY, zlen*ylen*sizeof(float), NULL, &error);

cl_mem ggEz=clCreateBuffer(context, CL_MEM_WRITE_ONLY, zlen*ylen*sizeof(float), NULL, &error);

cl_mem ggHx=clCreateBuffer(context, CL_MEM_READ_ONLY, zlen*ylen*sizeof(float), NULL, &error);
cl_mem ggHy=clCreateBuffer(context, CL_MEM_READ_ONLY, zlen*ylen*sizeof(float), NULL, &error);
cl_mem ggHz=clCreateBuffer(context, CL_MEM_READ_ONLY, zlen*ylen*sizeof(float), NULL, &error);

cl_mem ggEx_last=clCreateBuffer(context, CL_MEM_READ_ONLY, zlen*ylen*sizeof(float), NULL, &error);
cl_mem ggEy_last=clCreateBuffer(context, CL_MEM_READ_ONLY, zlen*ylen*sizeof(float), NULL, &error);
cl_mem ggEz_last=clCreateBuffer(context, CL_MEM_READ_ONLY, zlen*ylen*sizeof(float), NULL, &error);

cl_mem ggepsilon_r=clCreateBuffer(context, CL_MEM_READ_ONLY, zlen*ylen*sizeof(float), NULL, &error);
if (error!=CL_SUCCESS)
{
printf("error epsilon_r\n");
exit(0);
}
cl_mem ggC=clCreateBuffer(context, CL_MEM_READ_ONLY, 5*sizeof(float), NULL, &error);

	
// get a handle and map parameters for the kernel
cl_kernel k_rot13=clCreateKernel(prog, "rot13", &error);
printf("Made kernel\n");
if (error!=CL_SUCCESS)
{
printf("error2!!!!!!!!!!!!!!!!!!\n");

printf("Made kernel\n");
}

error=clSetKernelArg(k_rot13, 0, sizeof(cl_mem), &ggEx);
printf("Set args\n");

if (error!=CL_SUCCESS)
{
printf("error!!!!!!!!!!!!!!!!!!\n");
}

clSetKernelArg(k_rot13, 1, sizeof(cl_mem), &ggEy);
clSetKernelArg(k_rot13, 2, sizeof(cl_mem), &ggEz);

clSetKernelArg(k_rot13, 3, sizeof(cl_mem), &ggHx);
clSetKernelArg(k_rot13, 4, sizeof(cl_mem), &ggHy);
clSetKernelArg(k_rot13, 5, sizeof(cl_mem), &ggHz);

clSetKernelArg(k_rot13, 6, sizeof(cl_mem), &ggEx_last);
clSetKernelArg(k_rot13, 7, sizeof(cl_mem), &ggEy_last);
clSetKernelArg(k_rot13, 8, sizeof(cl_mem), &ggEz_last);

clSetKernelArg(k_rot13, 9, sizeof(cl_mem), &ggepsilon_r);

error=clSetKernelArg(k_rot13, 10, sizeof(cl_mem), &ggC);
if (error!=CL_SUCCESS)
{
printf("error!!!!!!!!!!!!!!!!!!\n");
}
	//puts(buf2);
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

FILE *gnuplot = popen("gnuplot -persist","w");
fprintf(gnuplot, "set terminal x11 title 'Solarsim' \n");
fflush(gnuplot);

FILE *gnuplot2 = popen("gnuplot -persist","w");
fprintf(gnuplot2, "set terminal x11 title 'Solarsim' \n");
fflush(gnuplot2);

int llen=zlen/16;
int lcount=0;
float height=16.6e-10+1e-10;
int on=0;
for (i=0;i<zlen;i++)
{
	for (j=0;j<ylen;j++)
	{
		if ((on==1)&&(y[j]<height))
		{
			epsilon_r[i][j]=3.0;
		}
	}
lcount++;

if (lcount>=llen)
{
	lcount=0;
	if (on==0)
	{
		on=1;
	}else
	{
		on=0;
	}
}

}

/*llen=ylen/16;
lcount=0;
on=0;
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		if (on==1) epsilon_r[i][j]=1.0;
	}
lcount++;

if (lcount>=llen)
{
	lcount=0;
	if (on==0)
	{
		on=1;
	}else
	{
		on=0;
	}
}

}*/

/*for (j=ylen*0.25;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		epsilon_r[i][j]=1.0;
	}

}*/

for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		if (y[j]<1e-10) epsilon_r[i][j]=14.0;
	}

}
/*for (j=ylen*0.6;j<ylen*0.7;j++)
{
	for (i=0;i<zlen;i++)
	{
		epsilon_r[i][j]=14.0;
	}

}

for (j=ylen*0.6;j<ylen*0.7;j++)
{
	for (i=zlen*0.45;i<zlen*0.55;i++)
	{
		epsilon_r[i][j]=1.0;
	}

}*/

/*for (j=0;j<ylen*0.2;j++)
{
	for (i=0;i<zlen;i++)
	{
		epsilon_r[i][j]=14.0;
	}

}*/


/*for (j=0;j<ylen*0.45;j++)
{
	for (i=zlen*0.3;i<zlen*0.35;i++)
	{
		epsilon_r[i][j]=70.0;
	}

}

for (j=ylen*0.55;j<ylen;j++)
{
	for (i=zlen*0.3;i<zlen*0.35;i++)
	{
		epsilon_r[i][j]=70.0;
	}

}*/

float time=0.0;
do
{
pos=0;


float c=1.0/sqrt(epsilon0*mu0);
float Hy_last_r=0.0;

float Cx=(dt2/(epsilon0*dx));
float Cy=(dt2/(epsilon0*dy));
float Cz=(dt2/(epsilon0*dz));
//printf("Efield start\n");
int err;
//Hz[1][1]=1.0;

/*for (i=0;i<zlen;i++)
{
for (j=0;j<zlen;j++)
{
Ex_last[i][j]=i;
Ey_last[i][j]=i+1;
Ez_last[i][j]=i+2;
}
}*/


for (i=0;i<zlen;i++)
{
//printf("%ld\n",i);
	memcpy ( &gEx[i*ylen], Ex[i], sizeof(float)*ylen );
	memcpy ( &gEy[i*ylen], Ey[i], sizeof(float)*ylen );
	memcpy ( &gEz[i*ylen], Ez[i], sizeof(float)*ylen );
	memcpy ( &gHx[i*ylen], Hx[i], sizeof(float)*ylen );
	memcpy ( &gHy[i*ylen], Hy[i], sizeof(float)*ylen );
	memcpy ( &gHz[i*ylen], Hz[i], sizeof(float)*ylen );

	memcpy ( &gEx_last[i*ylen], Ex_last[i], sizeof(float)*ylen );
	memcpy ( &gEy_last[i*ylen], Ey_last[i], sizeof(float)*ylen );
	memcpy ( &gEz_last[i*ylen], Ez_last[i], sizeof(float)*ylen );
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

error=clEnqueueWriteBuffer(cq, ggepsilon_r, CL_FALSE, 0, zlen*ylen*sizeof(float), gepsilon_r, 0, NULL, NULL);

float gC[5];
gC[0]=Cx;
gC[1]=Cy;
gC[2]=Cz;
gC[3]=(float)ylen;
gC[4]=(float)zlen;

error=clEnqueueWriteBuffer(cq, ggC, CL_FALSE, 0, 5*sizeof(float), gC, 0, NULL, NULL);

size_t global;
global = (size_t)zlen*ylen;
size_t local;
local = (size_t)1;

//printf("Running..\n");

error=clEnqueueNDRangeKernel(cq, k_rot13, 1, NULL, &global, &local, 0, NULL, NULL);
if (error!=CL_SUCCESS)
{
printf("Run error\n");
}

//printf("Run\n");
error=clEnqueueReadBuffer(cq, ggEx, CL_FALSE, 0, zlen*ylen*sizeof(float), gEx, 0, NULL, NULL);
if (error!=CL_SUCCESS)
{
printf("Read error\n");
}

error=clEnqueueReadBuffer(cq, ggEy, CL_FALSE, 0, zlen*ylen*sizeof(float), gEy, 0, NULL, NULL);
error=clEnqueueReadBuffer(cq, ggEz, CL_FALSE, 0, zlen*ylen*sizeof(float), gEz, 0, NULL, NULL);

error=clFinish(cq);
	// Await completion of all the above
	
	
	// Finally, output out happy message.
//printf("\nEx=%le %le %le\n",gEx[1],gEy[1],gEz[2]);
for (i=0;i<zlen;i++)
{
	memcpy ( Ex[i],&gEx[i*ylen], sizeof(float)*ylen );
	memcpy ( Ey[i],&gEy[i*ylen], sizeof(float)*ylen );
	memcpy ( Ez[i],&gEz[i*ylen], sizeof(float)*ylen );
}
/*FILE *one=fopen("./gEx.dat","w");
for (i=0;i<zlen;i++)
{
for (j=0;j<ylen;j++)
{
fprintf(one,"%ld %ld %lf %le %le\n",i,j,Ex[i][j],Ey[i][j],Ez[i][j]);
}
fprintf(one,"\n");
}
fclose(one);*/
/*printf("\nEx=%le %le %le\n",Ex[0][0],Ey[0][1],Ez[0][2]);
clReleaseMemObject(ggEx);
clReleaseMemObject(ggEy);
clReleaseMemObject(ggEz);
clReleaseMemObject(ggHx);
clReleaseMemObject(ggHy);
clReleaseMemObject(ggHz);
clReleaseMemObject(ggEx_last);
clReleaseMemObject(ggEy_last);
clReleaseMemObject(ggEz_last);
clReleaseMemObject(ggepsilon_r);
clReleaseMemObject(ggC);
exit(0);*/
/*   for(i=0; i < threads; i++)
   {
      pthread_create( &thread_id[i], NULL, thread_function_E, NULL );
   }

   for(j=0; j < threads; j++)
   {
      pthread_join( thread_id[j], NULL); 
   }

one=fopen("./tEx.dat","w");
for (i=0;i<zlen;i++)
{
for (j=0;j<ylen;j++)
{
fprintf(one,"%ld %ld %lf %le %le\n",i,j,Ex[i][j],Ey[i][j],Ez[i][j]);
}
fprintf(one,"\n");
}
fclose(one);
getchar();*/
//printf("Efield stop\n");

//for (i=2;i<zlen-2;i++)
//{
//i=20;//ylen-2;
i=3;
//float start=z[1];
//float stop=z[zlen-2];
	//for (i=1;i<zlen;i++)
//i=ylen/4;
float start=y[1];
float stop=y[ylen-2];
	for (j=0;j<ylen;j++)
	{

		if ((y[j]>1e-10)&&(y[j]<20e-10))
		{
			float a=0.0;
			float b=0.0;
			float c=0.0;
			float phi=0.0;
			float theta=0.0;


			float dot=0.0;
			phi=0.0;
			theta=90.0;

			float shift=-2.0;
			a=sin(theta*(2.0*pi/360.0))*cos(phi*(2.0*pi/360.0));
			b=sin(theta*(2.0*pi/360.0))*sin(phi*(2.0*pi/360.0));
			c=cos(theta*(2.0*pi/360.0));
			//printf(" a=%lf b=%lf c=%lf\n",a,b,c);
			//getchar();
			//dot=tan(2.0*pi*(shift)/360.0)*(z[i]-start)*2.0*pi/lambda;
			dot=tan(2.0*pi*(shift)/360.0)*(y[j]-start)*2.0*pi/lambda;
			float mod=1.0;
			//dot=0.0;
			Ex[i][j]+=mod*a*sin(dot-time*omega);
			Ey[i][j]+=mod*b*sin(dot-time*omega);
			Ez[i][j]+=mod*c*sin(dot-time*omega);
		}
		//printf("%lf %lf %lf %lf\n",sin(dot),a,b,c);
		//Hy[i][j]=cos(((float)step)/(2.0*3.14159*10.0));s
	}
//}
/*j=0;
Ex[0][j]=Ex_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ex[1][j]-Ex_last[0][j]);
Ey[0][j]=Ey_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ey[1][j]-Ey_last[0][j]);
Ez[0][j]=Ez_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ez[1][j]-Ez_last[0][j]);

j=ylen-1;
Ex[0][j]=Ex_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ex[1][j]-Ex_last[0][j]);
Ey[0][j]=Ey_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ey[1][j]-Ey_last[0][j]);
Ez[0][j]=Ez_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ez[1][j]-Ez_last[0][j]);
*/
for (j=0;j<(ylen-1);j++)
{
	Ex[0][j]=Ex_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ex[1][j]-Ex_last[0][j]);//-Ex_last_last[1][j]-((dz-c*dt2)/(dz+c*dt2))*(Ex[1][j]+Ex_last_last[0][j])+(2.0*dz/(dz+c*dt2))*(Ex_last[0][j]+Ex_last[1][j])+(((dz*c*dt2*c*dt2)/((2.0*dy*dy)*(dz+c*dt2))))*(Ex_last[0][j+1]-2.0*Ex_last[0][j]+Ex_last[0][j-1]+Ex_last[1][j+1]-2.0*Ex_last[1][j]+Ex_last[1][j-1]);
;//
	Ey[0][j]=Ey_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ey[1][j]-Ey_last[0][j]);//-Ey_last_last[1][j]-((dz-c*dt2)/(dz+c*dt2))*(Ey[1][j]+Ey_last_last[0][j])+(2.0*dz/(dz+c*dt2))*(Ey_last[0][j]+Ey_last[1][j])+(((dz*c*dt2*c*dt2)/((2.0*dy*dy)*(dz+c*dt2))))*(Ey_last[0][j+1]-2.0*Ey_last[0][j]+Ey_last[0][j-1]+Ey_last[1][j+1]-2.0*Ey_last[1][j]+Ey_last[1][j-1]);//
	Ez[0][j]=Ez_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ez[1][j]-Ez_last[0][j]);//-Ez_last_last[1][j]-((dz-c*dt2)/(dz+c*dt2))*(Ez[1][j]+Ez_last_last[0][j])+(2.0*dz/(dz+c*dt2))*(Ez_last[0][j]+Ez_last[1][j])+(((dz*c*dt2*c*dt2)/((2.0*dy*dy)*(dz+c*dt2))))*(Ez_last[0][j+1]-2.0*Ez_last[0][j]+Ez_last[0][j-1]+Ez_last[1][j+1]-2.0*Ez_last[1][j]+Ez_last[1][j-1]);//

	Ex[zlen-1][j]=Ex_last[zlen-2][j]+((c*dt2-dz)/(c*dt2+dz))*(Ex[zlen-2][j]-Ex_last[zlen-1][j]);
	Ey[zlen-1][j]=Ey_last[zlen-2][j]+((c*dt2-dz)/(c*dt2+dz))*(Ey[zlen-2][j]-Ey_last[zlen-1][j]);
	Ez[zlen-1][j]=Ez_last[zlen-2][j]+((c*dt2-dz)/(c*dt2+dz))*(Ez[zlen-2][j]-Ez_last[zlen-1][j]);
}

/*for (j=0;j<(ylen-1);j++)
{


	Ex[zlen-1][j]=Ex_last[zlen-2][j]+((c*dt2-dz)/(c*dt2+dz))*(Ex[zlen-2][j]-Ex_last[zlen-1][j]);
	Ey[zlen-1][j]=Ey_last[zlen-2][j]+((c*dt2-dz)/(c*dt2+dz))*(Ey[zlen-2][j]-Ey_last[zlen-1][j]);
	Ez[zlen-1][j]=Ez_last[zlen-2][j]+((c*dt2-dz)/(c*dt2+dz))*(Ez[zlen-2][j]-Ez_last[zlen-1][j]);
}*/

/*i=0;
Ex[i][0]=Ex_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][1]-Ex_last[i][0]);
Ey[i][0]=Ey_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ey[i][1]-Ey_last[i][0]);
Ez[i][0]=Ez_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ez[i][1]-Ez_last[i][0]);
Ex[i][ylen-1]=Ex_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][ylen-2]-Ex_last[i][ylen-1]);
Ey[i][ylen-1]=Ey_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ey[i][ylen-2]-Ey_last[i][ylen-1]);
Ez[i][ylen-1]=Ez_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ez[i][ylen-2]-Ez_last[i][ylen-1]);
i=zlen-1;
Ex[i][0]=Ex_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][1]-Ex_last[i][0]);
Ey[i][0]=Ey_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ey[i][1]-Ey_last[i][0]);
Ez[i][0]=Ez_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ez[i][1]-Ez_last[i][0]);
Ex[i][ylen-1]=Ex_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][ylen-2]-Ex_last[i][ylen-1]);
Ey[i][ylen-1]=Ey_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ey[i][ylen-2]-Ey_last[i][ylen-1]);
Ez[i][ylen-1]=Ez_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ez[i][ylen-2]-Ez_last[i][ylen-1]);
*/
for (i=0;i<(zlen-1);i++)
{
	Ex[i][0]=Ex_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][1]-Ex_last[i][0]);
	//Ex[i][0]=-Ex_last_last[i][1]-((dy-c*dt2)/(dy+c*dt2))*(Ex[i][1]+Ex_last_last[i][0])+(2.0*dy/(dy+c*dt2))*(Ex_last[i][0]+Ex_last[i][1])+(((dy*c*dt2*c*dt2)/((2.0*dz*dz)*(dy+c*dt2))))*(Ex_last[i+1][0]-2.0*Ex_last[i][0]+Ex_last[i-1][0]+Ex_last[i+1][1]-2.0*Ex_last[i][1]+Ex_last[i-1][1]);

	Ey[i][0]=Ey_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ey[i][1]-Ey_last[i][0]);
	//Ey[i][0]=-Ey_last_last[i][1]-((dy-c*dt2)/(dy+c*dt2))*(Ey[i][1]+Ey_last_last[i][0])+(2.0*dy/(dy+c*dt2))*(Ey_last[i][0]+Ey_last[i][1])+(((dy*c*dt2*c*dt2)/((2.0*dz*dz)*(dy+c*dt2))))*(Ey_last[i+1][0]-2.0*Ey_last[i][0]+Ey_last[i-1][0]+Ey_last[i+1][1]-2.0*Ey_last[i][1]+Ey_last[i-1][1]);
	Ez[i][0]=Ez_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ez[i][1]-Ez_last[i][0]);
	//Ez[i][0]=-Ez_last_last[i][1]-((dy-c*dt2)/(dy+c*dt2))*(Ez[i][1]+Ez_last_last[i][0])+(2.0*dy/(dy+c*dt2))*(Ez_last[i][0]+Ez_last[i][1])+(((dy*c*dt2*c*dt2)/((2.0*dz*dz)*(dy+c*dt2))))*(Ez_last[i+1][0]-2.0*Ez_last[i][0]+Ez_last[i-1][0]+Ez_last[i+1][1]-2.0*Ez_last[i][1]+Ez_last[i-1][1]);

}

for (i=0;i<(zlen-1);i++)
{
	//printf("one - %le\n",Ex_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][ylen-2]-Ex_last[i][zlen-1]));
	Ex[i][ylen-1]=Ex_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][ylen-2]-Ex_last[i][ylen-1]);//Ex_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][ylen-2]-Ex_last[i][zlen-1]);//-Ex_last_last[i][ylen-2]-((dy-c*dt2)/(dy+c*dt2))*(Ex[i][ylen-2]+Ex_last_last[i][ylen-1])+(2.0*dy/(dy+c*dt2))*(Ex_last[i][ylen-1]+Ex_last[i][ylen-2])+(((dy*c*dt2*c*dt2)/((2.0*dz*dz)*(dy+c*dt2))))*(Ex_last[i-1][ylen-1]-2.0*Ex_last[i][ylen-1]+Ex_last[i+1][ylen-1]+Ex_last[i-1][ylen-2]-2.0*Ex_last[i][ylen-2]+Ex_last[i+1][ylen-2]);
	//printf("two - %le\n",Ex_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][ylen-2]-Ex_last[i][zlen-1]));
////
	Ey[i][ylen-1]=Ey_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ey[i][ylen-2]-Ey_last[i][ylen-1]);//-Ey_last_last[i][ylen-2]-((dy-c*dt2)/(dy+c*dt2))*(Ey[i][ylen-2]+Ey_last_last[i][ylen-1])+(2.0*dy/(dy+c*dt2))*(Ey_last[i][ylen-1]+Ey_last[i][ylen-2])+(((dy*c*dt2*c*dt2)/((2.0*dz*dz)*(dy+c*dt2))))*(Ey_last[i+1][ylen-1]-2.0*Ey_last[i][ylen-1]+Ey_last[i-1][ylen-1]+Ey_last[i+1][ylen-2]-2.0*Ey_last[i][ylen-2]+Ey_last[i-1][ylen-2]);//

	Ez[i][ylen-1]=Ez_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ez[i][ylen-2]-Ez_last[i][ylen-1]);//Ez_last_last[i][ylen-2]-((dy-c*dt2)/(dy+c*dt2))*(Ez[i][ylen-2]+Ez_last_last[i][ylen-1])+(2.0*dy/(dy+c*dt2))*(Ez_last[i][ylen-1]+Ez_last[i][ylen-2])+(((dy*c*dt2*c*dt2)/((2.0*dz*dz)*(dy+c*dt2))))*(Ez_last[i+1][ylen-1]-2.0*Ez_last[i][ylen-1]+Ez_last[i-1][ylen-1]+Ez_last[i+1][ylen-2]-2.0*Ez_last[i][ylen-2]+Ez_last[i-1][ylen-2]);
}

for (i=0;i<zlen;i++)
{

		memcpy ( Ex_last_last[i], Ex_last[i], sizeof(float)*ylen );
		memcpy ( Ey_last_last[i], Ey_last[i], sizeof(float)*ylen );
		memcpy ( Ez_last_last[i], Ez_last[i], sizeof(float)*ylen );

}

for (i=0;i<zlen;i++)
{

		memcpy ( Ex_last[i], Ex[i], sizeof(float)*ylen );
		memcpy ( Ey_last[i], Ey[i], sizeof(float)*ylen );
		memcpy ( Ez_last[i], Ez[i], sizeof(float)*ylen );

}

//printf("H Field start\n");
   for(i=0; i < threads; i++)
   {
      pthread_create( &thread_id[i], NULL, thread_function_H, NULL );
   }

   for(j=0; j < threads; j++)
   {
      pthread_join( thread_id[j], NULL); 
   }
//printf("H Field stop\n");

//for (i=0;i<zlen;i++)
//{
//	printf("Ex=%le Ey=%le Ez=%le Hx=%le Hy=%le Hz=%le\n",Ex[i],Ey[i],Ez[i],Hx[i],Hy[i],Hz[i]);
//}
FILE *out;

if ((step%200)==0)
{
out=fopen("./Ex.dat","w");
int delta_y=1;
int delta_z=1;
if (ylen>70) delta_y=ylen/70;
if (zlen>70) delta_z=zlen/70;
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

out=fopen("./Ey.dat","w");
j=0;
do
{
	i=0;
	do
	{
		fprintf(out,"%le %le %le\n",z[i],y[j],Ey[i][j]);
		i+=delta_z;
	}while(i<zlen);

fprintf(out,"\n");
j+=delta_y;
}while(j<ylen);
fclose(out);

out=fopen("./Ez.dat","w");
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


printf("plot! %ld\n",step);


fprintf(gnuplot2, "load 'Ez.plot'\n");
fflush(gnuplot2);
}

/*int farstart=zlen*0.5;
int number=zlen-farstart;
float far[number];
float zfar[number];
for (j=0;j<number;j++)
{
far[j]=Ex[j+farstart][(int)(ylen*0.71)];
zfar[j]=z[j+farstart];
}*/


/*float far[ylen];
for (j=0;j<ylen;j++)
{
far[j]=Ex[(int)(zlen*0.90)][j];
}*/

float far_top[zlen];
for (i=0;i<zlen;i++)
{
far_top[i]=Ex[i][(int)(ylen-1)];
//if (z[i]<1.5e-9) far_top[i]=0.0;
}

float far_right[ylen];
for (j=0;j<ylen;j++)
{
far_right[j]=Ex[zlen-1][j];
if (y[j]<17e-10) far_right[j]=0.0;
}

/*int farstart=ylen*0.7;
int number=ylen-farstart;
float far[number];
float yfar[number];
for (j=0;j<number;j++)
{
far[j]=Ex[zlen-1][(int)(j+farstart)];
zfar[j]=y[j+farstart];
}*/

if (step>9000)
{
	if (far_count>200)
	{

		//near_to_far(yfar, far,number);
		near_to_far(z, far_top,zlen,y, far_right,ylen);

		//near_to_far(zfar, far,number);
		printf("%ld\n",step);
		if (far_count>300)
		{
			FILE *out=fopen("./far.dat","w");
			for (j=0;j<len_far;j++)
			{
				fprintf(out,"%lf %le\n",xfar[j], far_avg[j]/((float)far_steps));
			}
			fclose(out);

			out=fopen("./near_top.dat","w");
			for (j=0;j<zlen;j++)
			{
				fprintf(out,"%le %le\n",z[j], near_top_avg[j]/((float)far_steps));
			}
			fclose(out);

			out=fopen("./near_right.dat","w");
			for (j=0;j<ylen;j++)
			{

				fprintf(out,"%le %le\n",y[j], near_right_avg[j]/((float)far_steps));
			}
			fclose(out);

			fprintf(gnuplot, "load 'far.plot'\n");
			fflush(gnuplot);
		far_count=0;
		}
	}
	far_count++;
}
step++;
for (i=0;i<zlen;i++)
{

		memcpy ( Hx_last[i], Hx[i], sizeof(float)*ylen );
		memcpy ( Hy_last[i], Hy[i], sizeof(float)*ylen );
		memcpy ( Hz_last[i], Hz[i], sizeof(float)*ylen );
		//Hx_last[i][j]=Hx[i][j];
		//Hy_last[i][j]=Hy[i][j];
		//Hz_last[i][j]=Hz[i][j];

}

//usleep(10000);
//getchar();
time+=dt;
//printf("%ld\n",step);
}while(step<10000);//

fprintf(gnuplot, "exit\n");
fflush(gnuplot);
pclose(gnuplot);

fprintf(gnuplot2, "exit\n");
fflush(gnuplot2);
pclose(gnuplot2);
Ex[i];
Ey[i];
Ez[i];
Hx[i];
Hy[i];
Hz[i];
Ex_last[i];
Ey_last[i];
Ez_last[i];
Hx_last[i];
Hy_last[i];
Hz_last[i];
epsilon_r[i];
clReleaseMemObject(ggEx);
clReleaseMemObject(ggEy);
clReleaseMemObject(ggEz);
clReleaseMemObject(ggHx);
clReleaseMemObject(ggHy);
clReleaseMemObject(ggHz);
clReleaseMemObject(ggEx_last);
clReleaseMemObject(ggEy_last);
clReleaseMemObject(ggEz_last);
clReleaseMemObject(ggepsilon_r);
clReleaseMemObject(ggC);
return 0;
}

