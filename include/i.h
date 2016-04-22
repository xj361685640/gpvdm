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


/** @file i.h
	@brief Header file for i.c
*/
#ifndef i_h
#define i_h
#include "advmath.h"

struct istruct
{
gdouble *x;
gdouble *data;

int len;
int max_len;
char name[200];
};

int inter_get_col_n(char *name);
void inter_add_to_hist(struct istruct* in,gdouble pos,gdouble value);
void inter_init_mesh(struct istruct* in,int len,gdouble min,gdouble max);
void inter_smooth_range(struct istruct* out,struct istruct* in,int points,gdouble x);
gdouble inter_avg_range(struct istruct* in,gdouble start,gdouble stop);
gdouble inter_array_get_max(gdouble *data,int len);
void inter_div(struct istruct* one,struct istruct* two);
void inter_div_gdouble(struct istruct* in,gdouble div);
gdouble inter_get_min_range(struct istruct* in,gdouble min, gdouble max);
void inter_make_cumulative(struct istruct* in);
void inter_y_mul_dx(struct istruct* in);
void inter_add_x(struct istruct* in,gdouble value);
int inter_sort(struct istruct* in);
gdouble inter_get_quartile(struct istruct* in,gdouble value);
void inter_save_seg(struct istruct* in,char *path,char *name,int seg);
gdouble inter_intergrate(struct istruct* in);
void inter_to_log_mesh(struct istruct* out,struct istruct* in);
void inter_smooth(struct istruct* out,struct istruct* in,int points);
gdouble inter_sum_mod(struct istruct* in);
void inter_set_value(struct istruct* in,gdouble value);
gdouble inter_get_neg(struct istruct* in,gdouble x);
gdouble inter_get_noend(struct istruct* in,gdouble x);
void inter_new(struct istruct* in,int len);
void inter_to_new_mesh(struct istruct* in,struct istruct* out);
void inter_swap(struct istruct* in);
void inter_log_y_m(struct istruct* in);
gdouble inter_get_min(struct istruct* in);
gdouble inter_get_fabs_max(struct istruct* in);
gdouble inter_norm_to_one_range(struct istruct* in,gdouble min,gdouble max);
void inter_chop(struct istruct* in,gdouble min, gdouble max);
void inter_save_a(struct istruct* in,char *path,char *name);
void inter_dump(struct istruct* in);
void inter_purge_zero(struct istruct* in);
void inter_append(struct istruct* in,gdouble x,gdouble y);
void inter_init(struct istruct* in);
void inter_sub_gdouble(struct istruct* in,gdouble value);
void inter_sub(struct istruct* one,struct istruct* two);
gdouble inter_sum(struct istruct* in);
void inter_copy(struct istruct* in,struct istruct* orig,int alloc);
int inter_get_col(char *file);
void inter_load_by_col(struct istruct* in,char *name,int col);
gdouble inter_get_diff(char *out_path,struct istruct* one,struct istruct* two,gdouble start,gdouble stop,struct istruct* mull);
void inter_pow(struct istruct* in,gdouble p);
gdouble inter_get_raw(gdouble *x,gdouble *data,int len,gdouble pos);
gdouble inter_norm(struct istruct* in,gdouble mul);
void inter_log_y(struct istruct* in);
void inter_mul(struct istruct* in,gdouble mul);
void inter_log_x(struct istruct* in);
void inter_save(struct istruct* in,char *name);
void inter_load(struct istruct* in,char *name);
gdouble inter_get_hard(struct istruct* in,gdouble x);
gdouble inter_get(struct istruct* in,gdouble x);
void inter_print(struct istruct* in);
void inter_free(struct istruct* in);
void inter_rescale(struct istruct* in,gdouble xmul, gdouble ymul);
void inter_mod(struct istruct* in);
void inter_add(struct istruct* out,struct istruct* in);
void inter_norm_area(struct istruct* in,gdouble mul);
gdouble inter_get_max(struct istruct* in);
gdouble inter_get_max_range(struct istruct* in,int start, int stop);
void inter_add_gdouble(struct istruct* in,gdouble value);
gdouble inter_intergrate_lim(struct istruct* in,gdouble from, gdouble to);
void inter_deriv(struct istruct* out,struct istruct* in);
void inter_import_array(struct istruct* in,gdouble *x,gdouble *y,int len,int alloc);
gdouble inter_avg(struct istruct* in);
void inter_convolve(struct istruct* one,struct istruct* two);
void inter_save_backup(struct istruct* in,char *name,int backup);
void inter_dft(gdouble *real,gdouble *imag,struct istruct* in,gdouble fx);
int inter_get_max_pos(struct istruct* in);
int inter_search_pos(struct istruct* in,gdouble x);
void inter_join_bins(struct istruct* in,gdouble delta);
void inter_reset(struct istruct* in);
void inter_find_peaks(struct istruct* out,struct istruct* in,int find_max);
void inter_sin(struct istruct *in,gdouble mag,gdouble fx,gdouble delta);
#endif
