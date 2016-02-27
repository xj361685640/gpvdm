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
#include <signal.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include "util.h"
#include "sim.h"
#include "dos.h"
#include "dump.h"
#include "complex_solver.h"
#include "elec_plugins.h"
#include "ntricks.h"
#include "log.h"
#include "inp.h"
#include "solver_interface.h"
#include "newton_interface.h"
#include "mesh.h"
#include "remesh.h"

struct device cell;

int run_simulation(char *outputpath, char *inputpath)
{
	char temp[1000];
	printf_log("Run_simulation\n");

	cell.kl_in_newton = FALSE;

	if (strcmp(outputpath, "") != 0)
		strcpy(cell.outputpath, outputpath);

	if (strcmp(inputpath, "") != 0)
		strcpy(cell.inputpath, inputpath);

	dump_init(&cell);
	dump_load_config(&cell);
//printf("%d %s\n",get_dump_status(dump_iodump),runpath);
//getchar();
	int i;

	printf_log("Load config\n");
	load_config(&cell);
	solver_init(cell.solver_name);
	newton_init(cell.newton_name);

	if (strcmp(cell.simmode, "optics") != 0) {
		printf_log("Loading DoS for %d layers\n",
			   cell.my_epitaxy.electrical_layers);
		char tempn[100];
		char tempp[100];
		i = 0;
		for (i = 0; i < cell.my_epitaxy.electrical_layers; i++) {
			dos_init(i);
			printf_log("Load DoS %d/%d\n", i,
				   cell.my_epitaxy.electrical_layers);
			sprintf(tempn, "%s_dosn.dat",
				cell.my_epitaxy.dos_file[i]);
			sprintf(tempp, "%s_dosp.dat",
				cell.my_epitaxy.dos_file[i]);
			load_dos(&cell, tempn, tempp, i);
		}

		if (get_dump_status(dump_write_converge) == TRUE) {
			cell.converge =
			    fopena(cell.outputpath, "converge.dat", "w");
			fclose(cell.converge);

			cell.tconverge =
			    fopena(cell.outputpath, "tconverge.dat", "w");
			fclose(cell.tconverge);
		}
	}
	device_init(&cell);

	join_path(2, temp, cell.outputpath, "equilibrium");
	remove_dir(temp);

	join_path(2, temp, cell.outputpath, "snapshots");
	remove_dir(temp);

	join_path(2, temp, cell.outputpath, "light_dump");
	remove_dir(temp);

	join_path(2, temp, cell.outputpath, "dynamic");
	remove_dir(temp);

	join_path(2, temp, cell.outputpath, "frequency");
	remove_dir(temp);

	mesh_cal_layer_widths(&cell);

	long double depth = 0.0;
	long double percent = 0.0;

	for (i = 0; i < cell.ymeshpoints; i++) {
		depth = cell.ymesh[i] - cell.layer_start[cell.imat[i]];
		percent = depth / cell.layer_width[cell.imat[i]];
		cell.Nad[i] =
		    get_dos_doping_start(cell.imat[i]) +
		    (get_dos_doping_stop(cell.imat[i]) -
		     get_dos_doping_start(cell.imat[i])) * percent;
//      printf("%Le %Le %Le %d %Le\n",depth,percent,cell.Nad[i],cell.imat[i],cell.layer_width[cell.imat[i]]);
	}

	init_mat_arrays(&cell);

//getchar();
	if (strcmp(cell.simmode, "optics") != 0) {
		for (i = 0; i < cell.ymeshpoints; i++) {
			cell.phi[i] = 0.0;
			cell.R[i] = 0.0;
			cell.n[i] = 0.0;
		}

		cell.C =
		    cell.xlen * cell.zlen * epsilon0 * cell.epsilonr[0] /
		    (cell.ylen + cell.other_layers);
		if (get_dump_status(dump_print_text) == TRUE)
			printf_log("C=%Le\n", cell.C);
		cell.A = cell.xlen * cell.zlen;
		cell.Vol = cell.xlen * cell.zlen * cell.ylen;

		light_init(&cell.mylight, &cell, cell.outputpath);
		light_set_dx(&cell.mylight, cell.ymesh[1] - cell.ymesh[0]);
		light_load_config(&cell.mylight);
		if (get_dump_status(dump_iodump) == FALSE)
			set_dump_status(dump_optics, FALSE);

		//update_arrays(&cell);

		cell.Vapplied = 0.0;
		get_initial(&cell);

		remesh_shrink(&cell);

		if (cell.math_enable_pos_solver == TRUE)
			solve_pos(&cell);

		time_init(&cell);

		cell.N = 0;
		cell.M = 0;

		solver_realloc(&cell);

		plot_open(&cell);

		plot_now(&cell, "plot");
		//set_solver_dump_every_matrix(1);

		find_n0(&cell);
		//set_solver_dump_every_matrix(0);
		draw_gaus(&cell);

		if (cell.onlypos == TRUE) {
			join_path(2, temp, cell.outputpath, "equilibrium");
			dump_1d_slice(&cell, temp);
			device_free(&cell);
			return 0;
		}
	}

	if (is_domain(cell.simmode) != 0) {
		char gussed_full_mode[200];
		if (guess_whole_sim_name
		    (gussed_full_mode, cell.inputpath, cell.simmode) == 0) {
			printf_log("I guess we are using running %s\n",
				   gussed_full_mode);
			strcpy(cell.simmode, gussed_full_mode);
		} else {
			ewe("I could not guess which simulation to run from the mode %s\n", cell.simmode);
		}

	}
#include "run_list.c"

	device_free(&cell);

	if (strcmp(cell.simmode, "optics") != 0) {
		plot_close(&cell);

		for (i = 0; i < cell.my_epitaxy.electrical_layers; i++) {
			dos_free(i);
		}

		solver_free_memory(&cell);

		light_free(&cell.mylight);
	}
	solver_interface_free();
	newton_interface_free();

	return cell.odes;
}

char *sim_output_path()
{
	return cell.outputpath;
}

char *sim_input_path()
{
	return cell.inputpath;
}
