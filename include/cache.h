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

/** @file cache.h
	@brief A cache to prevent duming from disk during the simulation.
*/

#ifndef cache_h
#define cache_h
#include "device.h"
#include "cache_struct.h"

#define CACHE_DIR -1

int cache_search(struct simulation *sim,char * file_name);
void cache_init(struct simulation *sim);
void cache_dump(struct simulation *sim);
void cache_free(struct simulation *sim);
void cache_add_item(struct simulation *sim,char * file_name,char *data,int len);
int cache_search(struct simulation *sim,char * file_name);
void cache_add_dir(struct simulation *sim,char * file_name);

#endif
