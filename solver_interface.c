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
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
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

#include <dlfcn.h>

#include <sys/types.h>
#include <dirent.h>
#include "util.h"
#include "inp.h"
#include "const.h"
#include "device.h"
#include "dump_ctrl.h"
#include "config.h"
#include "complex_solver.h"
#include "cal_path.h"
#include "lang.h"
#include "log.h"
#include "dll_interface.h"

static int unused __attribute__ ((unused));
static void (*dll_matrix_solve) ();
static void (*dll_matrix_dump) ();
static void (*dll_set_interface) ();
static void (*dll_matrix_solver_free) ();

static void *dll_handle;

void solver_init(char *solver_name)
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

	dll_matrix_solve = dlsym(dll_handle, "dll_matrix_solve");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_matrix_dump = dlsym(dll_handle, "dll_matrix_dump");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_set_interface = dlsym(dll_handle, "set_interface");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_matrix_solver_free = dlsym(dll_handle, "dll_matrix_solver_free");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_interface_fixup();
	(*dll_set_interface) (dll_get_interface());

}

void solver(int col, int nz, int *Ti, int *Tj, long double *Tx, long double *b)
{
	(*dll_matrix_solve) (col, nz, Ti, Tj, Tx, b);
}

void dump_matrix(int col, int nz, int *Ti, int *Tj, long double *Tx,
		 long double *b, char *index)
{
	(*dll_matrix_dump) (col, nz, Ti, Tj, Tx, b, index);
}

void solver_free()
{
	(*dll_matrix_solver_free) ();
}

void solver_interface_free()
{
	dlclose(dll_handle);
}
