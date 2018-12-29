//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

/** @file buffer.h
	@brief Strcutr to hold .dat files before they are written to disk.
*/

#ifndef buffer_h
#define buffer_h
#include "advmath.h"
//#include <zip.h>
#include <device.h>

struct buffer
{
char title[100];
char type[100];
gdouble x_mul;
gdouble y_mul;
gdouble z_mul;
gdouble data_mul;
char x_label[100];
char y_label[100];
char z_label[100];
char data_label[100];
char x_units[100];
char y_units[100];
char z_units[100];
char data_units[100];
char section_one[100];
char section_two[100];
int logscale_x;
int logscale_y;
int logscale_z;
int logscale_data;
int write_to_zip;
int norm_x_axis;
int norm_y_axis;
long double data_max;
long double data_min;
int x;
int y;
int z;
gdouble time;
gdouble Vexternal;
char *buf;
int len;
int max_len;
char zip_file_name[400];
//struct zip *zip_file;
};

void buffer_zip_set_name(struct buffer *in,char * name);
void buffer_init(struct buffer *in);
void buffer_malloc(struct buffer *in);
void buffer_add_xy_data(struct simulation *sim,struct buffer *in,gdouble *x, gdouble *y, int len);
void buffer_add_string(struct buffer *in,char * string);
void buffer_add_info(struct simulation *sim,struct buffer *in);
void buffer_dump(struct simulation *sim,char * file,struct buffer *in);
void buffer_dump_path(struct simulation *sim,char *path,char * file,struct buffer *in);
void buffer_free(struct buffer *in);
void buffer_dump_aes(char *path,char * file,struct buffer *in,char *key_text);
void buffer_add_xy_data_z_label(struct buffer *in,gdouble *x, gdouble *y, gdouble *z, int len);
void buffer_dump_cache(struct simulation *sim,char * file,struct buffer *in);
void buffer_add_dir(struct simulation *sim,char * file_name);
void buffer_add_3d_device_data_including_boundaries(struct simulation *sim,struct buffer *buf,struct device *in,gdouble ***data,long double left,long double right);

#endif
