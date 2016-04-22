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
#include <util.h>
#include <patch.h>
#include <dump_ctrl.h>
#include <inp.h>
#include <const.h>

static int unused __attribute__((unused));

void patch(struct simulation *sim,char *dest,char * patch_file)
{
FILE *in;
char token[100];
char file[100];
char newtext[100];

if ((in=fopen(patch_file,"r"))==NULL)
{
    ewe(sim,"Error opening file: %s\n",patch_file);
}
char filetoedit[200];
if (get_dump_status(sim,dump_iodump)==TRUE) printf("Patch %s\n",patch_file);
int found=FALSE;

struct inp_file ifile;
inp_init(sim,&ifile);

do
{
	unused=fscanf(in,"%s",token);


	if (strcmp(token,"#end")==0)
	{
		break;
	}if (token[0]!='#')
	{
		ewe(sim,"error token does not begin with #\n",token);
	}
	else
	{
		found=TRUE;
		unused=fscanf(in,"%s",file);
		unused=fscanf(in,"%s",newtext);
		join_path(2, filetoedit,dest,file);
		inp_load(sim,&ifile,filetoedit);
		inp_replace(sim,&ifile,token,newtext);
		//edit_file_by_var(filetoedit,token,newtext);
	}

}while(!feof(in));

inp_free(sim,&ifile);

if (strcmp(token,"#end")!=0)
{
	ewe(sim,"Error at end of patch file\n");
}

fclose(in);

if (found==FALSE)
{
	ewe(sim,"Token not found\n");
}

return;
}
