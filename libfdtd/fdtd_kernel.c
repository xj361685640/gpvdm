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
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <inp.h>
#include <sim.h>
#include <log.h>
#include <fdtd.h>

#include "vec.h"


void fdtd_kernel_init(struct simulation *sim, struct fdtd_data *data)
{
	cl_int error;
	data->cal_E=clCreateKernel(data->prog, "cal_E", &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"Can not make E kernel\n");
		exit(0);
	}

	data->update_E=clCreateKernel(data->prog, "update_E", &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"Can not make E kernel\n");
		exit(0);
	}

	data->cal_H=clCreateKernel(data->prog, "cal_H", &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"Can not make H kernel\n");
		exit(0);
	}

	data->update_H=clCreateKernel(data->prog, "update_H", &error);
	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"Can not make E kernel\n");
		exit(0);
	}

	clSetKernelArg(data->cal_E, 0, sizeof(cl_mem), &(data->ggEx));
	clSetKernelArg(data->cal_E, 1, sizeof(cl_mem), &(data->ggEy));
	clSetKernelArg(data->cal_E, 2, sizeof(cl_mem), &(data->ggEz));

	clSetKernelArg(data->cal_E, 3, sizeof(cl_mem), &(data->ggHx));
	clSetKernelArg(data->cal_E, 4, sizeof(cl_mem), &(data->ggHy));
	clSetKernelArg(data->cal_E, 5, sizeof(cl_mem), &(data->ggHz));

	clSetKernelArg(data->cal_E, 6, sizeof(cl_mem), &(data->ggEx_last));
	clSetKernelArg(data->cal_E, 7, sizeof(cl_mem), &(data->ggEy_last));
	clSetKernelArg(data->cal_E, 8, sizeof(cl_mem), &(data->ggEz_last));

	clSetKernelArg(data->cal_E, 9, sizeof(cl_mem), &(data->ggHx_last));
	clSetKernelArg(data->cal_E, 10, sizeof(cl_mem), &(data->ggHy_last));
	clSetKernelArg(data->cal_E, 11, sizeof(cl_mem), &(data->ggHz_last));

	clSetKernelArg(data->cal_E, 12, sizeof(cl_mem), &(data->ggepsilon_r));
	clSetKernelArg(data->cal_E, 13, sizeof(cl_mem), &(data->ggy));
	error=clSetKernelArg(data->cal_E, 14, sizeof(cl_mem), &(data->ggC));

	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error!!!!!!!!!!!!!!!!!!\n");
	}

	clSetKernelArg(data->update_E, 0, sizeof(cl_mem), &(data->ggEx));
	clSetKernelArg(data->update_E, 1, sizeof(cl_mem), &(data->ggEy));
	clSetKernelArg(data->update_E, 2, sizeof(cl_mem), &(data->ggEz));

	clSetKernelArg(data->update_E, 3, sizeof(cl_mem), &(data->ggHx));
	clSetKernelArg(data->update_E, 4, sizeof(cl_mem), &(data->ggHy));
	clSetKernelArg(data->update_E, 5, sizeof(cl_mem), &(data->ggHz));

	clSetKernelArg(data->update_E, 6, sizeof(cl_mem), &(data->ggEx_last));
	clSetKernelArg(data->update_E, 7, sizeof(cl_mem), &(data->ggEy_last));
	clSetKernelArg(data->update_E, 8, sizeof(cl_mem), &(data->ggEz_last));

	clSetKernelArg(data->update_E, 9, sizeof(cl_mem), &(data->ggHx_last));
	clSetKernelArg(data->update_E, 10, sizeof(cl_mem), &(data->ggHy_last));
	clSetKernelArg(data->update_E, 11, sizeof(cl_mem), &(data->ggHz_last));

	clSetKernelArg(data->update_E, 12, sizeof(cl_mem), &(data->ggepsilon_r));
	clSetKernelArg(data->update_E, 13, sizeof(cl_mem), &(data->ggy));
	error=clSetKernelArg(data->update_E, 14, sizeof(cl_mem), &(data->ggC));

	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error!!!!!!!!!!!!!!!!!!\n");
	}

	clSetKernelArg(data->cal_H, 0, sizeof(cl_mem), &(data->ggEx));
	clSetKernelArg(data->cal_H, 1, sizeof(cl_mem), &(data->ggEy));
	clSetKernelArg(data->cal_H, 2, sizeof(cl_mem), &(data->ggEz));

	clSetKernelArg(data->cal_H, 3, sizeof(cl_mem), &(data->ggHx));
	clSetKernelArg(data->cal_H, 4, sizeof(cl_mem), &(data->ggHy));
	clSetKernelArg(data->cal_H, 5, sizeof(cl_mem), &(data->ggHz));

	clSetKernelArg(data->cal_H, 6, sizeof(cl_mem), &(data->ggEx_last));
	clSetKernelArg(data->cal_H, 7, sizeof(cl_mem), &(data->ggEy_last));
	clSetKernelArg(data->cal_H, 8, sizeof(cl_mem), &(data->ggEz_last));

	clSetKernelArg(data->cal_H, 9, sizeof(cl_mem), &(data->ggHx_last));
	clSetKernelArg(data->cal_H, 10, sizeof(cl_mem), &(data->ggHy_last));
	clSetKernelArg(data->cal_H, 11, sizeof(cl_mem), &(data->ggHz_last));

	clSetKernelArg(data->cal_H, 12, sizeof(cl_mem), &(data->ggepsilon_r));
	clSetKernelArg(data->cal_H, 13, sizeof(cl_mem), &(data->ggy));
	error=clSetKernelArg(data->cal_H, 14, sizeof(cl_mem), &(data->ggC));

	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error!!!!!!!!!!!!!!!!!!\n");
	}

	clSetKernelArg(data->update_H, 0, sizeof(cl_mem), &(data->ggEx));
	clSetKernelArg(data->update_H, 1, sizeof(cl_mem), &(data->ggEy));
	clSetKernelArg(data->update_H, 2, sizeof(cl_mem), &(data->ggEz));

	clSetKernelArg(data->update_H, 3, sizeof(cl_mem), &(data->ggHx));
	clSetKernelArg(data->update_H, 4, sizeof(cl_mem), &(data->ggHy));
	clSetKernelArg(data->update_H, 5, sizeof(cl_mem), &(data->ggHz));

	clSetKernelArg(data->update_H, 6, sizeof(cl_mem), &(data->ggEx_last));
	clSetKernelArg(data->update_H, 7, sizeof(cl_mem), &(data->ggEy_last));
	clSetKernelArg(data->update_H, 8, sizeof(cl_mem), &(data->ggEz_last));

	clSetKernelArg(data->update_H, 9, sizeof(cl_mem), &(data->ggHx_last));
	clSetKernelArg(data->update_H, 10, sizeof(cl_mem), &(data->ggHy_last));
	clSetKernelArg(data->update_H, 11, sizeof(cl_mem), &(data->ggHz_last));

	clSetKernelArg(data->update_H, 12, sizeof(cl_mem), &(data->ggepsilon_r));
	clSetKernelArg(data->update_H, 13, sizeof(cl_mem), &(data->ggy));
	error=clSetKernelArg(data->update_H, 14, sizeof(cl_mem), &(data->ggC));

	if (error!=CL_SUCCESS)
	{
		printf_log(sim,"error!!!!!!!!!!!!!!!!!!\n");
	}


}
