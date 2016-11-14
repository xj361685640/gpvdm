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


#ifndef h_light
#define h_light
#include <complex.h>
#include "advmath.h"
#include "i.h"
#include <sim_struct.h>
#include <epitaxy.h>
#include <ray.h>

struct light
{
char config_file[300];
int points;
int lpoints;
gdouble *x;
gdouble dx;
gdouble **Ep;
gdouble **Epz;
gdouble **En;
gdouble **Enz;
gdouble **n;
gdouble **alpha0;
gdouble **alpha;
gdouble **photons;
gdouble **photons_asb;
gdouble **pointing_vector;
gdouble *photons_tot;
gdouble **E_tot_r;
gdouble **E_tot_i;
gdouble **H;
gdouble *reflect;
int *layer;
gdouble *sun_E;
gdouble *H1d;
int layers;
gdouble ylen;
struct istruct *mat;
struct istruct *mat_n;
struct istruct sun_read;
gdouble *sun;
gdouble *sun_norm;
gdouble *sun_photons;
gdouble *thick;
char **material_dir_name;
char suns_spectrum_file[200];
int M;
int N;
int *Ti;
int *Tj;
double *Tx;
double *Txz;
double *b;
double *bz;
gdouble lstart;
gdouble lstop;
gdouble *l;
gdouble *Gn;
gdouble *Gp;
gdouble dl;
gdouble laser_wavelength;
int laser_pos;
gdouble ND;
gdouble spotx;
gdouble spoty;
gdouble pulseJ;
gdouble pulse_width;
gdouble complex **t;
gdouble complex **r;
gdouble complex **nbar;
gdouble *layer_end;
gdouble device_start;
gdouble *G_percent;
int align_mesh;
int flip_field;
int device_start_layer;
int device_start_i;
int disable_transfer_to_electrical_mesh;
gdouble device_ylen;
gdouble Eg;
gdouble Psun;
gdouble laser_eff;
gdouble simplephotondensity;
gdouble simple_alpha;
gdouble Dphotoneff;
void (*fn_init)();
void (*fn_solve_and_update)();
int (*fn_solve_lam_slice)();
gdouble (*fn_cal_photon_density)();
void (*light_ver)();
void *lib_handle;
char mode[20];
struct epitaxy my_epitaxy;
gdouble electron_eff;
gdouble hole_eff;
int force_update;

//Ray tracing
int ray_trace;
long double extract_eff;

struct image my_image;
};

void light_norm_photon_density(struct light *in);
void light_memory(struct simulation *sim,struct light *in);
void light_load_materials(struct simulation *sim,struct light *in);
gdouble light_cal_photon_density(struct light *in);
void light_load_config(struct simulation *sim,struct light *in);
void light_load_config_file(struct simulation *sim,struct light *in);
void light_init_mesh(struct simulation *sim,struct light *in);
void light_set_sun_power(struct light *in,gdouble power, gdouble laser_eff);
void light_free_memory(struct simulation *sim,struct light *in);
void light_get_mode(struct istruct *mode,int lam,struct light *in);
void light_set_unity_power(struct light *in);
void light_solve_optical_problem(struct simulation *sim,struct light *in);
void light_solve_all(struct simulation *sim,struct light *in);
void light_set_dump(struct light *in,int dump);
void light_free(struct simulation *sim,struct light *in);
void light_dump(struct simulation *sim,struct light *in);
int light_solve_lam_slice(struct simulation *sim, struct light *in,int lam);
void light_set_dx(struct light *in,gdouble dx);
void light_dump_1d(struct simulation *sim,struct light *in, int i,char *ext);
void light_get_mode(struct istruct *mode,int lam,struct light *in);
int light_find_wavelength(struct light *in,gdouble lam);
void light_set_unity_laser_power(struct light *in,int lam);
void light_free_materials(struct light *in);
void light_free_epitaxy(struct light *in);
void light_load_epitaxy(struct simulation *sim,struct light *in,char *epi_file);
void light_calculate_complex_n(struct light *in);
int light_load_laser(struct simulation *sim, struct light *in,char *name);
gdouble light_get_sun(struct light *in);
void light_set_sun(struct light *in,gdouble Psun);
void light_set_model(struct light *in,char *model);
void light_dump_summary(struct simulation *sim,struct light *in);
void light_set_sun_delta_at_wavelength(struct light *in,long double lam);
void light_free_dlls(struct simulation *sim,struct light *in);
#endif
