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


void fdtd_mesh(struct simulation *sim,struct fdtd_data *data,struct device *cell)
{
	data->ysize=(float)epitaxy_get_optical_length(&cell->my_epitaxy);
	data->zsize=data->zsize*data->ysize;
	float device_start=(float)epitaxy_get_device_start(&(cell->my_epitaxy));
	float device_stop=(float)epitaxy_get_device_stop(&(cell->my_epitaxy));

	float start_y=device_start+(device_stop-device_start)/2.0;
	data->excitation_mesh_point=data->ylen*(start_y/data->ysize);

	data->dy=data->ysize/((float)data->ylen);
	data->dz=data->zsize/((float)data->zlen);
	data->dx=data->zsize/((float)data->zlen);

	float min=1.0/(clf*sqrt(pow(1.0/data->dy,2.0)+pow(1.0/data->dz,2.0)));
	data->dt=min*0.1;
	printf ("dy=%lf nm, dz=%lf nm min_dt=%le dt=%le %le\n",data->dy*1e9,data->dz*1e9,min,data->dt,data->time);

	int i;
	int y;
	int layer;

	float zpos=data->dz/2.0;
	float ypos=data->dy/2.0;

	for (y=0;y<data->ylen;y++)
	{
		zpos=0.0;
		//printf("here\n");
		data->layer[y]=epitaxy_get_optical_material_layer(&cell->my_epitaxy,ypos);
		
		for (i=0;i<data->zlen;i++)
		{
			
			if (layer==-1)
			{
				layer=1;
			}
			data->epsilon_r[i][y]=pow(inter_get_noend(&(cell->my_epitaxy.mat_n[data->layer[y]]),data->lambda),2.0);
			//printf("%f\n",data->epsilon_r[i][y]);
			//getchar();
			data->z_mesh[i]=zpos;

			zpos+=data->dz;

		}
	
	data->y_mesh[y]=ypos;
	ypos+=data->dy;
	}


}

void fdtd_setup_simulation(struct simulation *sim,struct fdtd_data *data)
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

	fdtd_set_3d_float(data, data->z_ang, 0.0);

}



