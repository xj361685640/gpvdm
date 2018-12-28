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

/** @file fdtd_lambda.c
	@brief Runs the fdtd code over multiple wavelengths.
*/

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
#include <device.h>
#include <fdtd.h>

#include "vec.h"
void fdtd_solve_all_lambda(struct simulation *sim,struct device *cell,struct fdtd_data *data)
{
	FILE * f;
	printf("here\n");
	int i=0;
	float lambda=data->lambda_start;
	int steps=data->lambda_points;
	printf("%d\n",steps);

	float dl=(data->lambda_stop-data->lambda_start)/((float)steps);
	for (i=0;i<steps;i++)
	{
		fdtd_solve_lambda(sim,data,cell,lambda);

		f=fopen("escape.dat","a");
		fprintf(f,"%e %le\n",lambda,data->escape);
		fclose(f);

		lambda=lambda+dl;
		printf("lambda= %.0f nm\n",lambda*1e9);
	}

}

void fdtd_solve_lambda(struct simulation *sim,struct fdtd_data *data,struct device *cell,float lambda)
{
	fdtd_set_lambda(sim,data,cell,lambda);

	do
	{
		int err;
		fdtd_solve_step(sim,data);

		if ((data->step%200)==0)
		{
			if (data->use_gpu==TRUE)
			{
				fdtd_opencl_pull_data(sim,data);
			}

			fdtd_dump(sim,data);

			printf_log(sim,"plot! %ld\n",data->step);

			if (data->plot==1)
			{
				fprintf(data->gnuplot2, "load 'Ex.plot'\n");
				fflush(data->gnuplot2);
			}
		printf_log(sim,"%ld/%ld %e s\n",data->step,data->max_ittr,data->time);
		}
		//exit(0);

		data->step++;

		/*if (fdtd_test_conv(sim,data)!=0)
		{
			printf("break\n");
			break;
		}*/

	}while(data->step<data->max_ittr);

}
