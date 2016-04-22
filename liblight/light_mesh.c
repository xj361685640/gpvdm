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



#include "util.h"
#include "const.h"
#include "light.h"
#include "device.h"
#include "const.h"
#include "dump.h"
#include "config.h"
#include "inp.h"
#include "util.h"
#include "hard_limit.h"
#include "epitaxy.h"
#include "lang.h"
#include "log.h"

static int unused __attribute__((unused));


void light_init_mesh(struct light *in)
{
	int i;
	gdouble pos=0.0;
	pos=in->dx;

	int layer=0;
	gdouble layer_end=in->thick[layer];
	//printf("%Le\n",layer_end);
	for (i=0;i<in->points;i++)
	{
		in->x[i]=pos;
		in->layer_end[i]=layer_end-pos;
		in->layer[i]=layer;
		if (in->device_start_layer>=layer) in->device_start_i=i;
		//printf("%d %d %d %d\n",in->device_start_i,in->points,in->device_start_layer,layer);
		pos+=in->dx;

		if (pos>layer_end)
		{
			//printf("%Le\n",in->thick[layer],la);
			//do
			//{
			if (layer<(in->layers-1))
			{
				layer++;
			//}while(in->thick[layer]==0.0);

				layer_end=layer_end+in->thick[layer];
			}
		}

		//printf("%Le %d %d %Le\n",in->x[i],i,layer,in->thick[layer]);
	}
	in->device_start_i++;

	in->dl=(in->lstop-in->lstart)/((gdouble)in->lpoints);

	pos=in->lstart;
	for (i=0;i<in->lpoints;i++)
	{
		in->l[i]=pos;
		pos+=in->dl;
	}

	int ii;

	for (i=0;i<in->lpoints;i++)
	{
		for (ii=0;ii<in->points;ii++)
		{
			in->alpha[i][ii]=inter_get_noend(&(in->mat[in->layer[ii]]),in->l[i]);
			in->alpha0[i][ii]=in->alpha[i][ii];
			in->n[i][ii]=inter_get_noend(&(in->mat_n[in->layer[ii]]),in->l[i]);
		}
	}

	light_calculate_complex_n(in);


	for (i=0;i<in->lpoints;i++)
	{
		in->sun_norm[i]=inter_get_noend(&(in->sun_read),in->l[i]);
	}

	gdouble tot=0.0;
	for  (i=0;i<in->lpoints;i++)
	{
		tot+=in->dl*in->sun_norm[i];
	}

	for  (i=0;i<in->lpoints;i++)
	{
		in->sun_norm[i]/=tot;
	}


}
