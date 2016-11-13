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



/** @file vec.h
	@brief Header file for vec.c
*/
#ifndef vech
#define vech
///Structure to hold vector
struct vec
{
double x;
double y;
double z;
};

//Vector routines

double vec_mod(struct vec *my_vec);
void vec_plus(struct vec *my_vec1);
void vec_fprintf(FILE *out,struct vec *my_vec);
double deg(double in);
double vec_ang(struct vec *in_v0,struct vec *in_v1);
void vec_sub(struct vec *my_vec1,struct vec *my_vec2);
void vec_add(struct vec *my_vec1,struct vec *my_vec2);
void vec_div(struct vec *my_vec1,double n);
void vec_mul(struct vec *my_vec1,double n);
double vec_fabs(struct vec *my_vec);
void vec_rotate(struct vec *my_vec,double ang);
double vec_dot(struct vec *a,struct vec *b);
double vec_dist(struct vec *a,struct vec *b);
void vec_init(struct vec *my_vec);
void vec_cross(struct vec *ret,struct vec *a,struct vec *b);
void vec_swap(struct vec *my_vec);
void vec_cpy(struct vec *my_vec1,struct vec *my_vec2);
void vec_norm(struct vec *my_vec);
void vec_print(struct vec *my_vec);
void vec_set(struct vec *my_vec,double x, double y, double z);
double overlap(double x0,double x1);
double vec_overlap(struct vec *a,struct vec *b);
double vec_get_dihedral(struct vec *a,struct vec *b,struct vec *c,struct vec *d);
void vec_rot(struct vec *in,struct vec *unit,struct vec *base, double ang);
void vec_add_values(struct vec *my_vec, double x,double y, double z);
int vec_cmp(struct vec *my_vec1,struct vec *my_vec2);
void rotx_vec(struct vec *out, struct vec *in,double a);
void roty_vec(struct vec *out, struct vec *in,double a);
void rotz_vec(struct vec *out, struct vec *in,double a);
double ang_vec(struct vec *one,struct vec *two);
#endif
