//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file cal_path.c
	@brief Calculate the path of where stuff is, and if it can't find a file look harder.  Win/Linux.
*/


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
#include <stdarg.h>
#include <const.h>


#include <limits.h>

int get_delta_path(struct simulation *sim,char *out, char *root,char *file_name)
{
int root_len=strlen(root);
int file_name_len=strlen(file_name);
if (root_len>file_name_len)
{
	strcpy(out,file_name);
	return -1;
}

if (root_len==0)
{
	strcpy(out,file_name);
	return 0;
}
if (strcmp_begin(file_name,root)==0)
{
	strcpy(out,file_name+root_len+1);
	return 0;
}

return -1;
}

int find_dll(struct simulation *sim, char *lib_path,char *lib_name)
{
char full_name[PATHLEN];
char temp[PATHLEN];
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

}

ewe(sim,"I can't find the dll %s,\n",lib_name);

return -1;
}

void set_path(struct simulation *sim,char *out, char *name)
{
char cwd[PATHLEN];
char temp[PATHLEN];

	if (getcwd(cwd,PATHLEN)==NULL)
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
	printf("search %s",sim->exe_path);
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
 
	//Ubuntu
	join_path(2,temp,"/usr/lib/x86_64-linux-gnu/gpvdm/",name);
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

void get_file_name_from_path(char *out,char *in)
{
	int i=0;

	strcpy(out,in);

	if (strlen(in)==0)
	{
		return;
	}

	for (i=strlen(in)-1;i>=0;i--)
	{
		if ((in[i]=='\\') || (in[i]=='/'))
		{
			i++;
			strcpy(out,(char*)(in+i));
			return;
		}
	}
	
}

int is_dir_in_path(char *long_path, char* search_dir)
{
	if( strstr(long_path, search_dir) != NULL)
	{
		return 0;
	}else
	{
		return -1;
	}
return -1;
}

void get_nth_dir_name_from_path(char *out,char *in,int n)
{
	int i=0;
	int ii=0;
	int pos=0;
	int start=0;
	int stop=0;
	int count=0;
	strcpy(out,in);

	if (strlen(in)==0)
	{
		return;
	}

	for (i=0;i<strlen(in);i++)
	{
		if ((in[i]=='\\') || (in[i]=='/') || (i==strlen(in)-1))
		{
			if (i!=0)
			{
				stop=i;
				if (i==strlen(in)-1)
				{
					stop++;
				}


				if (count==n)
				{

					for (ii=start;ii<stop;ii++)
					{
						out[pos]=in[ii];
						pos++;
					}
					out[pos]=0;
					return;
				}
				start=stop+1;
				count++;
			}else
			{
				start=i+1;		// move it past the first / in a unix string
			}
		}
	}
	
}
void cal_path(struct simulation *sim)
{
char cwd[PATHLEN];
char temp[PATHLEN];

strcpy(cwd,"");
strcpy(temp,"");

strcpy(sim->share_path,"nopath");

strcpy(sim->plugins_path,"");
strcpy(sim->lang_path,"");
strcpy(sim->input_path,"");
strcpy(sim->output_path,"");

memset(temp, 0, PATHLEN * sizeof(char));
int len = readlink("/proc/self/exe", temp, PATHLEN);
if (len == -1)
{
	ewe(sim,"IO error\n");
}


get_dir_name_from_path(sim->exe_path, temp);

if (isfile("configure.ac")==0)
{
	strcpy(sim->share_path,cwd);
	//printf_log(sim,"share path: %s\n",sim->share_path);
}else
if (isfile("ver.py")==0)
{
	path_up_level(temp, cwd);
	strcpy(sim->share_path,temp);
	//printf_log(sim,"share path: %s\n",sim->share_path);
}else
{
	strcpy(sim->share_path,"/usr/lib64/gpvdm/");
}

if (getcwd(cwd,PATHLEN)==NULL)
{
	ewe(sim,"IO error\n");
}

strcpy(sim->root_simulation_path,cwd);
strcpy(sim->output_path,cwd);
strcpy(sim->input_path,cwd);
set_path(sim,sim->plugins_path, "plugins");
set_path(sim,sim->lang_path, "lang");
set_path(sim,sim->materials_path, "materials");
set_path(sim,sim->spectra_path, "spectra");

}

char *get_spectra_path(struct simulation *sim)
{
return sim->spectra_path;
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


void join_path(int max, ...)
{
	max=max+1;
	char temp[PATHLEN];
	strcpy(temp,"");
	va_list arguments;
	int i;
	va_start ( arguments, max );
	char *ret=va_arg ( arguments, char * );
	strcpy(ret,"");
	for (i = 1; i < max; i++ )
	{
		if ((i!=1)&&(strcmp(temp,"")!=0))
		{
			strcat(ret,"/");
		}
		strcpy(temp,va_arg ( arguments, char * ));
		strcat(ret,temp);
	}
	va_end ( arguments );                  // Cleans up the list

	return;
}


/**Make sure the slashes go the right way in a string for which ever OS we are on.
@param path path to check
*/
void assert_platform_path(char * path)
{
	int i=0;
	char temp[PATHLEN];
	strcpy(temp,"");
	int max=strlen(path);
	for (i=0;i<max;i++)
	{
		if ((path[i]=='\\')||(path[i]=='/'))
		{
			strcat(temp,"/");
		}else
		{
			temp[i]=path[i];
			temp[i+1]=0;
		}


	}

	strcpy(path,temp);

	return;
}
