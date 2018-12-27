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

/** @file debug.c
@brief some debuging code
*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "util.h"
#include <stdarg.h>

static int debug_enabled=0;

void set_debug(int value)
{
	debug_enabled=value;
}

void debug_printf( const char *format, ...)
{

	if (debug_enabled==TRUE)
	{
		char temp[1000];
		//char temp2[1000];
		va_list args;
		va_start(args, format);
		vsprintf(temp,format, args);


			printf("%s\n",temp);

		va_end(args);
	}

}

