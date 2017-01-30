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


#include "util.h"
#include "sim.h"
#include "dos.h"
#include "dump.h"
#include "complex_solver.h"
#include "ntricks.h"
#include "log.h"
#include "inp.h"
#include "solver_interface.h"
#include "newton_interface.h"
#include "mesh.h"
#include "remesh.h"
#include "lang.h"
#include <plot.h>
#include "device.h"
#include <cal_path.h>
#include <string.h>
#include <contacts.h>

int run_simulation(struct simulation *sim)
{
struct device cell;
log_clear(sim);

printf_log(sim,"%s\n",_("Runing simulation"));

device_init(&cell);
cell.onlypos=FALSE;

dump_init(sim,&cell);

set_dump_status(sim,dump_stop_plot, FALSE);
set_dump_status(sim,dump_print_text, TRUE);


char temp[1000];

cell.kl_in_newton=FALSE;

//if (strcmp(outputpath,"")!=0) strcpy(get_output_path(sim),outputpath);

//if (strcmp(inputpath,"")!=0) strcpy(get_input_path(sim),inputpath);

dump_load_config(sim,&cell);

int i;
int z;
int x;
int y;


join_path(2,temp,get_output_path(sim),"error.dat");
remove(temp);

join_path(2,temp,get_output_path(sim),"equilibrium");
remove_dir(sim,temp);

join_path(2,temp,get_output_path(sim),"snapshots");
remove_dir(sim,temp);

join_path(2,temp,get_output_path(sim),"light_dump");
remove_dir(sim,temp);

join_path(2,temp,get_output_path(sim),"dynamic");
remove_dir(sim,temp);

join_path(2,temp,get_output_path(sim),"frequency");
remove_dir(sim,temp);



load_config(sim,&cell);

if (strcmp(sim->force_sim_mode,"")!=0)
{
	strcpy(cell.simmode,sim->force_sim_mode);
}

if (strcmp(cell.simmode,"opticalmodel@optics")!=0)
{
	solver_init(sim,cell.solver_name);
	newton_init(sim,cell.newton_name);

	printf_log(sim,"%s: %d\n",_("Loading DoS layers"),cell.my_epitaxy.electrical_layers);
	char tempn[100];
	char tempp[100];
	i=0;

	for (i=0;i<cell.my_epitaxy.electrical_layers;i++)
	{
		dos_init(&cell,i);
		printf_log(sim,"%s %d/%d\n",_("Load DoS"),i,cell.my_epitaxy.electrical_layers);
		sprintf(tempn,"%s_dosn.dat",cell.my_epitaxy.dos_file[i]);
		sprintf(tempp,"%s_dosp.dat",cell.my_epitaxy.dos_file[i]);
		load_dos(sim,&cell,tempn,tempp,i);
	}

	device_alloc_traps(&cell);

	if (get_dump_status(sim,dump_write_converge)==TRUE)
	{
	sim->converge = fopena(get_output_path(sim),"converge.dat","w");
	fclose(sim->converge);

	sim->tconverge=fopena(get_output_path(sim),"tconverge.dat","w");
	fclose(sim->tconverge);
	}

	mesh_cal_layer_widths(&cell);

	long double depth=0.0;
	long double percent=0.0;
	long double value=0.0;


	for (z=0;z<cell.zmeshpoints;z++)
	{
		for (x=0;x<cell.xmeshpoints;x++)
		{
			for (y=0;y<cell.ymeshpoints;y++)
			{

				depth=cell.ymesh[y]-cell.layer_start[cell.imat[z][x][y]];
				percent=depth/cell.my_epitaxy.width[cell.imat_epitaxy[z][x][y]];
				cell.Nad[z][x][y]=get_dos_doping_start(&cell,cell.imat[z][x][y])+(get_dos_doping_stop(&cell,cell.imat[z][x][y])-get_dos_doping_start(&cell,cell.imat[z][x][y]))*percent;
			}
		}		
		
	}
	init_mat_arrays(&cell);




	for (z=0;z<cell.zmeshpoints;z++)
	{
		for (x=0;x<cell.xmeshpoints;x++)
		{
			for (y=0;y<cell.ymeshpoints;y++)
			{
				cell.phi[z][x][y]=0.0;
				cell.R[z][x][y]=0.0;
				cell.n[z][x][y]=0.0;
			}
		}
	}

	contacts_load(sim,&cell);

	cell.C=cell.xlen*cell.zlen*epsilon0*cell.epsilonr[0][0][0]/(cell.ylen+cell.other_layers);
	if (get_dump_status(sim,dump_print_text)==TRUE) printf_log(sim,"C=%Le\n",cell.C);
	cell.A=cell.xlen*cell.zlen;
	cell.Vol=cell.xlen*cell.zlen*cell.ylen;

	///////////////////////light model
	char old_model[100];
	gdouble old_Psun=0.0;
	old_Psun=light_get_sun(&cell.mylight);
	light_init(&cell.mylight);
	light_set_dx(&cell.mylight,cell.ymesh[1]-cell.ymesh[0]);
	light_load_config(sim,&cell.mylight);

	if (cell.led_on==TRUE)
	{
		strcpy(old_model,cell.mylight.mode);
		strcpy(cell.mylight.mode,"ray");
	}
	
	light_load_dlls(sim,&cell.mylight);
	light_setup_ray(sim,&cell,&cell.mylight);

	if (cell.led_on==TRUE)
	{
		cell.mylight.force_update=TRUE;

		light_set_sun(&(cell.mylight),1.0);
		light_set_sun_delta_at_wavelength(sim,&(cell.mylight),cell.led_wavelength);
		//light_set_unity_power(&(cell.mylight));
		light_solve_all(sim,&(cell.mylight));
		
		cell.mylight.force_update=FALSE;
		strcpy(cell.mylight.mode,old_model);
		light_set_sun(&(cell.mylight),old_Psun);
		light_free_dlls(sim,&cell.mylight);
		light_load_dlls(sim,&cell.mylight);
	}
	///////////////////////

	//update_arrays(&cell);

	contact_set_all_voltages(sim,&cell,0.0);
	get_initial(sim,&cell);

	remesh_shrink(&cell);

	if (cell.math_enable_pos_solver==TRUE)
	{
		for (z=0;z<cell.zmeshpoints;z++)
		{
			for (x=0;x<cell.xmeshpoints;x++)
			{
				solve_pos(sim,&cell,z,x);
			}
		}
	}


	time_init(sim,&cell);

	cell.N=0;
	cell.M=0;

	solver_realloc(sim,&cell);



	plot_open(sim);


	cell.go_time=FALSE;

	plot_now(sim,&cell,"plot");
	//set_solver_dump_every_matrix(1);

	find_n0(sim,&cell);
	//set_solver_dump_every_matrix(0);
	draw_gaus(&cell);


	if (cell.onlypos==TRUE)
	{
		join_path(2,temp,get_output_path(sim),"equilibrium");
		dump_1d_slice(sim,&cell,temp);
		device_free(sim,&cell);
		device_free_traps(&cell);
		mesh_free(sim,&cell);
		return 0;
	}
}


//Load the dll
if (is_domain(cell.simmode)!=0)
{
	char gussed_full_mode[200];
	if (guess_whole_sim_name(sim,gussed_full_mode,get_input_path(sim),cell.simmode)==0)
	{
		printf_log(sim,"I guess we are using running %s\n",gussed_full_mode);
		strcpy(cell.simmode,gussed_full_mode);
	}else
	{
		ewe(sim,"I could not guess which simulation to run from the mode %s\n",cell.simmode);
	}


}

run_electrical_dll(sim,&cell,strextract_domain(cell.simmode));


if (strcmp(cell.simmode,"opticalmodel@optics")!=0)
{
	device_free(sim,&cell);
	device_free_traps(&cell);
	mesh_free(sim,&cell);
	plot_close(sim);

	for (i=0;i<cell.my_epitaxy.electrical_layers;i++)
	{
		dos_free(&cell,i);
	}
	solver_free_memory(sim,&cell);

	newton_interface_free(sim);
	light_free(sim,&cell.mylight);
}



return cell.odes;
}

