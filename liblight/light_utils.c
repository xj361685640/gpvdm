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

gdouble light_get_sun(struct light *in)
{
return in->Psun;
}

void light_set_sun(struct light *in,gdouble Psun)
{
in->Psun=Psun;
}

void light_set_model(struct light *in,char *model)
{
strcpy(in->mode,model);
}

void light_solve_optical_problem(struct simulation *sim,struct light *in)
{
int i;

//if (get_dump_status(sim,dump_iodump)==TRUE) printf(sim,"light_solve_optical_problem\n");

	gdouble Psun=in->Psun*gpow(10.0,-in->ND);
	light_set_sun_power(in,Psun,in->laser_eff);
	//if ((in->laser_eff==0)&&(in->Psun==0))
	//{

	//	if (get_dump_status(sim,dump_optics)==TRUE) printf_log(sim,_("It's dark I know what the answer is\n"));
	//	for (i=0;i<in->lpoints;i++)
	//	{
	//		memset(in->En[i], 0.0, in->points*sizeof(gdouble));
	//		memset(in->Ep[i], 0.0, in->points*sizeof(gdouble));
	//		memset(in->Enz[i], 0.0, in->points*sizeof(gdouble));
	//		memset(in->Epz[i], 0.0, in->points*sizeof(gdouble));
	//	}
		
		//printf(sim,"dark\n");
	//}else
	//{
//
	light_solve_all(sim,in);
	//}

	light_cal_photon_density(in);


	//light_dump(in);

	for (i=0;i<in->lpoints;i++)
	{
		light_dump_1d(sim,in, i,"");
	}

}

gdouble light_cal_photon_density(struct light *in)
{
if (in->disable_cal_photon_density==FALSE)
{
	int i;
	int ii;
	gdouble tot=0.0;

	gdouble H_tot=0.0;
	gdouble photons_tot=0.0;

	for (ii=0;ii<in->points;ii++)
	{
		tot=0.0;
		H_tot=0.0;
		photons_tot=0.0;
		for (i=0;i<in->lpoints;i++)
		{
			in->E_tot_r[i][ii]=in->Ep[i][ii]+in->En[i][ii];
			in->E_tot_i[i][ii]=in->Enz[i][ii]+in->Epz[i][ii];
			in->pointing_vector[i][ii]=0.5*epsilon0*cl*in->n[i][ii]*(gpow(in->E_tot_r[i][ii],2.0)+gpow(in->E_tot_i[i][ii],2.0));

			if (strcmp(in->mode,"bleach")!=0)
			{
				in->photons[i][ii]=in->pointing_vector[i][ii]*(in->l[i]/(hp*cl));
				in->photons_asb[i][ii]=in->photons[i][ii]*in->alpha[i][ii];
			}
			gdouble E=((hp*cl)/in->l[i])/Q-in->Eg;

			//getchar();
			if (E>0.0)
			{
			in->H[i][ii]=E*Q*in->photons_asb[i][ii];
			}else
			{
			in->H[i][ii]=0.0;
			}

			//printf(sim,"%d %d %Le %Le %Le\n",i,ii,E,in->H[i][ii],in->photons_asb[i][ii]);
			photons_tot+=in->photons[i][ii]*in->dl;
			tot+=in->photons_asb[i][ii]*in->dl;
			H_tot+=in->H[i][ii]*in->dl;
			//printf(sim,"%Le %Le\n",E,in->l[i]);
			//getchar();*/

		}

	in->Gn[ii]=tot;
	in->Gp[ii]=tot;
	in->H1d[ii]=H_tot;
	in->photons_tot[ii]=photons_tot;

	for (i=0;i<in->lpoints;i++)
	{
		in->reflect[i]=(gpow(in->En[i][0],2.0)+gpow(in->Enz[i][0],2.0))/(gpow(in->Ep[i][0],2.0)+gpow(in->Epz[i][0],2.0));
	}

	}

}else
{
	if ((in->laser_eff==0)&&(in->Psun==0))
	{
		memset(in->Gn, 0.0, in->points*sizeof(gdouble));
		memset(in->Gp, 0.0, in->points*sizeof(gdouble));
	}
}

return 0.0;
}


