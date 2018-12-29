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

/** @file buffer.c
@brief used to save output files to disk with a nice header, so the user knows what was writtne to them
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "buffer.h"
#include "const.h"
#include "code_ctrl.h"
#include "cal_path.h"
#include "dump.h"
#include <log.h>
#include <cache.h>

void buffer_zip_set_name(struct buffer *in,char * name)
{
	strcpy(in->zip_file_name,name);
	in->write_to_zip=TRUE;
}

void buffer_init(struct buffer *in)
{
in->write_to_zip=FALSE;
in->norm_x_axis=FALSE;
in->norm_y_axis=FALSE;
in->data_max=NAN;
in->data_min=NAN;
}

void buffer_malloc(struct buffer *in)
{
in->len=0;
in->max_len=0;
in->max_len+=1024;
strcpy(in->title,"");
strcpy(in->type,"xy");
strcpy(in->title,"");
in->x_mul=1.0;
in->y_mul=1.0;
in->z_mul=1.0;
in->data_mul=1.0;
strcpy(in->x_label,"");
strcpy(in->y_label,"");
strcpy(in->z_label,"");
strcpy(in->data_label,"");
strcpy(in->x_units,"");
strcpy(in->y_units,"");
strcpy(in->z_units,"");
strcpy(in->data_units,"");
strcpy(in->section_one,"");
strcpy(in->section_two,"");
in->logscale_x=0;
in->logscale_y=0;
in->logscale_z=0;
in->logscale_data=0;
in->x=0;
in->y=0;
in->z=0;
in->time= -1.0;
in->Vexternal= -1.0;
in->buf=(char*)malloc(sizeof(char)*in->max_len);
memset(in->buf, 0, in->max_len);
}

void buffer_add_xy_data(struct simulation *sim,struct buffer *in,gdouble *x, gdouble *y, int len)
{
int i;
char string[200];
gdouble x_out=0.0;
gdouble y_out=0.0;
gdouble max=0.0;
gdouble min=0.0;

if (get_dump_status(sim,dump_write_headers)==TRUE)
{
	sprintf(string,"#data\n");
	buffer_add_string(in,string);
}

if (len>0)
{
	max=y[0];
	min=y[0];

	for (i=0;i<len;i++)
	{
		if (y[i]<min) min=y[i];
		if (y[i]>max) max=y[i];
	}


	for (i=0;i<len;i++)
	{
		x_out=x[i];
		y_out=y[i];
		if (in->norm_x_axis==TRUE)
		{
			x_out/=x[len-1];
		}

		if (in->norm_y_axis==TRUE)
		{
			y_out=(y[i]-min)/(max-min);
		}

		sprintf(string,"%Le %Le\n",x_out,y_out);
		buffer_add_string(in,string);
	}
}

if (get_dump_status(sim,dump_write_headers)==TRUE)
{
	sprintf(string,"#end\n");
	buffer_add_string(in,string);
}

}


void buffer_add_xy_data_z_label(struct buffer *in,gdouble *x, gdouble *y, gdouble *z, int len)
{
int i;
char string[200];
gdouble x_out=0.0;
gdouble y_out=0.0;
gdouble max=0.0;
gdouble min=0.0;

sprintf(string,"#data\n");
buffer_add_string(in,string);


if (len>0)
{
	max=y[0];
	min=y[0];

	for (i=0;i<len;i++)
	{
		if (y[i]<min) min=y[i];
		if (y[i]>max) max=y[i];
	}

	for (i=0;i<len;i++)
	{
		x_out=x[i];
		y_out=y[i];
		if (in->norm_x_axis==TRUE)
		{
			x_out/=x[len-1];
		}

		if (in->norm_y_axis==TRUE)
		{
			y_out=(y[i]-min)/(max-min);
		}

		sprintf(string,"%Le %Le #%Le\n",x_out,y_out,z[i]);
		buffer_add_string(in,string);
	}
}
sprintf(string,"#end\n");
buffer_add_string(in,string);

}

void buffer_add_string(struct buffer *in,char * string)
{
int str_len=strlen(string);
int pos=in->len;
in->len+=str_len;
//the +1 accounts for the zero at the end of a string
if (in->len+100>in->max_len)
{
	in->max_len+=1024;
	in->buf=(char*)realloc((char*)in->buf,sizeof(char)*in->max_len);
}

strcpy((char*)(in->buf+pos),string);
}

void buffer_add_info(struct simulation *sim,struct buffer *in)
{
char temp[400];

if (get_dump_status(sim,dump_write_headers)==TRUE)
{
	buffer_add_string(in,"#gpvdm\n");
	sprintf(temp,"#title %s\n",in->title);
	buffer_add_string(in,temp);

	sprintf(temp,"#type %s\n",in->type);
	buffer_add_string(in,temp);

	sprintf(temp,"#x_mul %Lf\n",in->x_mul);
	buffer_add_string(in,temp);

	sprintf(temp,"#y_mul %Lf\n",in->y_mul);
	buffer_add_string(in,temp);

	sprintf(temp,"#z_mul %Lf\n",in->z_mul);
	buffer_add_string(in,temp);

	sprintf(temp,"#data_mul %Lf\n",in->data_mul);
	buffer_add_string(in,temp);

	sprintf(temp,"#x_label %s\n",in->x_label);
	buffer_add_string(in,temp);

	sprintf(temp,"#y_label %s\n",in->y_label);
	buffer_add_string(in,temp);

	sprintf(temp,"#z_label %s\n",in->z_label);
	buffer_add_string(in,temp);

	sprintf(temp,"#data_label %s\n",in->data_label);
	buffer_add_string(in,temp);

	sprintf(temp,"#x_units %s\n",in->x_units);
	buffer_add_string(in,temp);

	sprintf(temp,"#y_units %s\n",in->y_units);
	buffer_add_string(in,temp);

	sprintf(temp,"#z_units %s\n",in->z_units);
	buffer_add_string(in,temp);

	sprintf(temp,"#data_units %s\n",in->data_units);
	buffer_add_string(in,temp);

	sprintf(temp,"#logscale_x %d\n",in->logscale_x);
	buffer_add_string(in,temp);

	sprintf(temp,"#logscale_y %d\n",in->logscale_y);
	buffer_add_string(in,temp);

	sprintf(temp,"#logscale_z %d\n",in->logscale_z);
	buffer_add_string(in,temp);

	sprintf(temp,"#logscale_data %d\n",in->logscale_data);
	buffer_add_string(in,temp);

	sprintf(temp,"#section_one %s\n",in->section_one);
	buffer_add_string(in,temp);

	sprintf(temp,"#section_two %s\n",in->section_two);
	buffer_add_string(in,temp);

	sprintf(temp,"#time %Le\n",in->time);
	buffer_add_string(in,temp);

	sprintf(temp,"#Vexternal %Le\n",in->Vexternal);
	buffer_add_string(in,temp);

	if (isnan(in->data_max)!=1)
	{
		sprintf(temp,"#data_max %Le\n",in->data_max);
		buffer_add_string(in,temp);
	}

	if (isnan(in->data_min)!=1)
	{
		sprintf(temp,"#data_min %Le\n",in->data_min);
		buffer_add_string(in,temp);
	}

	if (in->x!=0)
	{
		sprintf(temp,"#x %d\n",in->x);
		buffer_add_string(in,temp);
	}

	if (in->y!=0)
	{
		sprintf(temp,"#y %d\n",in->y);
		buffer_add_string(in,temp);
	}

	if (in->z!=0)
	{
		sprintf(temp,"#z %d\n",in->z);
		buffer_add_string(in,temp);
	}
}
}

void buffer_dump(struct simulation *sim,char * file,struct buffer *in)
{
	FILE *out;
	sim->files_written++;
	sim->bytes_written+=in->len;

	out = fopen(file, "wb");
	fwrite(in->buf, in->len, 1, out);
	fclose(out);
	log_write_file_access(sim,file,'w');
}

void buffer_dump_cache(struct simulation *sim,char * file_name,struct buffer *in)
{
	//printf("%s %d\n",file_name,dumpfiles_should_dump(sim,file_name));
	//getchar();
	if (dumpfiles_should_dump(sim,file_name)==0)
	{
		if (get_dump_status(sim,dump_use_cache)==FALSE)
		{
			buffer_dump(sim,file_name,in);
		}else
		{
			cache_add_item(sim,file_name,in->buf,in->len);
		}
	}
}

void buffer_add_dir(struct simulation *sim,char * file_name)
{
struct stat st = {0};

	if (get_dump_status(sim,dump_use_cache)==FALSE)
	{
		if (stat(file_name, &st) == -1)
		{
				mkdir(file_name, 0700);
		}
	}else
	{
		cache_add_dir(sim,file_name);
	}
}

void buffer_dump_path(struct simulation *sim,char *path,char * file,struct buffer *in)
{
	char wholename[200];
	join_path(2, wholename,path,file);
	buffer_dump_cache(sim,wholename,in);
}


void buffer_free(struct buffer *in)
{
free(in->buf);
in->len=0;
in->max_len=0;
}
