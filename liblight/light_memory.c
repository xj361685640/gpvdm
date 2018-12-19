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


#include <stdlib.h>
#include "util.h"
#include "const.h"
#include "light.h"
#include "device.h"
#include "config.h"
#include "util.h"
#include "lang.h"
#include "log.h"

static int unused __attribute__((unused));

void light_memory(struct simulation *sim,struct light *in)
{
printf_log(sim,"alloc: light_memory\n");
int i;

	if (in->align_mesh==FALSE)
	{
		in->dx=in->ylen/((gdouble)in->points);
	}else
	{
		in->points=(int)(in->ylen/in->dx);
	}

	in->x=(gdouble *)malloc(in->points*sizeof(gdouble));
	in->H1d=(gdouble *)malloc(in->points*sizeof(gdouble));
	in->l=(gdouble *)malloc(in->lpoints*sizeof(gdouble));
	in->Gn=(gdouble *)malloc(in->points*sizeof(gdouble));
	in->Gp=(gdouble *)malloc(in->points*sizeof(gdouble));
	in->photons_tot=(gdouble *)malloc(in->points*sizeof(gdouble));
	in->sun=(gdouble *)malloc(in->lpoints*sizeof(gdouble));
	in->layer_end=(gdouble *)malloc(in->points*sizeof(gdouble));
	in->sun_norm=(gdouble *)malloc(in->lpoints*sizeof(gdouble));
	in->sun_photons=(gdouble *)malloc(in->lpoints*sizeof(gdouble));
	in->sun_E=(gdouble *)malloc(in->lpoints*sizeof(gdouble));
	in->En=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->Enz=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->Ep=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->Epz=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->photons_asb=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->H=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->alpha=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->alpha0=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->photons=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->pointing_vector=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->E_tot_r=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->E_tot_i=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->n=(gdouble **)malloc(in->lpoints*sizeof(gdouble*));
	in->t=(gdouble complex **)malloc(in->lpoints*sizeof(gdouble complex *));
	in->r=(gdouble complex **)malloc(in->lpoints*sizeof(gdouble complex *));
	in->nbar=(gdouble complex **)malloc(in->lpoints*sizeof(gdouble complex *));
	for (i=0;i<in->lpoints;i++)
	{
		in->En[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->Enz[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->Ep[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->Epz[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->photons_asb[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->alpha[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->alpha0[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->photons[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->pointing_vector[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->E_tot_r[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->E_tot_i[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->n[i]=(gdouble *)malloc(in->points*sizeof(gdouble));
		in->t[i]=(gdouble complex *)malloc(in->points*sizeof(gdouble complex));
		in->r[i]=(gdouble complex *)malloc(in->points*sizeof(gdouble complex));
		in->nbar[i]=(gdouble complex *)malloc(in->points*sizeof(gdouble complex));
		in->H[i]=(gdouble *)malloc(in->points*sizeof(gdouble));

	}

	in->reflect=(gdouble *)malloc(in->lpoints*sizeof(gdouble));
	in->extract_eff=(gdouble *)malloc(in->lpoints*sizeof(gdouble));

	for (i=0;i<in->lpoints;i++)
	{
		in->extract_eff[i]=1.0;
	}
	
	in->layer=(int *)malloc(in->points*sizeof(int));

	in->N=0.0;
	in->N+=in->points;
	in->N+=in->points-1;
	in->N+=in->points;

	in->N+=in->points-1;
	in->N+=in->points; //t
	in->N+=in->points-1;
	//in->N+=1;
	in->M=in->points+in->points;
	in->Ti=malloc(in->N*sizeof(int));
	in->Tj= malloc(in->N*sizeof(int));
	in->Tx=malloc(in->N*sizeof(double));
	in->Txz=malloc(in->N*sizeof(double));
	in->b=malloc(in->M*sizeof(double));
	in->bz=malloc(in->M*sizeof(double));
}



void light_free_memory(struct simulation *sim,struct light *in)
{

	light_free_epitaxy(in);

	int i;
	free(in->H1d);
	free(in->x);
	free(in->layer_end);

	for (i=0;i<in->lpoints;i++)
	{
		free(in->En[i]);
		free(in->Enz[i]);
		free(in->Ep[i]);
		free(in->Epz[i]);

		free(in->photons_asb[i]);
		free(in->alpha[i]);
		free(in->alpha0[i]);
		free(in->photons[i]);
		free(in->pointing_vector[i]);
		free(in->E_tot_r[i]);
		free(in->E_tot_i[i]);
		free(in->n[i]);
		free(in->t[i]);
		free(in->r[i]);
		free(in->nbar[i]);
		free(in->H[i]);
	}


	free(in->t);
	free(in->r);
	free(in->nbar);
	free(in->En);
	free(in->Enz);
	free(in->Ep);
	free(in->Epz);
	free(in->photons_asb);
	free(in->alpha);
	free(in->alpha0);
	free(in->photons);
	free(in->pointing_vector);
	free(in->E_tot_r);
	free(in->E_tot_i);
	free(in->n);
	free(in->H);
	free(in->reflect);
	free(in->extract_eff);

	free(in->sun);
	free(in->sun_norm);
	free(in->sun_photons);
	free(in->sun_E);
	free(in->Gn);
	free(in->Gp);
	free(in->photons_tot);
	free(in->layer);
	free(in->Ti);
	free(in->Tj);
	free(in->Tx);
	free(in->Txz);
	free(in->b);
	free(in->bz);
	free(in->l);
	inter_free(&(in->sun_read));

	printf_log(sim,_("Freeing memory from the optical model\n"));
}
