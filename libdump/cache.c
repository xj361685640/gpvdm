//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012 -2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file cache.c
@brief cace so that files are not written to disk bit by bit instead at the end of the simulation
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "sim.h"
#include "dump.h"
#include <cal_path.h>
#include <string.h>
#include <log.h>
#include <stdlib.h>
#include <cache_struct.h>
#include <gui_hooks.h>
#include <lang.h>
#include "cache.h"

static int unused __attribute__((unused));

void cache_init(struct simulation *sim)
{
if ((sim->cache_len!=-1)||(sim->cache_max!=-1))
{
	ewe(sim,"You are trying to init the cache when it is not empty\n");
}
	sim->cache_len=0;
	sim->cache_max=2000;
	sim->cache=(struct cache_item *)malloc(sizeof(struct cache_item)*sim->cache_max);
}

void cache_dump(struct simulation *sim)
{
printf_log(sim,"%s\n",_("Writing files to disk"));
int i;
char send_data[200];
FILE *out;
struct stat st = {0};

gui_send_data(sim,"enable_pulse:false");
sprintf(send_data,"text:%s",_("Writing files to disk"));
gui_send_data(sim,send_data);

for (i=0;i<sim->cache_len;i++)
{
	if (sim->cache[i].len>=0)
	{
		sim->files_written++;
		sim->bytes_written+=sim->cache[i].len;
		out = fopen(sim->cache[i].file_name, "wb");
		fwrite(sim->cache[i].data, sim->cache[i].len, 1, out);
		fclose(out);
		log_write_file_access(sim,sim->cache[i].file_name,'w');
	}else
	{
		if (stat(sim->cache[i].file_name, &st) == -1)
		{
				mkdir(sim->cache[i].file_name, 0700);
		}
	}
	sprintf(send_data,"percent:%Lf",(gdouble)(i)/(gdouble)(sim->cache_len));
	gui_send_data(sim,send_data);
	//printf("write: %s\n",sim->cache[i].file_name);
}

}

void cache_free(struct simulation *sim)
{
int i;
	for (i=0;i<sim->cache_len;i++)
	{
		if (sim->cache[i].len>=0)
		{
			free(sim->cache[i].data);
		}
	}
	free(sim->cache);
	sim->cache_len=-1;
	sim->cache_max=-1;
}

int cache_search(struct simulation *sim,char * file_name)
{
return -1;
int i;
for (i=0;i<sim->cache_len;i++)
{
	if (strcmp(sim->cache[i].file_name,file_name)==0)
	{
		return i;
	}
}
return -1;
}

void cache_add_dir(struct simulation *sim,char * file_name)
{
	cache_add_item(sim,file_name,NULL,CACHE_DIR);
}

void cache_add_item(struct simulation *sim,char * file_name,char *data,int len)
{
int pos=0;
	pos=cache_search(sim,file_name);
	if (pos==-1)
	{
		strcpy(sim->cache[sim->cache_len].file_name,file_name);
		sim->cache[sim->cache_len].len=len;
		if (len>=0)
		{
			sim->cache[sim->cache_len].data=(char*)malloc(sizeof(char)*len);
			memcpy(sim->cache[sim->cache_len].data, data, len);
		}
		sim->cache_len++;
	}else
	{
		strcpy(sim->cache[pos].file_name,file_name);
		sim->cache[pos].len=len;
		if (len>=0)
		{
			sim->cache[pos].data=(char*)realloc(sim->cache[pos].data,sizeof(char)*len);
			memcpy(sim->cache[pos].data, data, len);
		}
	}

	if (sim->cache_len>=sim->cache_max)
	{
		sim->cache_max+=2000;
		sim->cache=(struct cache_item *)realloc(sim->cache,sizeof(struct cache_item)*sim->cache_max);
	}

}
