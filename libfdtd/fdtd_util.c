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

void fdtd_init(struct fdtd_data *data)
{
	data->src_code=NULL;
}

size_t fdtd_load_code(struct simulation *sim,struct fdtd_data *data)
{
	size_t srcsize;
	int file_size=0;
    FILE *in=fopen("./libfdtd/code.cl","r");
	if (in==NULL)
	{
		ewe(sim,"I could not find the OpenCL code\n");
	}

	fseek(in, 0, SEEK_END);
	file_size = ftell(in)+10;
	fseek(in, 0, SEEK_SET);

	data->src_code=malloc(sizeof(char)*file_size);

    srcsize=fread(data->src_code, file_size, 1, in);
    fclose(in);
	return srcsize;
}
