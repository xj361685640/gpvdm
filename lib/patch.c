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

/** @file patch.c
	@brief Patch an input file from a patch file, used usually for fitting.
*/


#include <stdio.h>
#include <string.h>
#include <util.h>
#include <patch.h>
#include <dump_ctrl.h>
#include <inp.h>
#include <const.h>
#include <log.h>

static int unused __attribute__((unused));

void patch(struct simulation *sim,char *dest,char * patch_file)
{
char *temp;
char token[100];
char file[100];
char newtext[100];
char description[200];

struct inp_file config_file;
inp_init(sim,&config_file);
if (inp_load(sim,&config_file,patch_file)!=0)
{
	ewe(sim,"patch file %s not found\n",patch_file);
}


char filetoedit[200];

printf_log(sim,"Patch %s\n",patch_file);

int found=FALSE;

struct inp_file ifile;
inp_init(sim,&ifile);

do
{
	temp = inp_get_string(sim,&config_file);
	if (temp==NULL)
	{
		break;
	}

	if (strcmp(temp,"#end")==0)
	{
		break;
	}

	strcpy(token,temp);

	if (token[0]!='#')
	{
		ewe(sim,"error token does not begin with # >%s< >%s<\n",token,patch_file);
	}
	else
	{
		found=TRUE;
		strcpy(file,inp_get_string(sim,&config_file));
		strcpy(description,inp_get_string(sim,&config_file));
		strcpy(newtext,inp_get_string(sim,&config_file));

		if (inp_load_from_path(sim,&ifile,dest,file)!=0)
		{
			ewe(sim,"File %s %s not found to patch.\n",dest,file);
		}

		inp_replace(sim,&ifile,token,newtext);

	}

}while(1);

inp_free(sim,&ifile);
inp_free(sim,&config_file);

return;
}
