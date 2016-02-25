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

#ifndef dll_interface_h
#define dll_interface_h

#include "light.h"
#include "device.h"
#include "solver_interface.h"

struct dll_interface {
	void (*printf_log) (const char *format, ...);
	void (*waveprint) (char *, double);
	int (*get_dump_status) (int);
	void (*light_dump_1d) (struct light *, int, char *);
	void (*light_solve_optical_problem) (struct light *);
	void (*light_free_memory) (struct light *);
	void (*light_transfer_gen_rate_to_device) (struct device *,
						   struct light *);
	int (*complex_solver) (int col, int nz, int *Ti, int *Tj, double *Tx,
			       double *Txz, double *b, double *bz);
	 gdouble(*get_n_den) (gdouble top, gdouble T, int mat);
	 gdouble(*get_dn_den) (gdouble top, gdouble T, int mat);
	 gdouble(*get_n_w) (gdouble top, gdouble T, int mat);
	 gdouble(*get_p_den) (gdouble top, gdouble T, int mat);
	 gdouble(*get_dp_den) (gdouble top, gdouble T, int mat);
	 gdouble(*get_p_w) (gdouble top, gdouble T, int mat);
	 gdouble(*B) (gdouble x);
	 gdouble(*dB) (gdouble x);
	void (*dump_matrix) (int col, int nz, int *Ti, int *Tj, long double *Tx,
			     long double *b, char *index);
	int (*ewe) (const char *format, ...);
	void (*solver) (int col, int nz, int *Ti, int *Tj, long double *Tx,
			long double *b);
	 gdouble(*get_J) (struct device * in);
	 gdouble(*get_I) (struct device * in);
	FILE *(*fopena) (char *path, char *name, const char *mode);
	void (*dump_1d_slice) (struct device * in, char *out_dir);
	void (*update_arrays) (struct device * in);
};

void dll_interface_fixup();
struct dll_interface *dll_get_interface();

#endif
