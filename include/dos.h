//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//	roderick.mackenzie@nottingham.ac.uk
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


#ifndef dos_h
#define dos_h

#include <device.h>

struct dosconfig
{
char dos_name[20];
long double edge;
long double m;
int dostype;
long double Nt;
long double Et;
long double Nt2;
long double Et2;
long double Eshift;
long double nstart;
long double nstop;
long double base1;
long double base2;
int npoints;
int expan_len;
long double expan_N[20];
long double expan_E[20];
long double mu;
long double tau0;
long double tau1;
long double Tstart;
long double Tstop;
long double Ngaus;
int Tsteps;
int traps;
long double dband;
long double detrap;
int srh_bands;
long double srh_start;

long double srh_sigman;
long double srh_sigmap;
long double srh_vth;
long double Nc;
long double Nv;
long double srh_cut;

long double del_start;
long double del_stop;
long double Eg;
long double epsilonr;
long double doping_start;
long double doping_stop;
long double Xi;
long double gaus_mull;

long double pl_fe_fh;
long double pl_trap;
long double pl_recom;
int pl_enabled;

int Esteps;
long double B;
};

struct dos
{
int used;
long double *x;
int xlen;
int tlen;
int srh_bands;
long double *t;
long double *srh_E;
long double *srh_den;
long double **c;
long double **w;
long double ***srh_r1;
long double ***srh_r2;
long double ***srh_r3;
long double ***srh_r4;
long double ***srh_c;
struct dosconfig config;
};

void dos_init();
void dos_free();
long double get_dos_epsilonr(int mat);
long double get_dos_doping_start(int mat);
long double get_dos_doping_stop(int mat);
long double get_dos_E_n(int band,int mat);
long double get_dos_E_p(int band,int mat);
void dos_free_now(struct dos *mydos);

long double get_n_pop_srh(struct simulation *sim,long double top,long double T,int trap, int mat);
long double get_p_pop_srh(struct simulation *sim,long double top,long double T,int trap, int mat);
long double get_dn_pop_srh(struct simulation *sim,long double top,long double T,int trap, int mat);
long double get_dp_pop_srh(struct simulation *sim,long double top,long double T,int trap, int mat);

void load_dos(struct simulation *sim,struct device *dev,char *namen, char *namep,int mat);
long double get_dn_trap_den(long double top,long double T,int type,int band, int mat);
long double get_dp_trap_den(long double top,long double T,int type, int mat);
int search(long double *x,int N,long double find);
long double get_n_srh(struct simulation *sim,long double top,long double T,int trap,int r, int mat);
long double get_dn_srh(struct simulation *sim,long double top,long double T,int trap,int r, int mat);
long double get_p_srh(struct simulation *sim,long double top,long double T,int trap,int r, int mat);
long double get_dp_srh(struct simulation *sim,long double top,long double T,int trap,int r, int mat);
long double dos_get_band_energy_n(int band, int mat);
long double dos_get_band_energy_p(int band, int mat);
long double dos_srh_get_fermi_p(long double n, long double p,int band, int mat, long double T);
long double dos_srh_get_fermi_n(long double n, long double p,int band, int mat, long double T);
long double get_Nc_free(int mat);
long double get_Nv_free(int mat);
long double get_n_mu(int mat);
long double get_p_mu(int mat);
long double get_dos_Eg(int mat);
long double get_dos_Xi(int mat);


long double get_dos_E_n(int band,int mat);
long double get_dos_E_p(int band,int mat);
long double get_n_w(long double top,long double T,int mat);
long double get_p_w(long double top,long double T,int mat);
long double get_top_from_n(long double n,long double T,int mat);
long double get_top_from_p(long double p,long double T,int mat);
long double get_n_den(long double top,long double T, int mat);
long double get_p_den(long double top,long double T, int mat);
long double get_n_mu(int mat);
long double get_p_mu(int mat);
long double get_dn_den(long double top,long double T, int mat);
long double get_dp_den(long double top,long double T, int mat);
long double get_dpdT_den(long double top,long double T,int mat);
long double get_dndT_den(long double top,long double T,int mat);
long double get_dos_filled_n(struct device *in);
long double get_dos_filled_p(struct device *in);
void gen_dos_fd_gaus_n(struct simulation *sim,int mat);
void gen_dos_fd_gaus_p(struct simulation *sim,int mat);
int hashget(long double *x,int N,long double find);
void draw_gaus(struct device *in);

long double get_dos_filled_n(struct device *in);
long double get_dos_filled_p(struct device *in);
void safe_file(char *name);

long double get_pl_fe_fh(int mat);
long double get_pl_fe_te(int mat);
long double get_pl_te_fh(int mat);
long double get_pl_th_fe(int mat);
long double get_pl_ft_th(int mat);
int get_pl_enabled(int mat);

long double get_dos_B(int mat);

#endif
