//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file fxdomain.h
@brief header file for the fx domain plugin
*/


#ifndef fxdomain_h
#define fxdomain_h
#include <sim.h>

struct fxdomain
{
int fxdomain_sim_mode;
int fxdomain_points;
int fxdomain_n;
long double fxdomain_Vexternal;
long double fxdomain_voltage_modulation_max;
long double fxdomain_light_modulation_depth;
int fxdomain_do_fit;
char fxdomain_modulation_type[100];
long double fxdomain_L;
int periods_to_fit;
int fxdomain_modulation_rolloff_enable;
long double fxdomain_modulation_rolloff_start_fx;
long double fxdomain_modulation_rolloff_speed;

};

void sim_fxdomain(struct simulation *sim,struct device *in);
extern struct fxdomain fxdomain_config;
void fxdomain_load_config(struct simulation *sim,struct fxdomain *in,struct device *dev,char *config_file_name);
void newton_aux_fxdomain(struct device *in,long double V,long double* i,long double* didv,long double* didphi,long double* didxil,long double* didxipl,gdouble* didphir,gdouble* didxir,gdouble* didxipr);
long double newton_fxdomain(struct device *in,long double Vtot,int usecap);

void newton_aux_fxdomain_voc(struct device *in,long double V,long double* i,long double* didv,long double* didphi,long double* didxil,long double* didxipl,gdouble* didphir,gdouble* didxir,gdouble* didxipr);
long double fxdomain_newton_sim_voc_fast(struct simulation *sim,struct device *in,int do_LC);
long double fxdomain_newton_sim_voc(struct simulation *sim,struct device *in);
void fxdomain_set_light_for_voc(struct simulation *sim,struct device *in,long double Voc);

#endif




