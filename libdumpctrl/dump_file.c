//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.rodmack.com
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

/** @file dump_file.c
@brief fine grained control of dump files, select which ones you want written or not.
*/


#include "sim.h"
#include "inp.h"
#include "log.h"
#include <cal_path.h>

int dumpfiles_should_dump(struct simulation* sim,char *name)
{
	char rel_path[1000];
	get_delta_path(sim,rel_path, get_output_path(sim),name);

	if (sim->dumpfiles<1)
	{
		return 0;
	}

	int in_path=FALSE;
	char file_name[200];
	char dir_name[200];
	get_file_name_from_path(file_name,rel_path);

	int i;
	for (i=0;i<sim->dumpfiles;i++)
	{

		//printf("'%s' '%s' %d\n",sim->dumpfile[i].file_name,file_name,fnmatch2(sim->dumpfile[i].file_name, file_name));
		if (fnmatch2(sim->dumpfile[i].file_name, file_name)==0)
		{

			in_path=is_dir_in_path(rel_path, sim->dumpfile[i].path_name);

			get_dir_name_from_path(dir_name, rel_path);
			if ((strcmp(sim->dumpfile[i].path_name,"root")==0)&&(strcmp(dir_name,"")==0))
			{
				in_path=0;
			}

			//printf("%s match a>%s b>%s %d %s '%s' %d\n",dir_name,sim->dumpfile[i].file_name, file_name,in_path,rel_path, sim->dumpfile[i].path_name,sim->dumpfile[i].write_out);

			if (in_path==0)
			{
				if (sim->dumpfile[i].write_out==TRUE)
				{
					return 0;
				}else
				{
					return -1;
				}
			}
		}
	}

return 0;
}

void dumpfiles_process(struct simulation* sim,struct istruct *in,char *name)
{
	return;
/*	int i;
	for (i=0;i<sim->dumpfiles;i++)
	{

		if (fnmatch2(sim->dumpfile[i].filename, name)==0)
		{
			if (sim->dumpfile[i].ynorm==TRUE)
			{
				inter_norm(in,1.0);
			}

			return;
		}


	}
*/
}

void dumpfiles_free(struct simulation* sim)
{
	return;
	free(sim->dumpfile);
	sim->dumpfiles=-1;
}

void dumpfiles_load(struct simulation* sim)
{
	int i;
	int dump;
	struct inp_file inp;
	inp_init(sim,&inp);
	char file[100];
	char file_name[200];
	int dump_file=0;
	int norm_y=0;
	char *line;
	int ret=0;

	if (inp_load_from_path(sim,&inp,get_input_path(sim),"dump_file.inp")!=0)
	{
		sim->dumpfiles=0;
		printf_log(sim,"Error opening dump_file.inp");
	}

	sim->dumpfiles=inp_count_hash_tags(sim,&inp)-2;

	if (sim->dumpfiles<=0)
	{
		printf_log(sim,"Error reading dump_file.inp");
		inp_free(sim,&inp);
		return;
	}

	sim->dumpfile=(struct dumpfiles_struct*)malloc(sizeof(struct dumpfiles_struct)*sim->dumpfiles);

	inp_reset_read(sim,&inp);

	for (i=0;i<sim->dumpfiles;i++)
	{
		line  = inp_get_string(sim,&inp);		//token

		line  = inp_get_string(sim,&inp);		//full_path

//		printf(">%s\n",line);
//		getchar();
		get_file_name_from_path(sim->dumpfile[i].file_name,line);
//		printf(">%s\n",sim->dumpfile[pos].file_name);
//		getchar();
		get_nth_dir_name_from_path(sim->dumpfile[i].path_name,line,0);

		line  = inp_get_string(sim,&inp);		//true false

		sim->dumpfile[i].write_out=english_to_bin(sim, line);

	}


	inp_free(sim,&inp);


}

void dumpfiles_turn_on_dir(struct simulation* sim,char *in)
{
int i;

	for (i=0;i<sim->dumpfiles;i++)
	{
		//printf("%s\n",sim->dumpfile[i].path_name);
		if (strcmp(sim->dumpfile[i].path_name,in)==0)
		{
			sim->dumpfile[i].write_out=TRUE;
		}
	}

}
