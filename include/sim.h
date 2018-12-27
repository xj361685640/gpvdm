//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012-2017 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

/** @file sim.h
@brief functions which work on the sim.h structure.
*/


#ifndef sim_h
#define sim_h

#include "code_ctrl.h"
#include "const.h"
#include <stdio.h>
#include <sim_struct.h>

#include "version.h"
#include "device.h"
#include <util.h>
#include "light_interface.h"
#include "newton_interface.h"


//newtonsolver
int solve_cur_thermal(struct device *in,int thermal, int z, int x);
int solve_pos(struct simulation *sim,struct device *in, int z, int x);
void get_initial(struct simulation *sim,struct device *in);
void update_y_array(struct simulation *sim,struct device *in,int z,int x);
void find_n0(struct simulation *sim,struct device *in);

//from time.c
void time_mesh_save(struct simulation *sim,struct device *in);
void time_load_mesh(struct simulation *sim,struct device *in,int number);
void time_init(struct simulation *sim,struct device *in);
void time_store(struct simulation *sim,struct device *in);
void device_timestep(struct simulation *sim,struct device *in);
int time_test_last_point(struct device *in);
gdouble time_get_voltage(struct device *in);
gdouble time_get_sun(struct device *in);
gdouble time_get_laser(struct device *in);
gdouble time_get_fs_laser(struct device *in);
void time_memory_free(struct device *in);
//
int get_clamp_state();

void get_max_layers(int in);
void lock_main(int argc, char *argv[]);

void init_mat_arrays(struct device *in);



void load_config(struct simulation *sim,struct device *in);
void update(struct device *cell);
int run_simulation(struct simulation *sim);
void solve_all(struct simulation *sim,struct device *in);

//Light
void light_transfer_gen_rate_to_device(struct device *cell,struct light *in);
void solve_light(struct device *cell,struct light *in,gdouble Psun_in,gdouble Plaser_in);
void light_load_dlls(struct simulation *sim,struct light *in);
void light_transfer_gen_rate_to_device(struct device *cell,struct light *in);
void light_solve_and_update(struct simulation *sim,struct device *cell,struct light *in,gdouble laser_eff_in);
void light_init(struct light *in);

//debug
void stop_start(struct simulation *sim,struct device *in);
void run_electrical_dll(struct simulation *sim,struct device *in,char *dll_name);
void gen_dos_fd_gaus_fd(struct simulation *sim);
void sim_init(struct simulation *sim);
void fit_log_init(struct simulation *sim);
void light_setup_ray(struct simulation *sim,struct device *cell,struct light *in,struct epitaxy *my_epitaxy);

#endif

