//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include "const.h"
#include "inp.h"

struct inp_file hard_limit_inp;

void hard_limit_init()
{
	inp_init(&hard_limit_inp);
	inp_load(&hard_limit_inp, "hard_limit.inp");
}

void hard_limit_free()
{
	inp_free(&hard_limit_inp);
}

void hard_limit(char *token, gdouble * value)
{
	char token0[1000];
	gdouble ret = *value;
	gdouble min = 0.0;
	gdouble max = 0.0;
	char *text = inp_search_part(&hard_limit_inp, token);

	if (text != NULL) {
		sscanf(text, "%s %Lf %Lf", token0, &max, &min);

		if (strcmp(token0, token) == 0) {
			if (ret > max) {
				ret = max;
			}

			if (ret < min) {
				ret = min;
			}
		}
	}
	*value = ret;
}
