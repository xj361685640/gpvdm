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

struct dosconfig
{
char dos_name[20];
gdouble edge;
gdouble m;
int dostype;
gdouble Nt;
gdouble Et;
gdouble Nt2;
gdouble Et2;
gdouble Eshift;
gdouble nstart;
gdouble nstop;
gdouble base1;
gdouble base2;
int npoints;
int expan_len;
gdouble expan_N[20];
gdouble expan_E[20];
gdouble mu;
gdouble tau0;
gdouble tau1;
gdouble Tstart;
gdouble Tstop;
gdouble Ngaus;
int Tsteps;
int traps;
gdouble dband;
gdouble detrap;
int srh_bands;
gdouble srh_start;

gdouble srh_sigman;
gdouble srh_sigmap;
gdouble srh_vth;
gdouble Nc;
gdouble Nv;
gdouble srh_cut;

gdouble del_start;
gdouble del_stop;
gdouble Eg;
gdouble epsilonr;
gdouble doping_start;
gdouble doping_stop;
gdouble Xi;
gdouble gaus_mull;

gdouble pl_fe_fh;
gdouble pl_trap;
gdouble pl_recom;
int pl_enabled;

int Esteps;
gdouble B;
};

struct dos
{
int used;
gdouble *x;
int xlen;
int tlen;
int srh_bands;
gdouble *t;
gdouble *srh_E;
gdouble *srh_den;
gdouble **c;
gdouble **w;
gdouble ***srh_r1;
gdouble ***srh_r2;
gdouble ***srh_r3;
gdouble ***srh_r4;
gdouble ***srh_c;
struct dosconfig config;
};

void dos_init();
void dos_free();
gdouble get_dos_epsilonr(int mat);
gdouble get_dos_doping_start(int mat);
gdouble get_dos_doping_stop(int mat);
gdouble get_dos_E_n(int band,int mat);
gdouble get_dos_E_p(int band,int mat);
void dos_free_now(struct dos *mydos);

gdouble get_n_pop_srh(gdouble top,gdouble T,int trap, int mat);
gdouble get_p_pop_srh(gdouble top,gdouble T,int trap, int mat);
gdouble get_dn_pop_srh(gdouble top,gdouble T,int trap, int mat);
gdouble get_dp_pop_srh(gdouble top,gdouble T,int trap, int mat);

void load_dos(struct device *dev,char *namen, char *namep,int mat);
gdouble get_dn_trap_den(gdouble top,gdouble T,int type,int band, int mat);
gdouble get_dp_trap_den(gdouble top,gdouble T,int type, int mat);
int search(gdouble *x,int N,gdouble find);
gdouble get_n_srh(gdouble top,gdouble T,int trap,int r, int mat);
gdouble get_dn_srh(gdouble top,gdouble T,int trap,int r, int mat);
gdouble get_p_srh(gdouble top,gdouble T,int trap,int r, int mat);
gdouble get_dp_srh(gdouble top,gdouble T,int trap,int r, int mat);
gdouble dos_get_band_energy_n(int band, int mat);
gdouble dos_get_band_energy_p(int band, int mat);
gdouble dos_srh_get_fermi_p(gdouble n, gdouble p,int band, int mat, gdouble T);
gdouble dos_srh_get_fermi_n(gdouble n, gdouble p,int band, int mat, gdouble T);
gdouble get_Nc_free(int mat);
gdouble get_Nv_free(int mat);
gdouble get_n_mu(int mat);
gdouble get_p_mu(int mat);
gdouble get_dos_Eg(int mat);
gdouble get_dos_Xi(int mat);


gdouble get_dos_E_n(int band,int mat);
gdouble get_dos_E_p(int band,int mat);
gdouble get_n_w(gdouble top,gdouble T,int mat);
gdouble get_p_w(gdouble top,gdouble T,int mat);
gdouble get_top_from_n(gdouble n,gdouble T,int mat);
gdouble get_top_from_p(gdouble p,gdouble T,int mat);
gdouble get_n_den(gdouble top,gdouble T, int mat);
gdouble get_p_den(gdouble top,gdouble T, int mat);
gdouble get_n_mu(int mat);
gdouble get_p_mu(int mat);
gdouble get_dn_den(gdouble top,gdouble T, int mat);
gdouble get_dp_den(gdouble top,gdouble T, int mat);
gdouble get_dpdT_den(gdouble top,gdouble T,int mat);
gdouble get_dndT_den(gdouble top,gdouble T,int mat);
gdouble get_dos_filled_n(struct device *in);
gdouble get_dos_filled_p(struct device *in);
void gen_dos_fd_gaus_n();
void gen_dos_fd_gaus_p();
int hashget(gdouble *x,int N,gdouble find);
void draw_gaus(struct device *in);

gdouble get_dos_filled_n(struct device *in);
gdouble get_dos_filled_p(struct device *in);
void safe_file(char *name);

gdouble get_pl_fe_fh(int mat);
gdouble get_pl_fe_te(int mat);
gdouble get_pl_te_fh(int mat);
gdouble get_pl_th_fe(int mat);
gdouble get_pl_ft_th(int mat);
int get_pl_enabled(int mat);

gdouble get_dos_B(int mat);

#endif
