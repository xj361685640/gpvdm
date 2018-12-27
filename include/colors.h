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

/** @file colors.h
	@brief Define terminal colors.
*/

#ifndef colors_h
#define colors_h

#ifdef windows


#define fg_reset	15
#define fg_wight	7
#define fg_red		12
#define fg_green	10
#define fg_yellow	14
#define fg_blue		9
#define fg_purple	13

#else
#define fg_reset	0
#define fg_wight	97
#define fg_red		31
#define fg_green	32
#define fg_yellow	33
#define fg_blue		34
#define fg_purple	35

#endif

#endif
