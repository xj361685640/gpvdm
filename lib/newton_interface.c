//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.roderickmackenzie.eu
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
#include "util.h"

	#include <dlfcn.h>

#include "inp.h"
#include "light_interface.h"
#include "const.h"
#include "device.h"
#include "dump_ctrl.h"
#include "config.h"
#include "complex_solver.h"
#include "cal_path.h"
#include "lang.h"
#include "log.h"
#include "newton_interface.h"

static int unused __attribute__((unused));


void newton_init(struct simulation *sim,char *solver_name)
{
//printf_log(sim,_("Solver initialization\n"));
char lib_path[1000];

find_dll(sim, lib_path,solver_name);


char *error;

	sim->dll_solver_handle = dlopen(lib_path, RTLD_LAZY);

	if (!sim->dll_solver_handle)
	{
		ewe(sim,"%s\n", dlerror());
	}

	sim->dll_solve_cur = dlsym(sim->dll_solver_handle, "dll_solve_cur");
	if ((error = dlerror()) != NULL)
	{
		ewe(sim, "%s\n", error);
	}

	sim->dll_solver_realloc = dlsym(sim->dll_solver_handle, "dll_solver_realloc");
	if ((error = dlerror()) != NULL)
	{
		ewe(sim, "%s\n", error);
	}

	sim->dll_solver_free_memory = dlsym(sim->dll_solver_handle, "dll_solver_free_memory");
	if ((error = dlerror()) != NULL)
	{
		ewe(sim, "%s\n", error);
	}



}


int solve_cur(struct simulation *sim,struct device *in,int z,int x)
{
return (*sim->dll_solve_cur)(sim,in,z,x);
}

void newton_set_min_ittr(struct device *in,int ittr)
{
in->newton_min_itt=ittr;
}

void solver_realloc(struct simulation *sim,struct device * in)
{
(*sim->dll_solver_realloc)(sim,in);
}

void solver_free_memory(struct simulation *sim,struct device * in)
{
(*sim->dll_solver_free_memory)(in);
}

void newton_interface_free(struct simulation *sim)
{
dlclose(sim->dll_solver_handle);
}
