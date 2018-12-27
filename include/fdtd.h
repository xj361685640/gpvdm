//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


/** @file fdtd.h
	@brief Header for FDTD functions.
*/


#ifndef h_fdtd
#define h_fdtd
#include <complex.h>
#include "advmath.h"
#include <sim_struct.h>
#include <device.h>

#define use_open_cl

#ifdef use_open_cl
	#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#endif

struct fdtd_data
{


	float ***Ex;
	float ***Ey;
	float ***Ez;
	float ***Hx;
	float ***Hy;
	float ***Hz;
	float ***Ex_last;
	float ***Ey_last;
	float ***Ez_last;
	float ***Ex_last_last;
	float ***Ey_last_last;
	float ***Ez_last_last;
	float ***Hx_last;
	float ***Hy_last;
	float ***Hz_last;
	float ***epsilon_r;
	float ***z_ang;
	float dt;
	float dt2;
	float zsize;
	float xsize;
	float ysize;
	int *layer;

	float *gEx;
	float *gEy;
	float *gEz;
	float *gHx;
	float *gHy;
	float *gHz;
	float *gEx_last;
	float *gEy_last;
	float *gEz_last;
	float *gHx_last;
	float *gHy_last;
	float *gHz_last;
	float *gepsilon_r;
	float *gy;

	//axis
	float *z_mesh;
	float *x_mesh;
	float *y_mesh;

	float dx;
	float dy;
	float dz;

	float f;
	float omega;
	int excitation_mesh_point;
	char *src_code;
	int use_gpu;
	#ifdef use_open_cl

		
		cl_context context;
		cl_device_id device;

		//opengl memory
		cl_mem ggEx;
		cl_mem ggEy;
		cl_mem ggEz;

		cl_mem ggHx;
		cl_mem ggHy;
		cl_mem ggHz;

		cl_mem ggEx_last;
		cl_mem ggEy_last;
		cl_mem ggEz_last;

		cl_mem ggHx_last;
		cl_mem ggHy_last;
		cl_mem ggHz_last;

		cl_mem ggepsilon_r;

		cl_mem ggy;
		cl_mem ggC;

		cl_program prog;
		cl_command_queue cq;

		cl_kernel cal_E;
		cl_kernel update_E;
		cl_kernel cal_H;
		cl_kernel update_H;
	#endif

	//config
	int lam_jmax;
	float gap;
	int plot;
	FILE *gnuplot;
	FILE *gnuplot2;
	int zlen;
	int xlen;
	int ylen;
	int max_ittr;

	float src_start;
	float src_stop;
	float lambda;
	float stop;
	float time;
};

int do_fdtd(struct simulation *sim,struct device *cell);
void fdtd_free_all(struct simulation *sim, struct fdtd_data *data);
void fdtd_opencl_load_config(struct simulation *sim, struct fdtd_data *data);
void opencl_init(struct simulation *sim, struct fdtd_data *data);
size_t fdtd_opencl_load_code(struct simulation *sim,struct fdtd_data *data);
void fdtd_opencl_kernel_init(struct simulation *sim, struct fdtd_data *data);
void fdtd_setup_simulation(struct simulation *sim,struct fdtd_data *data);
void fdtd_opencl_push_to_gpu(struct simulation *sim,struct fdtd_data *data);
void fdtd_opencl_write_ctrl_data(struct simulation *sim,struct fdtd_data *data);
void fdtd_solve_step(struct simulation *sim,struct fdtd_data *data);
int fdtd_opencl_solve_step(struct simulation *sim,struct fdtd_data *data);
void fdtd_opencl_pull_data(struct simulation *sim,struct fdtd_data *data);
void fdtd_dump(struct simulation *sim,struct fdtd_data *data);
void fdtd_opencl_get_mem(struct simulation *sim, struct fdtd_data *data);
void fdtd_opencl_freemem(struct simulation *sim, struct fdtd_data *data);
void fdtd_mesh(struct simulation *sim,struct fdtd_data *data,struct device *cell);
void fdtd_set_3d_float(struct fdtd_data *in, float ***var, float value);
void fdtd_free_3d_float(struct fdtd_data *in, float ***var);
void fdtd_3d_malloc_float(struct fdtd_data *in, float * (***var));
void fdtd_memcpy(struct fdtd_data *data, float ***out, float ***in);

#endif
