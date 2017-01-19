//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


#include <stdio.h>
#include <stdlib.h>

	#include <dlfcn.h>

#include "util.h"
#include "inp.h"
#include "const.h"
#include "device.h"
#include "dump_ctrl.h"
#include "config.h"
#include "complex_solver.h"
#include "cal_path.h"
#include "lang.h"
#include <log.h>

static int unused __attribute__((unused));

void solver_init(struct simulation *sim,char *solver_name)
{
char lib_path[1000];

find_dll(sim, lib_path,solver_name);

char *error;

	sim->dll_matrix_handle = dlopen(lib_path, RTLD_LAZY);

	if (!sim->dll_matrix_handle)
	{
		ewe(sim, "%s\n", dlerror());
	}

	sim->dll_matrix_solve = dlsym(sim->dll_matrix_handle, "dll_matrix_solve");
	if ((error = dlerror()) != NULL)
	{
		ewe(sim, "%s\n", error);
	}

	sim->dll_matrix_dump = dlsym(sim->dll_matrix_handle, "dll_matrix_dump");
	if ((error = dlerror()) != NULL)
	{
		ewe(sim, "%s\n", error);
	}

	sim->dll_set_interface = dlsym(sim->dll_matrix_handle, "set_interface");
	if ((error = dlerror()) != NULL)
	{
		ewe(sim, "%s\n", error);
	}

	sim->dll_matrix_solver_free = dlsym(sim->dll_matrix_handle, "dll_matrix_solver_free");
	if ((error = dlerror()) != NULL)
	{
		ewe(sim, "%s\n", error);
	}



}


void solver(struct simulation *sim,int col,int nz,int *Ti,int *Tj, long double *Tx, long double *b)
{
(*sim->dll_matrix_solve)(sim,col,nz,Ti,Tj,Tx,b);
}

void dump_matrix(struct device *in)
{
int i;
	for (i=0;i<in->N;i++)
	{
		printf("%ld %ld %Le\n",in->Ti[i],in->Tj[i],in->Tx[i]);
	}
	//(*sim->dll_matrix_dump)(sim,col,nz,Ti,Tj, Tx,b,index);
}

void solver_free(struct simulation *sim)
{
if (sim->dll_matrix_handle!=NULL)
{ 
	(*sim->dll_matrix_solver_free)(sim);


	printf("DEALLOC=%p\n",sim->dll_matrix_handle);

	if (dlclose(sim->dll_matrix_handle)!=0)
	{
		ewe(sim,"Error closing dll\n");
	}
	
	sim->dll_matrix_handle=NULL;
	sim->dll_matrix_solve=NULL;
	sim->dll_matrix_dump=NULL;
	sim->dll_set_interface=NULL;
	sim->dll_matrix_solver_free=NULL;

}
}

