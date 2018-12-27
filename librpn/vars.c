//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file vars.c
@brief RPN varables.
*/


#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "util.h"
#include "cal_path.h"
#include "const.h"
#include <rpn.h>
#include <log.h>

void rpn_add_var(struct simulation *sim,struct rpn *in,char *name,double value)
{
	strcpy(in->vars[in->vars_pos].name,name);
	in->vars[in->vars_pos].value=value;
	in->vars_pos++;
}

int rpn_is_var(struct simulation *sim,struct rpn *in,char *out,char *name)
{
	int i;
	for (i=0;i<in->vars_pos;i++)
	{
		if (strcmp(name,in->vars[i].name)==0)
		{
			if (out!=NULL)
			{
				sprintf(out,"%le",in->vars[i].value);
			}
			return 0;
		}
	}
return -1;
}

