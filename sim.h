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

#ifndef sim_h
#define sim_h
#include "advmath.h"

#include "i.h"
#include "code_ctrl.h"
#include "const.h"
#include "version.h"

#include "device.h"
#include "dos.h"
#include "exp.h"
#include "plot.h"
#include "util.h"
#include "dump.h"
#include "complex_solver.h"
#include "config.h"
#include "advmath.h"
#include "light_interface.h"
#include "newton_interface.h"
#include "lang.h"

char *sim_output_path();
char *sim_input_path();
//newtonsolver
int solve_cur_thermal(struct device *in, int thermal);
int solve_pos(struct device *in);
void get_initial(struct device *in);
void device_get_memory(struct device *in);
void device_free(struct device *in);
void update_arrays(struct device *in);
void device_timestep(struct device *in);
void find_n0(struct device *in);

//from time.c
void time_mesh_save();
void time_load_mesh(struct device *in, int number);
void time_init(struct device *in);
void device_timestep(struct device *in);
int time_run();
gdouble time_get_voltage();
gdouble time_get_sun();
gdouble time_get_laser();
gdouble time_get_fs_laser();
void time_memory_free();
void time_enable_everything(int in);
//
int get_clamp_state();

void get_max_layers(int in);
void lock_main(int argc, char *argv[]);

void init_mat_arrays(struct device *in);

void device_init(struct device *in);
void load_config(struct device *in);
void update(struct device *cell);
int run_simulation(char *outputpath, char *inputpath);
void solve_all(struct device *in);
void solver_free();

//DOS model
void gen_dos_fd_gaus_fd();
//Light
void light_transfer_gen_rate_to_device(struct device *cell, struct light *in);
void solve_light(struct device *cell, struct light *in, gdouble Psun_in,
		 gdouble Plaser_in);
void light_load_dlls(struct light *in, struct device *cell, char *output_path);
void light_transfer_gen_rate_to_device(struct device *cell, struct light *in);
void light_solve_and_update(struct device *cell, struct light *in,
			    gdouble laser_eff_in);
//debug
void stop_start(struct device *in);

#endif
