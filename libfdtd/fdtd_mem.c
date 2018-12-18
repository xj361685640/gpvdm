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



void fdtd_get_mem(struct simulation *sim, struct fdtd_data *data)
{
	int i;

	data->Ex=(float **)malloc(sizeof(float*)*data->zlen);
	data->Ey=(float **)malloc(sizeof(float*)*data->zlen);
	data->Ez=(float **)malloc(sizeof(float*)*data->zlen);
	data->Hx=(float **)malloc(sizeof(float*)*data->zlen);
	data->Hy=(float **)malloc(sizeof(float*)*data->zlen);
	data->Hz=(float **)malloc(sizeof(float*)*data->zlen);
	data->Ex_last=(float **)malloc(sizeof(float*)*data->zlen);
	data->Ey_last=(float **)malloc(sizeof(float*)*data->zlen);
	data->Ez_last=(float **)malloc(sizeof(float*)*data->zlen);
	data->Ex_last_last=(float **)malloc(sizeof(float*)*data->zlen);
	data->Ey_last_last=(float **)malloc(sizeof(float*)*data->zlen);
	data->Ez_last_last=(float **)malloc(sizeof(float*)*data->zlen);
	data->Hx_last=(float **)malloc(sizeof(float*)*data->zlen);
	data->Hy_last=(float **)malloc(sizeof(float*)*data->zlen);
	data->Hz_last=(float **)malloc(sizeof(float*)*data->zlen);
	data->epsilon_r=(float **)malloc(sizeof(float*)*data->zlen);
	data->z_ang=(float **)malloc(sizeof(float*)*data->zlen);

	for (i=0;i<data->zlen;i++)
	{
		data->Ex[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Ey[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Ez[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Hx[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Hy[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Hz[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Ex_last[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Ey_last[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Ez_last[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Ex_last_last[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Ey_last_last[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Ez_last_last[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Hx_last[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Hy_last[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->Hz_last[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->epsilon_r[i]=(float *)malloc(sizeof(float)*data->ylen);
		data->z_ang[i]=(float *)malloc(sizeof(float)*data->ylen);
	}

	data->gEx=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gEy=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gEz=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gHx=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gHy=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gHz=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gEx_last=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gEy_last=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gEz_last=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gHx_last=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gHy_last=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gHz_last=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gepsilon_r=(float *)malloc(sizeof(float)*data->zlen*data->ylen);
	data->gy=(float *)malloc(sizeof(float)*data->ylen);

	data->x_mesh=malloc(sizeof(float)*data->zlen);
	data->y_mesh=malloc(sizeof(float)*data->ylen);
	data->z_mesh=malloc(sizeof(float)*data->zlen);

	if (data->use_gpu==TRUE)
	{
		fdtd_opencl_get_mem(sim,data);
	}
}

void fdtd_free_all(struct simulation *sim, struct fdtd_data *data)
{

	int i;
	for (i=0;i<data->zlen;i++)
	{
		free(data->Ex[i]);
		free(data->Ey[i]);
		free(data->Ez[i]);
		free(data->Hx[i]);
		free(data->Hy[i]);
		free(data->Hz[i]);
		free(data->Ex_last[i]);
		free(data->Ey_last[i]);
		free(data->Ez_last[i]);
		free(data->Hx_last[i]);
		free(data->Hy_last[i]);
		free(data->Hz_last[i]);
		free(data->epsilon_r[i]);
	}

	free(data->Ex);
	free(data->Ey);
	free(data->Ez);
	free(data->Hx);
	free(data->Hy);
	free(data->Hz);
	free(data->Ex_last);
	free(data->Ey_last);
	free(data->Ez_last);
	free(data->Hx_last);
	free(data->Hy_last);
	free(data->Hz_last);
	free(data->epsilon_r);


	free(data->gEx);
	free(data->gEy);
	free(data->gEz);
	free(data->gHx);
	free(data->gHy);
	free(data->gHz);
	free(data->gEx_last);
	free(data->gEy_last);
	free(data->gEz_last);
	free(data->gHx_last);
	free(data->gHy_last);
	free(data->gHz_last);
	free(data->gepsilon_r);
	free(data->gy);

	free(data->x_mesh);
	free(data->y_mesh);
	free(data->z_mesh);

	if (data->gnuplot!=NULL)
	{
		fprintf(data->gnuplot, "exit\n");
		fflush(data->gnuplot);
		pclose(data->gnuplot);
	}

	if (data->gnuplot2!=NULL)
	{
		fprintf(data->gnuplot2, "exit\n");
		fflush(data->gnuplot2);
		pclose(data->gnuplot2);
	}

	if (data->use_gpu==TRUE)
	{
		fdtd_opencl_freemem(sim, data);
	}
}
