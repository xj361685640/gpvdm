//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.rodmack.com
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
#include <dll_export.h>
#include <log.h>
#include <lang.h>
#include <util.h>

	#include <dlfcn.h>

#include <cal_path.h>

void run_electrical_dll(struct simulation *sim,struct device *in,char *dll_name)
{
	void *lib_handle;
	void (*init)();
	void (*dll_sim_run)();


	char lib_path[200];

	printf_log(sim,_("Loading electrical dll\n"));

	find_dll(sim, lib_path, dll_name);


	char *error;

	lib_handle = dlopen(lib_path, RTLD_LAZY);

	if (!lib_handle)
	{
		ewe(sim, "%s\n", dlerror());
	}

	init = dlsym(lib_handle, "set_interface");
	if ((error = dlerror()) != NULL)
	{
		ewe(sim, "%s\n", error);
	}

	dll_sim_run = dlsym(lib_handle, "dll_run_simulation");
	if ((error = dlerror()) != NULL)
	{
		ewe(sim, "%s\n", error);
	}


(*dll_sim_run)(sim,in);

	dlclose(lib_handle);

}
