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

/** @file fdtd_mesh.c
	@brief Setup the FDTD mesh.
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
	data->xsize=data->xsize*data->ysize;
	float device_start=(float)epitaxy_get_device_start(&(cell->my_epitaxy));
	float device_stop=(float)epitaxy_get_device_stop(&(cell->my_epitaxy));

	float start_y=device_start+(device_stop-device_start)/2.0;
	data->excitation_mesh_point=data->ylen*(start_y/data->ysize);

	data->dz=data->zsize/((float)data->zlen);
	data->dx=data->xsize/((float)data->xlen);
	data->dy=data->ysize/((float)data->ylen);


	printf ("dy=%lf nm, dz=%lf nm dt=%le %le\n",data->dy*1e9,data->dz*1e9,data->dt,data->time);

	int z=0;
	int x=0;
	int y=0;

	int layer=0;

	float zpos=data->dz/2.0;
	float xpos=data->dx/2.0;
	float ypos=data->dy/2.0;


	for (z=0;z<data->zlen;z++)
	{
		data->z_mesh[z]=zpos;
		zpos+=data->dz;
	}

	for (x=0;x<data->xlen;x++)
	{
		data->x_mesh[x]=xpos;
		xpos+=data->dx;
	}

	for (y=0;y<data->ylen;y++)
	{
		data->y_mesh[y]=ypos;
		data->layer[y]=epitaxy_get_optical_material_layer(&cell->my_epitaxy,ypos);
		ypos+=data->dy;
	}



}
