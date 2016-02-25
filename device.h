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

#ifndef device_h
#define device_h
#include "code_ctrl.h"
#include "light.h"
#include "epitaxy.h"
#include "advmath.h"

struct mesh {
	gdouble number;
	gdouble len;
	gdouble den;
};

struct device {
	struct epitaxy my_epitaxy;
	int remesh;
	int newmeshsize;
	gdouble Jnleft;
	gdouble Jnright;
	gdouble Jpleft;
	gdouble Jpright;
	gdouble *phi;
	gdouble *Nad;
	gdouble *G;
	gdouble *Gn;
	gdouble *Gp;
	gdouble *n;
	gdouble *p;
	gdouble *dn;
	gdouble *dndphi;
	gdouble *dp;
	gdouble *dpdphi;
	gdouble *Eg;
	gdouble *Xi;
	gdouble *Ev;
	gdouble *Ec;
	gdouble *Rfree;

	gdouble *mun;
	gdouble *mup;

	gdouble *Dn;
	gdouble *Dp;

	gdouble *epsilonr;

	gdouble *Fn;
	gdouble *Fp;
	gdouble *Nc;
	gdouble *Nv;
	gdouble *Tl;
	gdouble *Te;
	gdouble *Th;
	gdouble *ymesh;
	gdouble *R;
	gdouble *Fi;
	int *imat;
	gdouble *Jn;
	gdouble *Jp;

	gdouble *Jn_diffusion;
	gdouble *Jn_drift;

	gdouble *Jp_diffusion;
	gdouble *Jp_drift;

	gdouble Vapplied;
	int ymeshpoints;
	gdouble Vl;
	gdouble Vr;
	gdouble *x;
	gdouble *t;
	gdouble *xp;
	gdouble *tp;
	gdouble *kf;
	gdouble *kd;
	gdouble *kr;

	gdouble *Rn;
	gdouble *Rp;
	gdouble *kl;
	gdouble *ke;
	gdouble *kh;
	gdouble *Hl;
	gdouble *He;
	gdouble *Hh;
	gdouble *Habs;
	gdouble Psun;
	int excite_conv;
	int thermal_conv;
	int newton_enable_external_thermal;

	gdouble deltaFln;
	gdouble deltaFlp;
	gdouble deltaFrn;
	gdouble deltaFrp;

	gdouble *Rbi_k;

	gdouble *ex;
	gdouble *Dex;
	gdouble *Hex;

	gdouble *nf_save;
	gdouble *pf_save;
	gdouble *nt_save;
	gdouble *pt_save;

	gdouble *nfequlib;
	gdouble *pfequlib;
	gdouble *ntequlib;
	gdouble *ptequlib;

	gdouble **ntb_save;
	gdouble **ptb_save;

	gdouble *phi_save;

	gdouble xlen;
	gdouble ylen;
	gdouble zlen;

	int N;
	int M;
	int *Ti;		//row
	int *Tj;		//col
	long double *Tx;	//data
	long double *b;
	char **Tdebug;

	int lr_pcontact;
	int invert_applied_bias;

//plotting
	FILE *gnuplot;
	FILE *gnuplot_time;
	FILE *converge;
	FILE *tconverge;
//math
	int max_electrical_itt;
	gdouble electrical_clamp;
	int max_electrical_itt0;
	gdouble electrical_clamp0;
	gdouble electrical_error0;
	int math_enable_pos_solver;
	gdouble min_cur_error;
	gdouble Pmax_voltage;
	int pos_max_ittr;
	char solver_name[20];
	char newton_name[20];

//Device characterisation
	gdouble Voc;
	gdouble Jsc;
	gdouble FF;
	gdouble Pmax;
	gdouble Tll;
	gdouble Tlr;
	int Tliso;
	gdouble dt;
	int srh_sim;
	int go_time;
	gdouble time;
	gdouble *nlast;
	gdouble *plast;
	int ntrapnewton;
	int ptrapnewton;

