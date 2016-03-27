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

#ifndef h_newton
#define h_newton

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <device.h>

void dllinternal_newton_set_min_ittr(int ittr);
void update_solver_vars(struct device *in, int clamp);
void fill_matrix(struct device *in);
gdouble get_cur_error(struct device *in);
gdouble get_abs_error(struct device *in);
void solver_cal_memory(struct device *in, int *ret_N, int *ret_M);
void dllinternal_solver_free_memory(struct device *in);
int dllinternal_solve_cur(struct device *in);
void dllinternal_solver_realloc(struct device *in);
#endif
