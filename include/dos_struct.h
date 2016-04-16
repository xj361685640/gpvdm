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


#ifndef dos_struct_h
#define dos_struct_h

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


#endif
