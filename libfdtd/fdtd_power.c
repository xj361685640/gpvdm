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

/** @file fdtd_power.c
	@brief Calculate fdtd power.
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


float fdtd_power_zxy(struct simulation *sim,struct fdtd_data *data,int z, int x, int y)
{
	float e_power=0.0;
	float h_power=0.0;

	e_power=0.5*epsilon0f*(data->Ex[z][x][y]*data->Ex[z][x][y]+data->Ey[z][x][y]*data->Ey[z][x][y]+data->Ez[z][x][y]*data->Ez[z][x][y]);
	h_power=0.5*mu0f*(data->Hx[z][x][y]*data->Hx[z][x][y]+data->Hy[z][x][y]*data->Hy[z][x][y]+data->Hz[z][x][y]*data->Hz[z][x][y]);

	return e_power+h_power;
}

float fdtd_power_y(struct simulation *sim,struct fdtd_data *data, int y)
{
	int z;
	int x;
	float tot=0.0;
	for (z=0;z<data->zlen;z++)
	{
		for (x=0;x<data->xlen;x++)
		{
			tot=tot+fdtd_power_zxy(sim,data,z, x,y);
		}
	}

	return tot;
}

float fdtd_test_conv(struct simulation *sim,struct fdtd_data *data)
{
float ret=0.0;
float src=fdtd_power_zxy(sim,data,0, data->excitation_mesh_point_x, data->excitation_mesh_point_y);
float almost_top=fdtd_power_y(sim,data,  data->ylen-data->ylen/4);
float top=fdtd_power_y(sim,data,  data->ylen-2);

	if (top!=0.0)
	{
		ret=almost_top/src;
		data->escape=ret;
	}

return ret;
}
