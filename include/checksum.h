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

/** @file checksum.h
	@brief MD5 type checksums.
*/


#ifndef checksum_h
#define checksum_h
#include <stdint.h>
#include <sim_struct.h>

uint32_t leftrotate (uint32_t x, uint32_t c);
void checksum(char *out,char *data,int len);
void checksum_write(struct simulation *sim,char *file_name);
int checksum_check(struct simulation *sim,char *file_name);

#endif
