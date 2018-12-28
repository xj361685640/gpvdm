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

/** @file dump_snapshot_utils.c
@brief Utils to handle the snapshots dir.
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <exp.h>
#include "sim.h"
#include "dump.h"
#include <cal_path.h>
#include <pl.h>
#include <probe.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <inp.h>

static int unused __attribute__((unused));

void dump_make_snapshot_dir(struct simulation *sim,char *out_dir ,long double time,long  double voltage, int number)
{
	char snapshots_dir[PATH_MAX];
	char sub_dir[200];
	char temp[200];
	struct buffer buf;
	buffer_init(&buf);

	sprintf(sub_dir,"%d",number);

	join_path(2,snapshots_dir,get_output_path(sim),"snapshots");


	buffer_add_dir(sim,snapshots_dir);

	buffer_malloc(&buf);

	sprintf(temp,"#end\n");
	buffer_add_string(&buf,temp);

	buffer_dump_path(sim,snapshots_dir,"snapshots.inp",&buf);
	buffer_free(&buf);

	join_path(2,out_dir,snapshots_dir,sub_dir);

	buffer_add_dir(sim,out_dir);

	buffer_malloc(&buf);

	sprintf(temp,"#dump_voltage\n");
	buffer_add_string(&buf,temp);

	sprintf(temp,"%Lf\n",voltage);
	buffer_add_string(&buf,temp);

	sprintf(temp,"#dump_time\n");
	buffer_add_string(&buf,temp);

	sprintf(temp,"%Lf\n",time);
	buffer_add_string(&buf,temp);

	sprintf(temp,"#ver\n");
	buffer_add_string(&buf,temp);

	sprintf(temp,"1.0\n");
	buffer_add_string(&buf,temp);

	sprintf(temp,"#end\n");
	buffer_add_string(&buf,temp);

	buffer_dump_path(sim,out_dir,"snapshot_info.dat",&buf);
	buffer_free(&buf);


}

