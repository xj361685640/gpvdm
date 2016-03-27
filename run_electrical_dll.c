//    Organic Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for organic solar cells. 
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; version 2 of the License
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <dll_interface.h>
#include <solver_interface.h>
#include <dll_export.h>
#include <log.h>
#include <lang.h>
#include <util.h>

#include <dlfcn.h>

#include <cal_path.h>

void run_electrical_dll(struct device *in, char *dll_name)
{
	void *lib_handle;
	void (*init) ();
	void (*dll_sim_run) ();

	char lib_path[200];
	char lib_name[100];

	printf_log(_("Loading electrical dll\n"));

	sprintf(lib_name, "%s.so", dll_name);

	join_path(2, lib_path, get_solver_path(), lib_name);
	printf_log("I want to open %s %s %s\n", lib_path, get_solver_path(),
		   lib_name);

	char *error;

	lib_handle = dlopen(lib_path, RTLD_LAZY);

	if (!lib_handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(0);
	}

	init = dlsym(lib_handle, "set_interface");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_sim_run = dlsym(lib_handle, "dll_run_simulation");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	(*init) (dll_get_interface());
	(*dll_sim_run) (in);
}
