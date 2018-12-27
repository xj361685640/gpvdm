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

/** @file eval.c
@brief evaluate math expresions for RPN
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

char* eval_sin(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;

	sscanf(a,"%le",&aa);
	//sscanf(b,"%le",&bb);
	sum=sin(aa);
	sprintf(out,"%le",sum);
}

char* eval_abs(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;

	sscanf(a,"%le",&aa);
	//sscanf(b,"%le",&bb);
	sum=fabs(aa);
	sprintf(out,"%le",sum);
}

char* eval_log10(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;

	sscanf(a,"%le",&aa);
	//sscanf(b,"%le",&bb);
	sum=log10(aa);
	sprintf(out,"%le",sum);
}

char* eval_pos(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;

	sscanf(a,"%le",&aa);
	//sscanf(b,"%le",&bb);
	if (aa<0)
	{
		sum=0.0;
	}else
	{
		sum=aa;
	}
	sprintf(out,"%le",sum);
}

char* eval_add(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;
	char ret[100];
	sscanf(a,"%le",&aa);
	sscanf(b,"%le",&bb);
	sum=aa+bb;
	sprintf(out,"%le",sum);
}

char* eval_sub(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;
	char ret[100];
	sscanf(a,"%le",&aa);
	sscanf(b,"%le",&bb);
	sum=aa-bb;
	sprintf(out,"%le",sum);
}


char* eval_mul(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;
	char ret[100];
	sscanf(a,"%le",&aa);
	sscanf(b,"%le",&bb);
	sum=aa*bb;
	sprintf(out,"%le",sum);
}

char* eval_bg(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;
	char ret[100];
	sscanf(a,"%le",&aa);
	sscanf(b,"%le",&bb);

	if (aa>bb)
	{
		sum=1.0;
	}

	sprintf(out,"%le",sum);
}

char* eval_sm(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;
	char ret[100];
	sscanf(a,"%le",&aa);
	sscanf(b,"%le",&bb);

	if (aa<bb)
	{
		sum=1.0;
	}

	sprintf(out,"%le",sum);
}

char* eval_pow(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;
	char ret[100];
	sscanf(a,"%le",&aa);
	sscanf(b,"%le",&bb);
	sum=pow(aa,bb);
	sprintf(out,"%le",sum);
}

char* eval_div(char *out,char* a,char* b)
{
	double aa=0.0;
	double bb=0.0;
	double sum=0.0;
	char ret[100];
	sscanf(a,"%le",&aa);
	sscanf(b,"%le",&bb);
	sum=aa/bb;
	sprintf(out,"%le",sum);
}



