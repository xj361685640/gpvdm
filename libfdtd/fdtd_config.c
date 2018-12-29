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

/** @file fdtd_config.c
	@brief Loads the fdtd config file.
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


void fdtd_load_config(struct simulation *sim, struct fdtd_data *data)
{
	struct inp_file inp;

	inp_init(sim,&inp);
	if (inp_load_from_path(sim,&inp,sim->input_path,"fdtd.inp")!=0)
	{
		printf_log(sim,"can't find file the fdtd config file",sim->input_path);
		exit(0);
	}

	inp_check(sim,&inp,1.0);
	inp_search_float(sim,&inp,&(data->lambda),"#lambda");
	data->use_gpu=inp_search_english(sim,&inp,"#use_gpu");
	inp_search_int(sim,&inp,&(data->max_ittr),"#max_ittr");

	inp_search_int(sim,&inp,&(data->zlen),"#zlen");
	data->zlen=1.0;
	printf_log(sim,"zlen=%d\n",data->zlen);

	inp_search_int(sim,&inp,&(data->xlen),"#xlen");
	printf_log(sim,"xlen=%d\n",data->xlen);

	inp_search_int(sim,&inp,&(data->ylen),"#ylen");
	printf_log(sim,"ylen=%d\n",data->ylen);

	//inp_search_float(sim,&inp,&(data->zsize),"#zsize");
	data->zsize=1.0;
	printf_log(sim,"zsize=%e\n",data->zsize);

	inp_search_float(sim,&inp,&(data->xsize),"#xsize");
	printf_log(sim,"xsize=%e\n",data->xsize);

	inp_search_int(sim,&inp,&(data->lam_jmax),"#lam_jmax");
	printf_log(sim,"lam_jmax=%d\n",data->lam_jmax);
	inp_search_int(sim,&inp,&(data->plot),"#plot");

	inp_search_float(sim,&inp,&(data->dt),"#dt");
	printf_log(sim,"dt=%e\n",data->dt);

	inp_search_float(sim,&inp,&(data->lambda_start),"#fdtd_lambda_start");
	printf_log(sim,"lambda_start=%e\n",data->lambda_start);

	inp_search_float(sim,&inp,&(data->lambda_stop),"#fdtd_lambda_stop");
	printf_log(sim,"lambda_stop=%e\n",data->lambda_stop);

	inp_search_int(sim,&inp,&(data->lambda_points),"#fdtd_lambda_points");
	printf_log(sim,"lambda_points=%e\n",data->lambda_points);

	inp_free(sim,&inp);

	data->src_start=10e-9;
	data->src_stop=20e-9;

}