void light_norm_photon_density(struct light *in)
{

int i;
int ii;
gdouble max=0.0;
for (i=0;i<in->lpoints;i++)
{

	max=0.0;

	for (ii=0;ii<in->points;ii++)
	{
			if ((in->x[ii]>in->device_start)&&(in->x[ii]<in->device_start+in->device_ylen))
			{
				if (in->photons[i][ii]>max)
				{
					max=in->photons[i][ii];
				}
			}
	}

	if (max>0.0)
	{
		for (ii=0;ii<in->points;ii++)
		{

				in->photons[i][ii]/=max;
		}
	}


}


}


void light_calculate_complex_n(struct light *in)
{
int i=0;
int ii=0;
gdouble nc=0.0;
gdouble kc=0.0;

gdouble nr=0.0;
gdouble kr=0.0;
gdouble complex n0=0.0+0.0*I;
gdouble complex n1=0.0+0.0*I;

	for (i=0;i<in->lpoints;i++)
	{
		for (ii=0;ii<in->points;ii++)
		{
			if (ii==in->points-1)
			{
				nr=in->n[i][ii];
				kr=in->alpha[i][ii]*(in->l[i]/(PI*4.0));
			}else
			{
				nr=in->n[i][ii+1];
				kr=in->alpha[i][ii+1]*(in->l[i]/(PI*4.0));
			}
			nc=in->n[i][ii];
			kc=in->alpha[i][ii]*(in->l[i]/(PI*4.0));

			n0=nc-kc*I;
			n1=nr-kr*I;

			in->nbar[i][ii]=n0;

			in->r[i][ii]=(n0-n1)/(n0+n1);
			in->t[i][ii]=(2.0*n0)/(n0+n1);

			//printf(sim,"%Le %Le\n",cabs(in->r[i][ii]),1.0-cabs(in->r[i][ii]));
			//printf(sim,"%Le %Le\n",cabs(in->t[i][ii]),1.0-cabs(in->t[i][ii]));
			//getchar();

		}


		memset(in->En[i], 0.0, in->points*sizeof(gdouble));
		memset(in->Ep[i], 0.0, in->points*sizeof(gdouble));
		memset(in->Enz[i], 0.0, in->points*sizeof(gdouble));
		memset(in->Epz[i], 0.0, in->points*sizeof(gdouble));

	}
}



void light_set_sun_power(struct light *in,gdouble power, gdouble laser_eff)
{
int i;

gdouble E=0.0;
//gdouble tot0=0.0;
for  (i=0;i<in->lpoints;i++)
{
	in->sun[i]=in->sun_norm[i]*power*1000.0;		//The 1000 is because it is 1000 W/m2

	E=hp*cl/in->l[i];
	in->sun_photons[i]=in->sun[i]/E;
	if (i==in->laser_pos)
	{
		if (in->pulse_width!=0.0)
		{
			in->sun_photons[i]+=laser_eff*((in->pulseJ/in->pulse_width/E)/(in->spotx*in->spoty))/in->dl;
		}

	}

	in->sun_E[i]=gpow(2.0*(in->sun_photons[i]*E)/(epsilon0*cl*in->n[i][0]),0.5);


}

gdouble tot=0.0;
for  (i=0;i<in->lpoints;i++)
{
	tot=tot+in->sun_photons[i]*in->dl;
}

}

void light_solve_all(struct simulation *sim,struct light *in)
{
int i;
int slices_solved=0;
	for (i=0;i<in->lpoints;i++)
	{

		if (in->sun_E[i]!=0.0)
		{
			light_solve_lam_slice(sim,in,i);
			slices_solved++;
		}else
		{
			memset(in->En[i], 0.0, in->points*sizeof(gdouble));
			memset(in->Ep[i], 0.0, in->points*sizeof(gdouble));
			memset(in->Enz[i], 0.0, in->points*sizeof(gdouble));
			memset(in->Epz[i], 0.0, in->points*sizeof(gdouble));
		}
	}
	
if (slices_solved==0)
{
	printf_log(sim,_("It was dark I did not solve any slices\n"));
}
}

