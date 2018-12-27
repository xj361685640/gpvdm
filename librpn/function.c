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

/** @file function.c
@brief configure RPN functions
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

void add_function(struct simulation *sim,struct rpn *in,char *name,void *f)
{
	strcpy(in->functions[in->functions_count].name,name);
	in->functions[in->functions_count].f=f;
	in->functions_count++;
}

int is_function(struct simulation *sim,struct rpn *in,char *val)
{
	int i;
	for (i=0;i<in->functions_count;i++)
	{
		if (strcmp(val,in->functions[i].name)==0)
		{
			return i;
		}
	}
return -1;
}

char* function_run(struct simulation *sim,struct rpn *in,char *val,char *out,char* a,char* b)
{
	int i;
	char *ret;
	for (i=0;i<in->functions_count;i++)
	{
		if (strcmp(val,in->functions[i].name)==0)
		{
			(in->functions[i].f)(out,a,b);
			return out;
		}
	}
return "error";
}



