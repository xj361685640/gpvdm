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
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
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
	inp_search_float(sim,&inp,&(data->gap),"#gap");
	inp_search_int(sim,&inp,&(data->max_ittr),"#max_ittr");
	inp_search_int(sim,&inp,&(data->ylen),"#ylen");
	printf_log(sim,"ylen=%d\n",data->ylen);
	inp_search_int(sim,&inp,&(data->zlen),"#zlen");
	printf_log(sim,"zlen=%d\n",data->zlen);
	inp_search_float(sim,&inp,&(data->ysize),"#ysize");
	printf_log(sim,"ysize=%e\n",data->ysize);
	inp_search_float(sim,&inp,&(data->zsize),"#zsize");
	printf_log(sim,"zsize=%e\n",data->zsize);
	inp_search_int(sim,&inp,&(data->lam_jmax),"#lam_jmax");
	printf_log(sim,"lam_jmax=%d\n",data->lam_jmax);
	inp_search_float(sim,&inp,&(data->sithick),"#sithick");
	printf_log(sim,"sithick=%e\n",data->sithick);
	inp_search_int(sim,&inp,&(data->plot),"#plot");
	inp_free(sim,&inp);

}
