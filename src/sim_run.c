//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
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

#include "util.h"
#include "sim.h"
#include "dos.h"
#include "dump.h"
#include "complex_solver.h"
#include "ntricks.h"
#include "log.h"
#include "inp.h"
#include "solver_interface.h"
#include "newton_interface.h"
#include "mesh.h"
#include "remesh.h"
#include "lang.h"
#include <plot.h>
#include "device.h"
#include <dll_interface.h>
#include <cal_path.h>

int run_simulation(struct simulation *sim, char *outputpath, char *inputpath)
{
	struct device cell;

	printf_log(sim, _("Run_simulation\n"));

	device_init(&cell);
	cell.onlypos = FALSE;

	cell.root_dll_interface = dll_get_interface();
	cal_path(sim);

	dump_init(sim, &cell);

	set_dump_status(sim, dump_stop_plot, FALSE);
	set_dump_status(sim, dump_print_text, TRUE);

	char temp[1000];

	cell.kl_in_newton = FALSE;

	if (strcmp(outputpath, "") != 0)
		strcpy(get_output_path(sim), outputpath);

	if (strcmp(inputpath, "") != 0)
		strcpy(get_input_path(sim), inputpath);

	dump_load_config(sim, &cell);

//printf("%d %s\n",get_dump_status(sim,dump_iodump),runpath);
//getchar();
	int i;

	printf_log(sim, "Load config\n");
	load_config(sim, &cell);

	solver_init(sim, cell.solver_name);
	newton_init(sim, cell.newton_name);

	if (strcmp(cell.simmode, "optics") != 0) {
		printf_log(sim, _("Loading DoS for %d layers\n"),
			   cell.my_epitaxy.electrical_layers);
		char tempn[100];
		char tempp[100];
		i = 0;
		for (i = 0; i < cell.my_epitaxy.electrical_layers; i++) {
			dos_init(i);
			printf_log(sim, "Load DoS %d/%d\n", i,
				   cell.my_epitaxy.electrical_layers);
			sprintf(tempn, "%s_dosn.dat",
				cell.my_epitaxy.dos_file[i]);
			sprintf(tempp, "%s_dosp.dat",
				cell.my_epitaxy.dos_file[i]);
			load_dos(sim, &cell, tempn, tempp, i);
		}

		printf("%ld\n", cell.srh_bands);
		getchar();
		device_alloc_traps(&cell);

		if (get_dump_status(sim, dump_write_converge) == TRUE) {
			sim->converge =
			    fopena(get_output_path(sim), "converge.dat", "w");
			fclose(sim->converge);

			sim->tconverge =
			    fopena(get_output_path(sim), "tconverge.dat", "w");
			fclose(sim->tconverge);
		}
	}

	join_path(2, temp, get_output_path(sim), "equilibrium");
	remove_dir(sim, temp);

	join_path(2, temp, get_output_path(sim), "snapshots");
	remove_dir(sim, temp);

	join_path(2, temp, get_output_path(sim), "light_dump");
	remove_dir(sim, temp);

	join_path(2, temp, get_output_path(sim), "dynamic");
	remove_dir(sim, temp);

	join_path(2, temp, get_output_path(sim), "frequency");
	remove_dir(sim, temp);

	printf("load %ld\n", cell.ymeshpoints);
	getchar();

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
		if (get_dump_status(sim, dump_print_text) == TRUE)
			printf_log(sim, "C=%Le\n", cell.C);
		cell.A = cell.xlen * cell.zlen;
		cell.Vol = cell.xlen * cell.zlen * cell.ylen;

		light_init(&cell.mylight, &cell);
		light_set_dx(&cell.mylight, cell.ymesh[1] - cell.ymesh[0]);
		light_load_config(sim, &cell.mylight);
		light_load_dlls(sim, &cell.mylight, &cell);

		if (get_dump_status(sim, dump_iodump) == FALSE)
			set_dump_status(sim, dump_optics, FALSE);

		//update_arrays(&cell);

		cell.Vapplied = 0.0;
		get_initial(sim, &cell);

		remesh_shrink(&cell);

		if (cell.math_enable_pos_solver == TRUE)
			solve_pos(sim, &cell);

		time_init(&cell);

		cell.N = 0;
		cell.M = 0;

		solver_realloc(&cell);

		plot_open(sim);

		plot_now(sim, "plot");
		//set_solver_dump_every_matrix(1);

		find_n0(sim, &cell);
		//set_solver_dump_every_matrix(0);
		draw_gaus(&cell);

		if (cell.onlypos == TRUE) {
			join_path(2, temp, get_output_path(sim), "equilibrium");
			dump_1d_slice(sim, &cell, temp);
			mesh_free(sim, &cell);
			return 0;
		}
	}

	if (is_domain(cell.simmode) != 0) {
		char gussed_full_mode[200];
		if (guess_whole_sim_name
		    (sim, gussed_full_mode, get_input_path(sim),
		     cell.simmode) == 0) {
			printf_log(sim, "I guess we are using running %s\n",
				   gussed_full_mode);
			strcpy(cell.simmode, gussed_full_mode);
		} else {
			ewe(sim,
			    "I could not guess which simulation to run from the mode %s\n",
			    cell.simmode);
		}

	}

	run_electrical_dll(sim, &cell, strextract_domain(cell.simmode));

	mesh_free(sim, &cell);

	if (strcmp(cell.simmode, "optics") != 0) {
		plot_close(sim);

		for (i = 0; i < cell.my_epitaxy.electrical_layers; i++) {
			dos_free(i);
		}

		solver_free_memory(&cell);

	}
	solver_interface_free();
	newton_interface_free();
	light_free(sim, &cell.mylight);

	return cell.odes;
}
