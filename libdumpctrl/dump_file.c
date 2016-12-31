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



#include "sim.h"
#include "inp.h"
#include "log.h"
#include <cal_path.h>

int dumpfiles_should_dump(struct simulation* sim,char *name)
{
	int i;
	for (i=0;i<sim->dumpfiles;i++)
	{
 	
		if (fnmatch2(sim->dumpfile[i].filename, name)==0)
		{
			if (sim->dumpfile[i].dump==TRUE)
			{
				return 0;
			}else
			{
				return -1;
			}
		}
	}

return 0;
}

void dumpfiles_process(struct simulation* sim,struct istruct *in,char *name)
{
	int i;
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

}

void dumpfiles_free(struct simulation* sim)
{
	free(sim->dumpfile);
	sim->dumpfiles=-1;
}

void dumpfiles_load(struct simulation* sim)
{
	int dump;
	struct inp_file inp;
	inp_init(sim,&inp);
	char file[100];
	int dump_file=0;
	int norm_y=0;
	char *line;
	int ret=0;
	int pos=0;
	if (inp_load_from_path(sim,&inp,get_input_path(sim),"dump_file.inp")!=0)
	{
		ewe(sim,"Error opening dump_file.inp");
	}

	sim->dumpfiles=inp_count_hash_tags(sim,&inp)-1;

	if (sim->dumpfiles<=0)
	{
		ewe(sim,"Error reading dump_file.inp");
	}

	sim->dumpfile=(struct dumpfiles_struct*)malloc(sizeof(struct dumpfiles_struct)*sim->dumpfiles);

	inp_reset_read(sim,&inp);

	do
	{
		line  = inp_get_string(sim,&inp);

		if (line==NULL)
		{
			break;
		}

		if (strcmp(line,"#end")==0)
		{
			break;
		}

		strcpy(sim->dumpfile[pos].filename,(line+1));


		line  = inp_get_string(sim,&inp);

		if (line==NULL)
		{
			break;
		}

		sim->dumpfile[pos].dump=english_to_bin(sim, line);


		line  = inp_get_string(sim,&inp);

		if (line==NULL)
		{
			break;
		}

		sim->dumpfile[pos].ynorm=english_to_bin(sim, line);
		//printf("%s %d\n",sim->dumpfile[pos].filename,sim->dumpfile[pos].ynorm);
		pos++;


	}while(1);


	inp_free(sim,&inp);


}
