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



