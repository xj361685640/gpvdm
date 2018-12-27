//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

/** @file dos.h
	@brief Headers for reading and getting values from the DoS.
*/


#ifndef dos_h
#define dos_h

#include <device.h>
#include <dos_struct.h>

void dos_init(struct device *in,int mat);
void dos_free(struct device *in,int mat);
long double get_dos_epsilonr(struct device *in,int mat);
long double get_dos_doping_start(struct device *in,int mat);
long double get_dos_doping_stop(struct device *in,int mat);
void dos_free_now(struct dos *mydos);

long double get_n_pop_srh(struct simulation *sim,struct device *in,long double top,long double T,int trap, int mat);
long double get_p_pop_srh(struct simulation *sim,struct device *in,long double top,long double T,int trap, int mat);
long double get_dn_pop_srh(struct simulation *sim,struct device *in,long double top,long double T,int trap, int mat);
long double get_dp_pop_srh(struct simulation *sim,struct device *in,long double top,long double T,int trap, int mat);

void load_dos(struct simulation *sim,struct device *dev,char *namen, char *namep,int mat);
long double get_dn_trap_den(long double top,long double T,int type,int band, int mat);
long double get_dp_trap_den(long double top,long double T,int type, int mat);
int search(long double *x,int N,long double find);
long double get_n_srh(struct simulation *sim,struct device *in,long double top,long double T,int trap,int r, int mat);
long double get_dn_srh(struct simulation *sim,struct device *in,long double top,long double T,int trap,int r, int mat);
long double get_p_srh(struct simulation *sim,struct device *in,long double top,long double T,int trap,int r, int mat);
long double get_dp_srh(struct simulation *sim,struct device *in,long double top,long double T,int trap,int r, int mat);
long double dos_get_band_energy_n(struct device *in,int band, int mat);
long double dos_get_band_energy_p(struct device *in,int band, int mat);
long double dos_srh_get_fermi_p(struct device *in,long double n, long double p,int band, int mat, long double T);
long double dos_srh_get_fermi_n(struct device *in,long double n, long double p,int band, int mat, long double T);
long double get_Nc_free(struct device *in,int mat);
long double get_Nv_free(struct device *in,int mat);
long double get_n_mu(struct device *in,int mat);
long double get_p_mu(struct device *in,int mat);
long double get_dos_Eg(struct device *in,int mat);
long double get_dos_Xi(struct device *in,int mat);


long double get_dos_E_n(struct device *in,int band,int mat);
long double get_dos_E_p(struct device *in,int band,int mat);
long double get_n_w(struct device *in,long double top,long double T,int mat);
long double get_p_w(struct device *in,long double top,long double T,int mat);
long double get_top_from_n(struct device *in,long double n,long double T,int mat);
long double get_top_from_p(struct device *in,long double p,long double T,int mat);
long double get_n_den(struct device *in,long double top,long double T, int mat);
long double get_p_den(struct device *in,long double top,long double T, int mat);
long double get_n_mu(struct device *in,int mat);
long double get_p_mu(struct device *in,int mat);
long double get_dn_den(struct device *in,long double top,long double T, int mat);
long double get_dp_den(struct device *in,long double top,long double T, int mat);
long double get_dpdT_den(struct device *in,long double top,long double T,int mat);
long double get_dndT_den(struct device *in,long double top,long double T,int mat);
long double get_dos_filled_n(struct device *in);
long double get_dos_filled_p(struct device *in);
void gen_dos_fd_gaus_n(struct simulation *sim,int mat);
void gen_dos_fd_gaus_p(struct simulation *sim,int mat);
int hashget(long double *x,int N,long double find);
void draw_gaus(struct device *in);

long double get_dos_filled_n(struct device *in);
long double get_dos_filled_p(struct device *in);
void safe_file(char *name);

long double get_pl_fe_fh(struct device *in,int mat);
long double get_pl_fe_te(struct device *in,int mat);
long double get_pl_te_fh(struct device *in,int mat);
long double get_pl_th_fe(struct device *in,int mat);
long double get_pl_ft_th(struct device *in,int mat);
int get_pl_enabled(struct device *in,int mat);

long double get_dos_B(struct device *in,int mat);

#endif
