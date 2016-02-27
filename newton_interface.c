//    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for 1st, 2nd and 3rd generation solar cells.
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
#include <unistd.h>
#include "util.h"

#include <dlfcn.h>

#include <sys/types.h>
#include <dirent.h>
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
#include "dll_interface.h"
#include "newton_interface.h"

static int unused __attribute__ ((unused));
static void (*dll_set_interface) ();
static void (*dll_newton_set_min_ittr) ();
static int (*dll_solve_cur) ();
static int (*dll_solver_realloc) ();
static int (*dll_solver_free_memory) ();

static void *dll_handle;

void newton_init(char *solver_name)
{
	printf_log(_("Solver initialization\n"));
	char lib_name[100];
	char lib_path[1000];
	sprintf(lib_name, "%s.so", solver_name);

	join_path(2, lib_path, get_solver_path(), lib_name);
	printf_log("I want to open %s %s %s\n", lib_path, get_solver_path(),
		   lib_name);

	char *error;

	dll_handle = dlopen(lib_path, RTLD_LAZY);

	if (!dll_handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(0);
	}

	dll_solve_cur = dlsym(dll_handle, "dll_solve_cur");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_newton_set_min_ittr = dlsym(dll_handle, "dll_newton_set_min_ittr");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_set_interface = dlsym(dll_handle, "set_interface");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_solver_realloc = dlsym(dll_handle, "dll_solver_realloc");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_solver_free_memory = dlsym(dll_handle, "dll_solver_free_memory");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_interface_fixup();
	(*dll_set_interface) (dll_get_interface());

}

int solve_cur(struct device *in)
{
	printf("roderick\n");
	return (*dll_solve_cur) (in);
}

void newton_set_min_ittr(int ittr)
{
	printf("roderick2\n");

	(*dll_newton_set_min_ittr) (ittr);
}

void solver_realloc(struct device *in)
{
	printf("roderick3\n");

	(*dll_solver_realloc) (in);
	printf("roderick4\n");
}

void solver_free_memory(struct device *in)
{
	printf("roderick4\n");

	(*dll_solver_free_memory) (in);
}

void newton_interface_free()
{
	dlclose(dll_handle);
}
