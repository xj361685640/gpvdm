//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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



#include <sim.h>
#include <dump.h>
#include <ntricks.h>
#include <dump.h>
#include <inp.h>
#include <buffer.h>
#include <light_interface.h>
#include <dynamic_store.h>
#include <light.h>
#include <buffer.h>
#include "photon_extraction_plugin.h"
#include <device.h>
#include <cal_path.h>

static int unused __attribute__((unused));

void sim_optics(struct simulation *sim,struct device *in)
{

	char out_dir[2000];

	set_dump_status(sim,dump_lock, FALSE);
	//set_dump_status(sim,dump_optics, TRUE);
	//set_dump_status(sim,dump_optics_verbose, TRUE);
	set_dump_status(sim,dump_optics_summary, TRUE);

	struct light two;
	light_init(&two);

	struct buffer buf;
	buffer_init(&buf);

	struct inp_file inp;


	light_load_config(sim,&two);
	light_setup_dump_dir(sim,&two,out_dir);

	strcpy(two.mode,"ray");
	//two.Psun=1.0;
	light_load_dlls(sim,&two);
	light_setup_ray(sim,in,&two);
	two.my_image.theta_steps=360;
	//light_set_dx(&cell.mylight,cell.ymesh[1]-cell.ymesh[0]);


	two.disable_transfer_to_electrical_mesh=TRUE;

	inp_init(sim,&inp);

	//inp_load_from_path(sim,&inp,get_input_path(sim),"light.inp");
	//inp_search_gdouble(sim,&inp,&(Psun),"#Psun");
	//Psun=fabs(Psun);
	light_set_sun(&two,1.0);
	
	two.force_update=TRUE;

	light_set_unity_power(&two);
	light_solve_all(sim,&two);


	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,"Photon escape probability");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Wavelength");
	strcpy(buf.y_label,"Probability");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"a.u.");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=two.lpoints;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,two.l, two.extract_eff, two.lpoints);
	buffer_dump_path(sim,out_dir,"light_escape_probability.dat",&buf);
	buffer_free(&buf);
	
	light_dump(sim,&two);
	light_dump_summary(sim,&two);

	light_free(sim,&two);



}
