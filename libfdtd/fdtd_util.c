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

/** @file fdtd_util.c
	@brief Helper functions for FDTD.
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
#include <fdtd.h>

#include "vec.h"

void fdtd_zero_arrays(struct simulation *sim,struct fdtd_data *data)
{
	fdtd_set_3d_float(data, data->Ex, 0.0);
	fdtd_set_3d_float(data, data->Ey, 0.0);
	fdtd_set_3d_float(data, data->Ez, 0.0);

	fdtd_set_3d_float(data, data->Hx, 0.0);
	fdtd_set_3d_float(data, data->Hy, 0.0);
	fdtd_set_3d_float(data, data->Hz, 0.0);

	fdtd_set_3d_float(data, data->Ex_last, 0.0);
	fdtd_set_3d_float(data, data->Ey_last, 0.0);
	fdtd_set_3d_float(data, data->Ez_last, 0.0);

	fdtd_set_3d_float(data, data->Ex_last_last, 0.0);
	fdtd_set_3d_float(data, data->Ey_last_last, 0.0);
	fdtd_set_3d_float(data, data->Ez_last_last, 0.0);

	fdtd_set_3d_float(data, data->Hx_last, 0.0);
	fdtd_set_3d_float(data, data->Hy_last, 0.0);
	fdtd_set_3d_float(data, data->Hz_last, 0.0);

}


void fdtd_set_lambda(struct simulation *sim,struct fdtd_data *data,struct device *cell,float lambda)
{
	int z;
	int x;
	int y;
	int layer;
	float alpha;
	float n;
	float kappa;

	fdtd_zero_arrays(sim,data);

	data->time=0.0;
	data->step=0;
	data->lambda=lambda;
	data->f=clf/data->lambda;
	data->omega=2.0*3.14159*data->f;

	//float min=1.0/(clf*sqrt(pow(1.0/data->dz,2.0)+pow(1.0/data->dx,2.0)+pow(1.0/data->dy,2.0)));
	//data->dt=1e-18;//min*0.1;
	//

	float min=1.0/(clf*sqrt(pow(1.0/data->dz,2.0)+pow(1.0/data->dx,2.0)+pow(1.0/data->dy,2.0)));
	data->dt=8.359788e-19;//min*0.1;

	printf("dt=%e\n",data->dt);

	data->dt2=data->dt/2.0;
	//fdtd_setup_simulation(sim,data);
	printf(" lambda=%f\n",data->lambda*1e9);

	for (z=0;z<data->zlen;z++)
	{
		for (x=0;x<data->xlen;x++)
		{
			for (y=0;y<data->ylen;y++)
			{
				layer=data->layer[y];
				
				if (layer==-1)
				{
					layer=1;
				}

				n=inter_get_noend(&(cell->my_epitaxy.mat_n[data->layer[y]]),data->lambda);
				data->epsilon_r[z][x][y]=n;//pow(n,2.0);
				alpha=inter_get_noend(&(cell->my_epitaxy.mat[data->layer[y]]),data->lambda);
				kappa=(data->lambda*alpha)/(4.0*M_PI);
				data->sigma[z][x][y]=2.0*n*kappa/mu0f;	//taken from "optical properties of solids" F. Wooten eq. 2.91
				//printf("%e\n",data->sigma[z][x][y]);	
//*epsilon0f*sqrtf(powf((1+(kappa*kappa*2)/(data->omega*data->omega*mu0f*epsilon0f)),2.0)-1.0);

			}
		}
	}

}
