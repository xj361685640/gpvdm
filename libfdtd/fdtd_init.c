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

/** @file fdtd_init.c
	@brief Init the fdtd struct.
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

void fdtd_init(struct fdtd_data *data)
{
	data->src_code=NULL;
	data->gnuplot=NULL;
	data->gnuplot2=NULL;
	data->zlen=-1;
	data->xlen=-1;
	data->ylen=-1;
	data->max_ittr=-1;
	data->src_start=-1;
	data->src_stop=-1;
	data->lambda=-1;
	data->stop=1e-6;
	data->time=0.0;
	data->use_gpu=FALSE;
	data->excitation_mesh_point=-1;
	data->step=0;
	data->escape=-1;
}
