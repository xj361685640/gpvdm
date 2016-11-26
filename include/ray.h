//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

#ifndef ray_h
#define ray_h

#include <vec.h>

#define WAIT 0
#define READY 1
#define DONE 2

#define TRUE 1
#define FALSE 0

#define RAY_MAX 5000

struct plane
{
	struct vec xy0;
	struct vec xy1;
	int id;
	int edge;
};

struct ray
{
	struct vec xy;
	struct vec xy_end;
	struct vec dir;
	int state;
	int bounce;
	double mag;
};

struct image
{
	struct plane p[1000];
	struct ray rays[RAY_MAX];
	int lines;
	int nrays;
	int obj_mat_number[100];
	double obj_n[100];
	double obj_alpha[100];
	int objects;
	struct vec start_rays[100];
	int n_start_rays;
	int theta_steps;
	double y_escape_level;
};


void image_init(struct image *in);
int between(double v, double x0, double x1);
void add_plane(struct image *in, double x0,double y0,double x1,double y1,int id,int edge);
void ray_reset(struct image *in);
void add_ray(struct image *in,struct vec *start,struct vec *dir,double mag);
void dump_plane_to_file(char *file_name,struct image *in, int lam);
void dump_plane(struct image *in);
double get_rand();
void obj_norm(struct vec *ret,struct plane *my_obj);
int ray_intersect(struct vec *ret,struct plane *my_obj,struct ray *my_ray);
int search_ojb(struct image *in,int ray);
int activate_rays(struct image *in);
int pnpoly(struct image *in, struct vec *xy,int id);
void get_refractive(struct image *in,double *alpha,double *n0,double *n1,int ray);
int propergate_next_ray(struct image *in);
void add_box(struct image *in,double start_x,double start_y,double x_len,double y_len,int n,int sim_edge);
double get_eff(struct image *in);
#endif
