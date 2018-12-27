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

/** @file log.h
@brief write log files to disk
*/


#ifndef _log
#define _log
#include <sim_struct.h>

#define log_level_none 0
#define log_level_screen 1
#define log_level_disk 2
#define log_level_screen_and_disk 3

void textcolor(struct simulation *sim,int color);
void set_logging_level(struct simulation *sim,int value);
void log_clear(struct simulation *sim);
void printf_log(struct simulation *sim, const char *format, ...);
void waveprint(struct simulation *sim, char *in,double wavelength);
void log_time_stamp(struct simulation *sim);
int log_search_error(char *path);
void log_write_file_access(struct simulation *sim,char * file,char mode);
void log_tell_use_where_file_access_log_is(struct simulation *sim);
#endif
