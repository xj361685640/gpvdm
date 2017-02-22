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



#ifndef buffer_h
#define buffer_h
#include "advmath.h"
//#include <zip.h>

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
void buffer_add_xy_data(struct buffer *in,gdouble *x, gdouble *y, int len);
void buffer_add_string(struct buffer *in,char * string);
void buffer_add_info(struct buffer *in);
void buffer_dump(struct simulation *sim,char * file,struct buffer *in);
void buffer_dump_path(struct simulation *sim,char *path,char * file,struct buffer *in);
void buffer_free(struct buffer *in);
void buffer_dump_aes(char *path,char * file,struct buffer *in,char *key_text);
void buffer_add_xy_data_z_label(struct buffer *in,gdouble *x, gdouble *y, gdouble *z, int len);
#endif
