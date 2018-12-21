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


/** @file list.c
	@brief Algorithms to make and deal with lists.
*/

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "vec.h"
#include "list.h"
#include <util.h>


static int unused __attribute__((unused));

void list_init(struct simulation *sim,struct list* in)
{
in->length=0;
in->max=10;
in->list = (struct vec *) malloc(in->max*sizeof(struct vec ));
}

void list_add_no_rep(struct simulation *sim,struct list* in,struct vec *test)
{
int i;
for (i=0;i<in->length;i++)
{
	if (vec_cmp(&(in->list[i]),test)==0)
	{
		return;
	}
}
list_add(sim,in, test->x, test->y);
}

void list_add(struct simulation *sim,struct list* in,double one, double two)
{
in->list[in->length].x=one;
in->list[in->length].y=two;
in->list[in->length].z=0.0;
in->length++;
if (in->length==in->max)
{
in->max+=10;
in->list = (struct vec *) realloc(in->list,in->max*sizeof(struct vec ));
}
}

int list_get_length(struct simulation *sim,struct list* in)
{
return in->length;
}

void list_free(struct simulation *sim,struct list* in)
{
free(in->list);
}

int list_check(struct simulation *sim,struct list* in,struct vec *test)
{
int i;
for (i=0;i<in->length;i++)
{
	if (vec_cmp(&(in->list[i]),test)==0)
	{
		return 0;
	}
}
return -1;
}

void list_minmax_cal(struct simulation *sim,struct list* in)
{
int i;
double min=in->list[0].y;
double max=in->list[0].y;
for (i=0;i<in->length;i++)
{
if (in->list[i].y<min) min=in->list[i].y;
if (in->list[i].y>max) max=in->list[i].y;
}

in->min_y=min;
in->max_y=max;
}

void list_remove_bump_down(struct simulation *sim,struct list* in,int start)
{
int i;
double ly=0.0;
double cy=0.0;
double ry=0.0;

for (i=start;i<in->length;i++)
{

if (i==start)
{
	ly=in->list[i].y;
}else
{
	ly=in->list[i-1].y;
}

if (i==in->length-1)
{
	ry=in->list[i].y;
}else
{
	ry=in->list[i+1].y;
}
cy=in->list[i].y;
if ((ly>cy)&&(ry>cy))
{
in->list[i].y=(ly+cy+ry)/3.0;
}

}

}

void list_remove_bump_up(struct simulation *sim,struct list* in,int start)
{
int i;
double ly=0.0;
double cy=0.0;
double ry=0.0;

for (i=start;i<in->length;i++)
{

if (i==start)
{
	ly=in->list[i].y;
}else
{
	ly=in->list[i-1].y;
}

if (i==in->length-1)
{
	ry=in->list[i].y;
}else
{
	ry=in->list[i+1].y;
}
cy=in->list[i].y;
if ((ly<cy)&&(ry<cy))
{
in->list[i].y=(ly+cy+ry)/3.0;
}

}

}

void list_dump(struct simulation *sim,char *file_name,struct list* in)
{
int i;
FILE *file;
file=fopen(file_name,"w");
fprintf(file,"#%d\n",in->length);
for (i=0;i<in->length;i++)
{
	fprintf(file,"%lf %lf %lf\n",in->list[i].x,in->list[i].y,in->list[i].z);
}
fclose(file);
}

void list_cog_cal(struct simulation *sim,struct list* in)
{
int i;
double cog_x=0.0;
double cog_y=0.0;
for (i=0;i<in->length;i++)
{
	cog_x+=in->list[i].x;
	cog_y+=in->list[i].y;
}
in->cog_x=cog_x/((double)in->length);
in->cog_y=cog_y/((double)in->length);
}


void list_dump_2d(struct simulation *sim,char *file_name,struct list* in)
{
int i;
FILE *file;
file=fopen(file_name,"w");
fprintf(file,"#%d\n",in->length);
for (i=0;i<in->length;i++)
{
	fprintf(file,"%lf %lf\n",in->list[i].x,in->list[i].y);
}
fclose(file);
}

void list_load(struct simulation *sim,struct list* in,char *file_name)
{
int i;
double a,b,c;
int length;
char temp[100];
FILE *file;
if ((file=fopen(file_name,"r"))==NULL)
{
	ewe(sim,"List file not found %s\n",file_name);
}

unused=fscanf(file,"%s",temp);
sscanf((temp+1),"%d",&(length));
for (i=0;i<length;i++)
{
	unused=fscanf(file,"%lf %lf %lf",&(a),&(b),&(c));
	list_add(sim,in, a, b);
}

fclose(file);
}
