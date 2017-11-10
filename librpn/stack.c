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

void output_push(struct simulation *sim,struct rpn *in,char *val)
{
	strcpy(in->output[in->output_pos++],val);
}

void stack_push(struct simulation *sim,struct rpn *in,char *val)
{
	//printf("stack push %d %s\n",in->stack_pos,val);
	strcpy(in->stack[in->stack_pos++],val);
}

char* stack_pop(struct simulation *sim,struct rpn *in)
{
	//printf("stack pop %d %s\n",in->stack_pos,in->stack[in->stack_pos-1]);
	if (in->stack_pos==0)
	{
		strcpy(in->stack[0],"");
		return in->stack[0];
	}
	return in->stack[--in->stack_pos];
}

char* stack_peak(struct simulation *sim,struct rpn *in)
{
	if (in->stack_pos!=0)
	{
		return in->stack[in->stack_pos-1];
	}else
	{
		return "";
	}
}

void print_stack(struct simulation *sim,struct rpn *in)
{
	int i;
	printf("stack:\n");
	for (i=0;i<in->stack_pos;i++)
	{
		printf(">%s<\n",in->stack[i]);
	}
}
void print_output(struct simulation *sim,struct rpn *in)
{
	int i=0;
	printf("output:\n");
	for (i=0;i<in->output_pos;i++)
	{
		printf(">%s<\n",in->output[i]);
	}
}



