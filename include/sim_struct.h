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



#ifndef sim_struct_h
#define sim_struct_h

#include <stdio.h>
#include <server_struct.h>

struct dumpfiles_struct
{
char filename[100];
int dump;
int ynorm;
};

struct simulation
{
	//plotting
	FILE *gnuplot;
	FILE *gnuplot_time;
	FILE *converge;
	FILE *tconverge;
	//dump
	int dump_array[100];
	int dumpfiles;
	struct dumpfiles_struct *dumpfile;

	int log_level;
	//paths
	char plugins_path[400];
	char lang_path[400];
	char input_path[400];
	char output_path[400];
	char share_path[400];
	char exe_path[400];
	char materials_path[400];
	//Matrix solver
	int last_col;
	int last_nz;
	double *x;
	int *Ap;
	int *Ai;
	double *Ax;
	double *b;
	double *Tx;
	//complex solver
	int complex_last_col;
	int complex_last_nz;
	double *complex_x;
	double *complex_xz;
	int *complex_Ap;
	int *complex_Ai;
	double *complex_Ax;
	double *complex_Az;

	//Matrix solver dlls
	void (*dll_matrix_solve)();
	void (*dll_matrix_dump)();
	void (*dll_set_interface)();
	void (*dll_matrix_solver_free)();
	void *dll_matrix_handle;

	//Solve dlls
	int (*dll_solve_cur)();
	int (*dll_solver_realloc)();
	int (*dll_solver_free_memory)();
	void *dll_solver_handle;
	char force_sim_mode[100];

	//Fitting vars
	double last_total_error;

	struct server_struct server;

	int gui;
	int html;
	long int bytes_written;
	long int bytes_read;
	long int files_read;
	long int files_written;	

};

#endif

