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

/** @file fdtd.c
	@brief Main FDTD interface
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

int dump_number=0;

/*void my_handler(int s)
{
free_all();
           printf_log(sim,"Caught signal %d\n",s);
           exit(1); 

}*/

int do_fdtd(struct simulation *sim,struct device *cell)
{
	printf_log(sim,"**************************\n");
	printf_log(sim,"*       FDTD module      *\n");
	printf_log(sim,"**************************\n");

	FILE * f;
	f=fopen("conv.dat","w");
	fclose(f);

	struct fdtd_data data;

	fdtd_init(&data);

	fdtd_load_config(sim,&data);


	if (data.use_gpu==TRUE)
	{
		opencl_init(sim,&data);
	}

	fdtd_get_mem(sim, &data);

	if (data.use_gpu==TRUE)
	{
		fdtd_opencl_load_code(sim,&data);
		fdtd_opencl_kernel_init(sim, &data);
	}

	fdtd_mesh(sim,&data,cell);

	if (data.plot==1)
	{
		data.gnuplot = popen("gnuplot","w");
		fprintf(data.gnuplot, "set terminal x11 title 'Solarsim' \n");
		fflush(data.gnuplot);
	}


	if (data.use_gpu==TRUE)
	{
		fdtd_opencl_push_to_gpu(sim,&data);
		fdtd_opencl_write_ctrl_data(sim,&data);
	}

	fdtd_solve_all_lambda(sim,cell,&data);

	fdtd_free_all(sim,&data);

	printf("Exit\n");

return 0;

}

