//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
#include <inp.h>
#include <sim.h>
#include <log.h>
#include <fdtd.h>

#include "vec.h"

void fdtd_opencl_freemem(struct simulation *sim, struct fdtd_data *data)
{
	#ifdef use_open_cl
	printf_log(sim,"Freeingall\n");
	cl_int l_success;
	l_success=clReleaseMemObject(data->ggEx);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggEy);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggEz);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggHx);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggHy);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggHz);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggEx_last);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggEy_last);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggEz_last);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggHx_last);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggHy_last);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggHz_last);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggepsilon_r);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggy);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseMemObject(data->ggC);

	l_success=clReleaseProgram(data->prog);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseKernel(data->cal_E);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseKernel(data->cal_H);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseKernel(data->update_E);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	l_success=clReleaseKernel(data->update_H);
	if( l_success != CL_SUCCESS) printf_log(sim,"Can not free\n");

	clReleaseCommandQueue(data->cq);
	clReleaseContext(data->context);
	#endif
}

void fdtd_opencl_get_mem(struct simulation *sim, struct fdtd_data *data)
{
	#ifdef use_open_cl
	cl_int error;


	data->ggEx=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggEy=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggEz=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggHx=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggHy=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggHz=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggEx_last=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggEy_last=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggEz_last=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggHx_last=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggHy_last=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggHz_last=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggepsilon_r=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->zlen*data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggy=clCreateBuffer(data->context, CL_MEM_READ_WRITE, data->ylen*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}

	data->ggC=clCreateBuffer(data->context, CL_MEM_READ_WRITE, 17*sizeof(float), NULL, &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error Ex '%d'\n",error);
		exit(0);
	}
	#endif

}