void light_set_unity_power(struct light *in)
{
int i;

//gdouble E=0.0;

for  (i=0;i<in->lpoints;i++)
{
//	E=hp*cl/in->l[i];
	in->sun[i]=0.0;
	in->sun_photons[i]=0.0;
	in->sun_E[i]=1.0;//pow(2.0*(1e20*E)/(epsilon0*cl*in->n[i][0]),0.5);
}


}

int light_get_pos_from_wavelength(struct simulation *sim,struct light *in,double lam)
{
	int i=0;

	if (lam<in->lstart)
	{
		ewe(sim,"The desired wavelenght is smaller than the simulated range");
	}

	if (lam>in->lstop)
	{
		ewe(sim,"The desired wavelenght is smaller than the simulated range");
	}

	i=(int)((lam-in->lstart)/in->dl);

	return i;
}

void light_set_sun_delta_at_wavelength(struct simulation *sim,struct light *in,long double lam)
{
	int i;
	memset(in->sun, 0.0, in->lpoints*sizeof(gdouble));
	memset(in->sun_photons, 0.0, in->lpoints*sizeof(gdouble));
	memset(in->sun_E, 0.0, in->lpoints*sizeof(gdouble));
	i=light_get_pos_from_wavelength(sim,in,lam);
	in->sun_E[i]=1.0;
}

void light_set_unity_laser_power(struct light *in,int lam)
{
	memset(in->sun, 0.0, in->lpoints*sizeof(gdouble));
	memset(in->sun_photons, 0.0, in->lpoints*sizeof(gdouble));
	memset(in->sun_E, 0.0, in->lpoints*sizeof(gdouble));
	in->sun_E[lam]=1.0;
}

void light_get_mode(struct istruct *mode,int lam,struct light *in)
{
int ii;

for (ii=0;ii<mode->len;ii++)
{
	mode->data[ii]=inter_get_raw(in->x,in->photons[lam],in->points,in->device_start+mode->x[ii]);
}

}


void light_set_dx(struct light *in,gdouble dx)
{
in->dx=dx;
}

gdouble light_convert_density(struct device *in,gdouble start, gdouble width)
{
gdouble ratio=0.0;
gdouble stop=start+width;
	if ((start>=in->time)&&(start<=(in->time+in->dt)))
	{
		if (width>=in->dt)
		{
			ratio=((in->time+in->dt)-start)/in->dt;
		}

		if (width<=in->dt)
		{
			ratio=width/in->dt;
		}
	}


	if ((in->time>=start)&&(in->time<=stop))
	{
		ratio=1.0;
	}

return ratio;
}

void light_transfer_gen_rate_to_device(struct device *cell,struct light *in)
{
int z=0;
int x=0;
int y=0;

gdouble Gn=0.0;
gdouble Gp=0.0;

	if (in->align_mesh==FALSE)
	{
		for (y=0;y<cell->ymeshpoints;y++)
		{

			Gn=inter_get_raw(in->x,in->Gn,in->points,in->device_start+cell->ymesh[y])*in->Dphotoneff;
			Gp=inter_get_raw(in->x,in->Gp,in->points,in->device_start+cell->ymesh[y])*in->Dphotoneff;
			for (z=0;z<cell->zmeshpoints;z++)
			{
				for (x=0;x<cell->xmeshpoints;x++)
				{
					cell->Gn[z][x][y]=Gn*in->electron_eff;
					cell->Gp[z][x][y]=Gp*in->hole_eff;
					cell->Habs[z][x][y]=0.0;
				}
			}
		}
	}else
	{
		for (z=0;z<cell->zmeshpoints;z++)
		{
			for (x=0;x<cell->xmeshpoints;x++)
			{
				for (y=0;y<cell->ymeshpoints;y++)
				{
					cell->Gn[z][x][y]=in->Gn[in->device_start_i+y]*in->Dphotoneff;
					cell->Gp[z][x][y]=in->Gp[in->device_start_i+y]*in->Dphotoneff;
				}
			}
		}
	}

}
