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




#include <string.h>
#include <sys/stat.h>
#include "util.h"
#include "const.h"
#include "dump_ctrl.h"
#include "light.h"
#include "buffer.h"
#include <cal_path.h>
#include <lang.h>

void light_dump_verbose_2d(struct simulation *sim,struct light *in)
{
	FILE *out;
	int i;
	int ii;
	struct buffer buf;
	char line[1024];
	char temp[1024];

	buffer_init(&buf);

	out=fopena(in->dump_dir,"light_2d_Ep.dat","w");
	for (i=0;i<in->lpoints;i++)
	{
		for (ii=0;ii<in->points;ii++)
		{
			fprintf(out,"%Le %Le %Le\n",in->l[i],in->x[ii]-in->device_start,gpow(gpow(in->Ep[i][ii],2.0)+gpow(in->Epz[i][ii],2.0),0.5));

		}

	fprintf(out,"\n");
	}
	fclose(out);

	out=fopena(in->dump_dir,"light_2d_En.dat","w");
	for (i=0;i<in->lpoints;i++)
	{
		for (ii=0;ii<in->points;ii++)
		{
			fprintf(out,"%Le %Le %Le\n",in->l[i],in->x[ii]-in->device_start,gpow(gpow(in->En[i][ii],2.0)+gpow(in->Enz[i][ii],2.0),0.5));
		}

	fprintf(out,"\n");
	}
	fclose(out);

	out=fopena(in->dump_dir,"light_2d_E_mod.dat","w");
	for (i=0;i<in->lpoints;i++)
	{
		for (ii=0;ii<in->points;ii++)
		{
			fprintf(out,"%Le %Le %Le\n",in->l[i],in->x[ii]-in->device_start,gpow(gpow(in->Ep[i][ii]+in->En[i][ii],2.0)+gpow(in->Enz[i][ii]+in->Epz[i][ii],2.0),1.0));
		}

	fprintf(out,"\n");
	}
	fclose(out);








	out=fopena(in->dump_dir,"light_2d_n.dat","w");
	for (i=0;i<in->lpoints;i++)
	{
		for (ii=0;ii<in->points;ii++)
		{
			fprintf(out,"%Le %Le %Le\n",in->l[i],in->x[ii]-in->device_start,in->n[i][ii]);
		}

	fprintf(out,"\n");
	}
	fclose(out);

	out=fopena(in->dump_dir,"light_lambda_sun.dat","w");
	for (i=0;i<in->lpoints;i++)
	{
		fprintf(out,"%Le %Le\n",in->l[i],in->sun[i]);
	}
	fclose(out);

	out=fopena(in->dump_dir,"light_lambda_sun_norm.dat","w");
	for (i=0;i<in->lpoints;i++)
	{
		fprintf(out,"%Le %Le\n",in->l[i],in->sun_norm[i]);
	}
	fclose(out);

	out=fopena(in->dump_dir,"light_lambda_sun_photons.dat","w");
	for (i=0;i<in->lpoints;i++)
	{
		fprintf(out,"%Le %Le\n",in->l[i],in->sun_photons[i]);
	}
	fclose(out);

	buffer_malloc(&buf);
	buf.y_mul=1e9;
	buf.x_mul=1e9;
	strcpy(buf.title,"Optical absorption coefficient");
	strcpy(buf.type,"heat");
	strcpy(buf.x_label,"Position");
	strcpy(buf.y_label,"Wavelength");
	strcpy(buf.data_label,"Density");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"nm");
	strcpy(buf.data_units,"m^{-3}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=in->lpoints;
	buf.y=in->points;
	buf.z=1;
	buffer_add_info(sim,&buf);

	sprintf(temp,"#data\n");
	buffer_add_string(&buf,temp);
	
	for (i=0;i<in->lpoints;i++)
	{
		for (ii=0;ii<in->points;ii++)
		{
			sprintf(line,"%Le %Le %Le\n",in->l[i],in->x[ii]-in->device_start,in->alpha[i][ii]);
			buffer_add_string(&buf,line);
		}

	buffer_add_string(&buf,"\n");
	}

	sprintf(temp,"#end\n");
	buffer_add_string(&buf,temp);
	
	buffer_dump_path(sim,in->dump_dir,"light_lambda_alpha.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1e9;
	buf.x_mul=1e9;
	strcpy(buf.title,"Optical absorption coefficient");
	strcpy(buf.type,"heat");
	strcpy(buf.x_label,"Position");
	strcpy(buf.y_label,"Wavelength");
	strcpy(buf.data_label,"Density");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"nm");
	strcpy(buf.data_units,"m^{-3}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=in->lpoints;
	buf.y=in->points;
	buf.z=1;
	buffer_add_info(sim,&buf);

	sprintf(temp,"#data\n");
	buffer_add_string(&buf,temp);
	
	for (i=0;i<in->lpoints;i++)
	{
		for (ii=0;ii<in->points;ii++)
		{
			sprintf(line,"%Le %Le %Le\n",in->l[i],in->x[ii]-in->device_start,in->n[i][ii]);
			buffer_add_string(&buf,line);
		}

	buffer_add_string(&buf,"\n");
	}

	sprintf(temp,"#end\n");
	buffer_add_string(&buf,temp);
	
	buffer_dump_path(sim,in->dump_dir,"light_lambda_n.dat",&buf);
	buffer_free(&buf);

	out=fopena(in->dump_dir,"light_sun_wavelength_E.dat","w");
	for (ii=0;ii<in->lpoints;ii++)
	{
		fprintf(out,"%Le %Le\n",in->l[ii],in->sun_E[ii]);
	}
	fclose(out);


}
