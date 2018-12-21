//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file ligh_dump_verbose_1d.c
	@brief Dumps 1D optical fields from light model.
*/


#include <string.h>
#include <sys/stat.h>
#include "util.h"
#include "const.h"
#include "dump_ctrl.h"
#include "light.h"
#include "buffer.h"
#include <cal_path.h>
#include <lang.h>

void light_dump_verbose_1d(struct simulation *sim,struct light *in, int i,char *ext)
{
	char line[1024];
	int ii=0;
	//int max=0;
	struct buffer data_photons;
	struct buffer data_light_1d_Ep;
	struct buffer data_light_1d_En;
	struct buffer data_pointing;
	struct buffer data_E_tot;

	struct buffer data_r;
	struct buffer data_t;
	struct buffer data_n;
	struct buffer data_alpha;

	buffer_init(&data_light_1d_Ep);
	buffer_init(&data_light_1d_En);
	buffer_init(&data_pointing);
	buffer_init(&data_E_tot);

	buffer_init(&data_r);
	buffer_init(&data_t);
	buffer_init(&data_n);
	buffer_init(&data_alpha);
	buffer_init(&data_photons);

	buffer_malloc(&data_photons);
	buffer_malloc(&data_light_1d_Ep);
	buffer_malloc(&data_light_1d_En);
	buffer_malloc(&data_pointing);
	buffer_malloc(&data_E_tot);
	buffer_malloc(&data_r);
	buffer_malloc(&data_t);
	buffer_malloc(&data_n);
	buffer_malloc(&data_alpha);

	char name_photons[200];
	char name_light_1d_Ep[200];
	char name_light_1d_En[200];
	char name_pointing[200];
	char name_E_tot[200];
	char name_r[200];
	char name_t[200];
	char name_n[200];
	char name_alpha[200];

	sprintf(name_photons,"light_1d_%.0Lf_photons%s.dat",in->l[i]*1e9,ext);

	sprintf(name_light_1d_Ep,"light_1d_%.0Lf_Ep%s.dat",in->l[i]*1e9,ext);
	sprintf(name_light_1d_En,"light_1d_%.0Lf_En%s.dat",in->l[i]*1e9,ext);
	sprintf(name_pointing,"light_1d_%.0Lf_pointing%s.dat",in->l[i]*1e9,ext);
	sprintf(name_E_tot,"light_1d_%.0Lf_E_tot%s.dat",in->l[i]*1e9,ext);
	sprintf(name_r,"light_1d_%.0Lf_r%s.dat",in->l[i]*1e9,ext);
	sprintf(name_t,"light_1d_%.0Lf_t%s.dat",in->l[i]*1e9,ext);
	sprintf(name_n,"light_1d_%.0Lf_n%s.dat",in->l[i]*1e9,ext);
	sprintf(name_alpha,"light_1d_%.0Lf_alpha%s.dat",in->l[i]*1e9,ext);

	//
	for (ii=0;ii<in->points;ii++)
	{
		sprintf(line,"%Le %Le\n",in->x[ii]-in->device_start,in->photons[i][ii]);
		buffer_add_string(&data_photons,line);

		sprintf(line,"%Le %Le %Le %Le\n",in->x[ii]-in->device_start,gpow(gpow(in->Ep[i][ii],2.0)+gpow(in->Epz[i][ii],2.0),0.5),in->Ep[i][ii],in->Epz[i][ii]);
		buffer_add_string(&data_light_1d_Ep,line);

		sprintf(line,"%Le %Le %Le %Le\n",in->x[ii]-in->device_start,gpow(gpow(in->En[i][ii],2.0)+gpow(in->Enz[i][ii],2.0),0.5),in->En[i][ii],in->Enz[i][ii]);
		buffer_add_string(&data_light_1d_En,line);

		sprintf(line,"%Le %Le\n",in->x[ii]-in->device_start,in->pointing_vector[i][ii]);
		buffer_add_string(&data_pointing,line);

		sprintf(line,"%Le %Le %Le\n",in->x[ii]-in->device_start,in->E_tot_r[i][ii],in->E_tot_i[i][ii]);
		buffer_add_string(&data_E_tot,line);

		sprintf(line,"%Le %Le %Le %Le\n",in->x[ii]-in->device_start,gcabs(in->r[i][ii]),gcreal(in->r[i][ii]),gcimag(in->r[i][ii]));
		buffer_add_string(&data_r,line);

		sprintf(line,"%Le %Le %Le %Le\n",in->x[ii]-in->device_start,gcabs(in->t[i][ii]),gcreal(in->t[i][ii]),gcimag(in->t[i][ii]));
		buffer_add_string(&data_t,line);

		sprintf(line,"%Le %Le\n",in->x[ii]-in->device_start,in->n[i][ii]);
		buffer_add_string(&data_n,line);

		sprintf(line,"%Le %Le\n",in->x[ii]-in->device_start,in->alpha[i][ii]);
		buffer_add_string(&data_alpha,line);
	}

	buffer_dump_path(sim,in->dump_dir,name_photons,&data_photons);

	buffer_dump_path(sim,in->dump_dir,name_light_1d_Ep,&data_light_1d_Ep);
	buffer_dump_path(sim,in->dump_dir,name_light_1d_En,&data_light_1d_En);
	buffer_dump_path(sim,in->dump_dir,name_pointing,&data_pointing);
	buffer_dump_path(sim,in->dump_dir,name_E_tot,&data_E_tot);
	buffer_dump_path(sim,in->dump_dir,name_r,&data_r);
	buffer_dump_path(sim,in->dump_dir,name_t,&data_t);
	buffer_dump_path(sim,in->dump_dir,name_n,&data_n);
	buffer_dump_path(sim,in->dump_dir,name_alpha,&data_alpha);



	buffer_free(&data_photons);

	buffer_free(&data_light_1d_Ep);
	buffer_free(&data_light_1d_En);
	buffer_free(&data_pointing);
	buffer_free(&data_E_tot);
	buffer_free(&data_r);
	buffer_free(&data_t);
	buffer_free(&data_n);
	buffer_free(&data_alpha);

}
