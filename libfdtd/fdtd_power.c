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


void fdtd_power_xy(struct simulation *sim,struct fdtd_data *data,int z, int y)
{
	float e_power=0.0;
	float h_power=0.0;

	e_power=0.5*epsilon0f*(data->Ex[z][y]*data->Ex[z][y]+data->Ey[z][y]*data->Ey[z][y]+data->Ez[z][y]*data->Ez[z][y]);
	h_power=0.5*mu0f*(data->Hx[z][y]*data->Hx[z][y]+data->Hy[z][y]*data->Hy[z][y]+data->Hz[z][y]*data->Hz[z][y]);

	return e_power+h_power;
}

