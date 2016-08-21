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




#ifndef cal_path_h
#define cal_path_h
#include <device.h>

void cal_path(struct simulation *sim);
char *get_plugins_path(struct simulation *sim);
char *get_lang_path(struct simulation *sim);
char *get_input_path(struct simulation *sim);
char *get_output_path(struct simulation *sim);
char *get_materials_path(struct simulation *sim);
void set_input_path(struct simulation *sim,char *in);
void set_output_path(struct simulation *sim,char *in);
int find_dll(struct simulation *sim, char *lib_path,char *lib_name);
#endif
