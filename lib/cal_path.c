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



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cal_path.h"
#include "util.h"
#include "inp.h"
#include <log.h>

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include <limits.h>

int find_dll(struct simulation *sim, char *lib_path,char *lib_name)
{
char full_name[1024];
char temp[1024];
sprintf(full_name,"%s.so",lib_name);

join_path(2,lib_path,get_plugins_path(sim),full_name);
if (isfile(lib_path)==0)
{
	return 0;
}
	
struct dirent *next_file;
DIR *theFolder;

theFolder = opendir(get_plugins_path(sim));
if (theFolder!=NULL)
{
	while((next_file=readdir(theFolder))!=NULL)
	{
		split_dot(temp, next_file->d_name);
		if (strcmp(lib_name,temp)==0)
		{
			join_path(2,lib_path,get_plugins_path(sim),next_file->d_name);
			if (isfile(lib_path)==0)
			{
				closedir (theFolder);
				return 0;
			}
				
		}
	}

closedir (theFolder);

}else
{
	printf("can't open\n");
}

ewe(sim,"I can't find the dll %s,\n",lib_name);

return -1;
}

void set_path(struct simulation *sim,char *out, char *name)
{
char cwd[1000];
char temp[1000];

	if (getcwd(cwd,1000)==NULL)
	{
		ewe(sim,"IO error\n");
	}

	join_path(2,temp,cwd,name);

	if (isdir(temp)==0)
	{
		strcpy(out,temp);
		return;
	}

	join_path(2,temp,sim->exe_path,name);
	if (isdir(temp)==0)
	{
		strcpy(out,temp);
		return;
	}
	
	join_path(2,temp,"/usr/lib/gpvdm/",name);
	if (isdir(temp)==0)
	{
		strcpy(out,temp);
		return;
	}

	join_path(2,temp,"/usr/lib64/gpvdm/",name);
	if (isdir(temp)==0)
	{
		strcpy(out,temp);
		return;
	}
	
	join_path(2,temp,"/usr/share/gpvdm/",name);
	if (isdir(temp)==0)
	{
		strcpy(out,temp);
		return;
	}

	join_path(2,temp,sim->share_path,name);
	if (isdir(temp)==0)
	{
		strcpy(out,temp);
		return;
	}
	
	ewe(sim,"I can't find the %s\n",name);

}

void cal_path(struct simulation *sim)
{
char cwd[1000];
char temp[1000];
strcpy(cwd,"");
strcpy(sim->share_path,"nopath");

strcpy(sim->plugins_path,"");
strcpy(sim->lang_path,"");
strcpy(sim->input_path,"");
strcpy(sim->output_path,"");

char buff[PATH_MAX];
int len = readlink("/proc/self/exe", temp, 1000);
if (len == -1)
{
	ewe(sim,"IO error\n");
}


get_dir_name_from_path(sim->exe_path, temp);

//printf("I'm in %s\n",sim->exe_path);

if (isfile("configure.ac")==0)
{
	strcpy(sim->share_path,cwd);
	printf_log(sim,"share path: %s\n",sim->share_path);
}else
if (isfile("ver.py")==0)
{
	path_up_level(temp, cwd);
	strcpy(sim->share_path,temp);
	printf_log(sim,"share path: %s\n",sim->share_path);
}else
{
	strcpy(sim->share_path,"/usr/lib64/gpvdm/");
}

if (getcwd(cwd,1000)==NULL)
{
	ewe(sim,"IO error\n");
}

strcpy(sim->root_simulation_path,cwd);
strcpy(sim->output_path,cwd);
strcpy(sim->input_path,cwd);
set_path(sim,sim->plugins_path, "plugins");
set_path(sim,sim->lang_path, "lang");
set_path(sim,sim->materials_path, "materials");


}

char *get_materials_path(struct simulation *sim)
{
return sim->materials_path;
}

char *get_plugins_path(struct simulation *sim)
{
return sim->plugins_path;
}

char *get_lang_path(struct simulation *sim)
{
return sim->lang_path;
}

char *get_input_path(struct simulation *sim)
{
return sim->input_path;
}

char *get_output_path(struct simulation *sim)
{
return sim->output_path;
}

void set_output_path(struct simulation *sim,char *in)
{
strcpy(sim->output_path,in);
}

void set_input_path(struct simulation *sim,char *in)
{
strcpy(sim->input_path,in);
}


