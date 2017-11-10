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
#include <ctype.h>

#include "util.h"
#include "cal_path.h"
#include "const.h"
#include <rpn.h>
#include <log.h>

int isnumber(char a)
{
int i;
int ret=TRUE;
int digit=isdigit(a);

	if ((digit!=0)||(a=='.'))
	{
		return 0;
	}

return -1;

}

int edge_detect(struct simulation *sim,struct rpn *in,char *buf,char next)
{
	int last=strlen(buf);
	if (last==0)
	{
		return -1;
	}
	
	last--;

	if ((isnumber(buf[last])==0) && (next=='e'))
	{
		return -1;
	}

	if (last>=1)
	{
		if ((isnumber(buf[last-1])==0) && (buf[last]=='e') && ((next=='+') || (next=='-') || (isnumber(next)==0)) )
		{
			return -1;
		}
	}

	if (last>=2)
	{
		if ((isnumber(buf[last-2])==0) && (buf[last-1]=='e') && ((buf[last]=='+') || (buf[last]=='-')) && (isnumber(next)==0))
		{
			return -1;
		}
	}

	if ((isnumber(buf[last])==0) && (isnumber(next)!=0))
	{
		return 0;
	}

	if ((isnumber(buf[last])!=0) && (isnumber(next)==0))
	{
		return 0;
	}

	if ((next=='(') || (next==')'))
	{
		return 0;
	}

	if (strcmp(buf,")")==0)
	{
		return 0;
	}

	if (strcmp(buf,"(")==0)
	{
		return 0;
	}

	if (strcmp(buf,"*")==0)
	{
		return 0;
	}

	int i;
	for (i=0;i<in->opp_count;i++)
	{
		if ((strcmp(buf,in->opps[i].name)==0)||(in->opps[i].name[0]==next))
		{

			return 0;
		}
	}
/*
	if ((next=='>')||strcmp(buf,">")==0)
	{
		return 0;
	}

	if ((next=='>')||strcmp(buf,">")==0)
	{
		return 0;
	}

	if ((next=='-')||strcmp(buf,"-")==0)
	{
		return 0;
	}
	
	if ((next=='+')||strcmp(buf,"+")==0)
	{
		return 0;
	}
	
	if ((next=='/')||strcmp(buf,"/")==0)
	{
		return 0;
	}
*/
	if (strcmp(buf,"^")==0)
	{
		return 0;
	}
	return -1;
}