	int stop;
	gdouble Rshort;
	struct mesh *meshdata;
	int ymeshlayers;
	int onlypos;
	int odes;
	gdouble last_error;
	gdouble posclamp;
	int srh_bands;
	gdouble *wn;
	gdouble *wp;

//n traps
	gdouble *nt_all;
	gdouble **nt;
	gdouble **ntlast;
	gdouble **dnt;
	gdouble **srh_n_r1;
	gdouble **srh_n_r2;
	gdouble **srh_n_r3;
	gdouble **srh_n_r4;
	gdouble **dsrh_n_r1;
	gdouble **dsrh_n_r2;
	gdouble **dsrh_n_r3;
	gdouble **dsrh_n_r4;
	gdouble **Fnt;
	gdouble **xt;
	gdouble *tt;

	gdouble **nt_r1;
	gdouble **nt_r2;
	gdouble **nt_r3;
	gdouble **nt_r4;
//p traps
	gdouble *pt_all;
	gdouble **pt;
	gdouble **ptlast;
	gdouble **dpt;
	gdouble **srh_p_r1;
	gdouble **srh_p_r2;
	gdouble **srh_p_r3;
	gdouble **srh_p_r4;
	gdouble **dsrh_p_r1;
	gdouble **dsrh_p_r2;
	gdouble **dsrh_p_r3;
	gdouble **dsrh_p_r4;
	gdouble **Fpt;
	gdouble **xpt;
	gdouble *tpt;

	gdouble **pt_r1;
	gdouble **pt_r2;
	gdouble **pt_r3;
	gdouble **pt_r4;

	gdouble A;
	gdouble Vol;

	gdouble Rshunt;
	gdouble Rcontact;

	int lr_bias;

	int *dostype;

	int interfaceleft;
	int interfaceright;
	gdouble phibleft;
	gdouble phibright;
	gdouble vl_e;
	gdouble vl_h;
	gdouble vr_e;
	gdouble vr_h;
	int stop_start;
	gdouble externalv;
	gdouble Vapplied_last;
	gdouble Ilast;
	int timedumpcount;
	char outputpath[200];
	char inputpath[200];
	char simmode[200];
	gdouble area;

	gdouble *nrelax;
	gdouble *ntrap_to_p;
	gdouble *prelax;
	gdouble *ptrap_to_n;

	gdouble lcharge;
	gdouble rcharge;

	gdouble l_electrons;
	gdouble l_holes;
	gdouble r_electrons;
	gdouble r_holes;

	int dumpitdos;

	gdouble t_big_offset;

	gdouble C;
	gdouble other_layers;
	int last_ittr;

	int kl_in_newton;
	int config_kl_in_newton;
	void (*newton_aux) (struct device *, gdouble, gdouble *, gdouble *,
			    gdouble *, gdouble *, gdouble *, gdouble *,
			    gdouble *, gdouble *);
	gdouble *B;
	gdouble xnl_left;
	gdouble xpl_left;
	int stoppoint;
	gdouble ilast;

	int newton_clever_exit;
	char plot_file[100];

	gdouble start_stop_time;

	gdouble Is;
	gdouble n_id;
	gdouble Igen;
	struct light mylight;
	int dump_movie;

	gdouble *n_orig;
	gdouble *p_orig;
	gdouble *n_orig_f;
	gdouble *p_orig_f;
	gdouble *n_orig_t;
	gdouble *p_orig_t;
	int nofluxl;

	gdouble Vbi;
	int newton_min_itt;
	gdouble vbi;
	gdouble avg_gen;
	int dump_slicepos;
	gdouble pl_intensity;

	gdouble Rext;
	gdouble Cext;
	gdouble VCext_last;
	gdouble VCext;
	int newton_last_ittr;
	gdouble phi_mul;
	long double layer_start[100];
	long double layer_stop[100];
	long double layer_width[100];
};
#endif
