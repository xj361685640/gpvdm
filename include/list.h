//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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



/** @file list.h
	@brief Header file for list.c
*/
#ifndef list_h
#define list_h

#include <sim_struct.h>

struct list
{
struct vec *list;
int max;
int length;
double cog_x;
double cog_y;
double max_y;
double min_y;
};

void list_load(struct simulation *sim,struct list* in,char *file_name);
int list_check(struct simulation *sim,struct list* in,struct vec *test);
void list_dump(struct simulation *sim,char *file_name,struct list* in);
void list_init(struct simulation *sim,struct list* in);
void list_add_no_rep(struct simulation *sim,struct list* in,struct vec *test);
void list_add(struct simulation *sim,struct list* in,double one, double two);
int list_get_length(struct simulation *sim,struct list* in);
void list_free(struct simulation *sim,struct list* in);
void list_dump_2d(struct simulation *sim,char *file_name,struct list* in);
void list_cog_cal(struct simulation *sim,struct list* in);
void list_minmax_cal(struct simulation *sim,struct list* in);
void list_remove_bump_up(struct simulation *sim,struct list* in,int start);
void list_remove_bump_down(struct simulation *sim,struct list* in,int start);
#endif

