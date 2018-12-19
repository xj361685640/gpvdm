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



#include <util.h>
#include <dump_ctrl.h>
#include <complex_solver.h>
#include <const.h>
#include <light.h>
#include <device.h>
#include <light_interface.h>

#include <functions.h>
#include <log.h>
#include <stdlib.h>
#include "ray.h"
#include <cal_path.h>
#include <sys/stat.h>


void light_update_ray_mat(struct light *in,int lam)
{
	int i;
	int layer;
	double n=1.0;
	double alpha=0.0;

	for (i=0;i<in->my_image.objects;i++)
	{
		layer=in->my_image.obj_mat_number[i];

		if (layer==-1)
		{
			alpha=0.0;
			n=1.0;
		}else
		{
			alpha=inter_get_noend(&(in->my_epitaxy->mat[layer]),in->l[lam]);
			n=inter_get_noend(&(in->my_epitaxy->mat_n[layer]),in->l[lam]);
		}

		in->my_image.obj_n[i]=n;
		in->my_image.obj_alpha[i]=alpha;
	}



}

EXPORT void light_dll_ver(struct simulation *sim)
{
        printf_log(sim,"Ray tracing light model\n");
}

double get_rand()
{
	double r=0.0;
	r = rand();
	r=(double)r/(double)RAND_MAX;
	return r;
}

EXPORT int light_dll_solve_lam_slice(struct simulation *sim,struct light *in,int lam)
{
	in->ray_trace=TRUE;
	int i;
	double x_vec;
	double y_vec;
	double angle;

	int x=0;
	int ii=0;
	int nang=(in->my_image.theta_steps);
	double dang=360.0/((double)nang);
	double eff=0.0;
	int sims=0;

	char name[400];
	char out_dir[400];
	struct stat st = {0};
	FILE *out;
	sprintf(out_dir,"%s/light_dump",get_output_path(sim));
	sprintf(name,"%s/light_ray_%d.dat",out_dir,(int)(in->l[lam]*1e9));

	if (stat(out_dir, &st) == -1)
	{
		mkdir(out_dir, 0700);
	}
	
	if ((out=fopen(name,"w")))
	{
		fclose(out);
	}
	
	if (get_dump_status(sim,dump_optics)==TRUE)
	{
		char one[100];
		sprintf(one,"Ray tracing at %Lf nm\n",in->l[lam]*1e9);
		waveprint(sim,one,in->l[lam]*1e9);
	}

	light_update_ray_mat(in,lam);


	//for (x=0;x<in->my_image.n_start_rays;x++)
	x=5;
	{
		angle=0.0;

		for (ii=0;ii<nang;ii++)
		{
			angle+=dang;
			//angle=42.0;
			//angle=get_rand()*360.0;
			x_vec=cos(2*PI*(angle/360.0));
			y_vec=sin(2*PI*(angle/360.0));

			struct vec start;
			vec_init(&start);

			struct vec dir;
			vec_init(&dir);

		
			vec_set(&start,in->my_image.start_rays[x].x,in->my_image.start_rays[x].y,0.0);
			vec_set(&dir,x_vec,y_vec,0.0);
			
			add_ray(sim,&in->my_image,&start,&dir,1.0);
			activate_rays(&in->my_image);
			int ret=0;
			for (i=0;i<50;i++)
			{
				propergate_next_ray(sim,&in->my_image);
				//dump_plane(&in->my_image);
				//getchar();
				ret=activate_rays(&in->my_image);
				
				if (ret==0)
				{
					break;
				}

			}

			if (get_dump_status(sim,dump_ray_trace_map)==TRUE)
			{
				dump_plane_to_file(name,&in->my_image,lam);
			}

			double e=get_eff(&in->my_image);
			eff+=e;
			sims++;
			ray_reset(&in->my_image);
		}
		
	}
	in->extract_eff[lam]=eff/((double)sims);


return 0;
}


