//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.rodmack.com
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

/** @file frame.h
@brief used to build 2D pictures not used much any more due to python front end
*/

#ifndef frame_h
#define frame_h
struct map
{
double **data;
int xpoints;
int ypoints;
double xstart;
double ystart;
double xstop;
double ystop;
double xdelta;
double ydelta;
double *x;
double *y;
double *x_fake;
double *y_fake;
double cog_x;
double cog_y;
int count;
};

void frame_reset(struct map *in);
void frame_free(struct map *in);
void frame_init(struct map *in,int xpoints,int ypoints,double xstart,double xstop,double ystart,double ystop);
void frame_data_set(struct map *in,double x,double y,double data);
void frame_data_add(struct map *in,double x,double y,double data);
void frame_data_set_if_bigger(struct map *in,double x,double y,double data);
void frame_dump(char *file,struct map *in);
void frame_scale_delog_scale(struct map *in);
void frame_dump_outline(char *file,struct map *in);
void frame_cog_cal(struct map *in);
void frame_fill(struct map *in);
#endif
