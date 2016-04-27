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



#include <stdlib.h>
#include <dos.h>
#include "sim.h"
#include "dump.h"
#include "log.h"
#include <cal_path.h>

void init_dump(struct simulation *sim,struct device *in)
{
int i=0;
if (get_dump_status(sim,dump_iodump)==TRUE)
{
FILE *out;
	out=fopena(get_output_path(sim),"init_Fi.dat","w");
	for (i=0;i<in->ymeshpoints;i++)
	{
		fprintf(out,"%Le %Le\n",in->ymesh[i],in->Fi[i]);
	}
	fclose(out);

	out=fopena(get_output_path(sim),"init_Ec.dat","w");
	for (i=0;i<in->ymeshpoints;i++)
	{
		fprintf(out,"%Le %Le\n",in->ymesh[i],in->Ec[i]);
	}
	fclose(out);

	out=fopena(get_output_path(sim),"init_Ev.dat","w");
	for (i=0;i<in->ymeshpoints;i++)
	{
		fprintf(out,"%Le %Le\n",in->ymesh[i],in->Ev[i]);
	}
	fclose(out);

	out=fopena(get_output_path(sim),"init_n.dat","w");
	for (i=0;i<in->ymeshpoints;i++)
	{
		fprintf(out,"%Le %Le\n",in->ymesh[i],in->n[i]);
	}
	fclose(out);

	out=fopena(get_output_path(sim),"init_p.dat","w");
	for (i=0;i<in->ymeshpoints;i++)
	{
		fprintf(out,"%Le %Le\n",in->ymesh[i],in->p[i]);
	}
	fclose(out);
}
}


void get_initial(struct simulation *sim,struct device *in)
{
int i;


//getchar();
gdouble Ef=0.0;
gdouble phi_ramp=0.0;
gdouble Eg=in->Eg[0];
gdouble Xi=in->Xi[0];
gdouble charge_left=in->lcharge;
gdouble charge_right=in->rcharge;
gdouble top_l=0.0;
gdouble top_r=0.0;
if (in->interfaceleft==TRUE)
{
	top_l=in->phibleft-Eg;
}else
{
	if (in->lr_pcontact==LEFT)
	{
		top_l=get_top_from_p(in,charge_left,in->Te[0],in->imat[0]);
	}else
	{
		top_l= -(in->Eg[0]+get_top_from_n(in,charge_left,in->Te[0],in->imat[0]));
	}
}

if (in->interfaceright==TRUE)
{
	top_r= -in->phibright;
}else
{
	if (in->lr_pcontact==LEFT)
	{
		top_r=get_top_from_n(in,charge_right,in->Te[in->ymeshpoints-1],in->imat[in->ymeshpoints-1]);
	}else
	{
		top_r= -(Eg+get_top_from_p(in,charge_right,in->Te[in->ymeshpoints-1],in->imat[in->ymeshpoints-1]));
	}
}

if (get_dump_status(sim,dump_info_text)==TRUE)
{
	printf_log(sim,"check1= %Le %Le\n",get_p_den(in,top_l,in->Te[0],in->imat[0]),charge_left);
	printf_log(sim,"check2= %Le %Le\n",get_n_den(in,top_r,in->Te[in->ymeshpoints-1],in->imat[in->ymeshpoints-1]),charge_right);
}

gdouble delta_phi=top_l+top_r+in->Eg[0]+in->Xi[0]-in->Xi[in->ymeshpoints-1];
gdouble test_l= -in->Xi[0]+top_r;
gdouble test_r= -in->Xi[0]-in->Eg[0]-top_l;
in->vbi=delta_phi;
if (get_dump_status(sim,dump_print_text)==TRUE)
{
printf_log(sim,"delta=%Le\n",delta_phi);
printf_log(sim,">>>>top_l= %Le\n",top_l+Eg);
printf_log(sim,">>>>top_r= %Le\n",-top_r);
printf_log(sim,"left= %Le right = %Le  %Le %Le\n",test_l,test_r,test_r-test_l,delta_phi);
printf_log(sim,"%Le %Le %Le %Le %Le\n",top_l,top_r,Eg,delta_phi,in->phi[0]);
}

Ef= -(top_l+Xi+Eg);

gdouble Lp=get_p_den(in,(-in->Xi[0]-in->phi[0]-Eg)-Ef,in->Th[0],in->imat[0]);
gdouble Ln=get_n_den(in,Ef-(-in->Xi[0]-in->phi[0]),in->Te[0],in->imat[0]);
gdouble Rp=get_p_den(in,(-in->Xi[in->ymeshpoints-1]-delta_phi-Eg)-Ef,in->Th[in->ymeshpoints-1],in->imat[in->ymeshpoints-1]);
gdouble Rn=get_n_den(in,Ef-(-in->Xi[in->ymeshpoints-1]-delta_phi),in->Te[in->ymeshpoints-1],in->imat[in->ymeshpoints-1]);

in->l_electrons=Ln;
in->l_holes=Lp;
in->r_electrons=Rn;
in->r_holes=Rp;

if (get_dump_status(sim,dump_iodump)==TRUE)
{
printf_log(sim,"Ef=%Le\n",Ef);
printf_log(sim,"Holes on left contact = %Le\n", Lp);
printf_log(sim,"Electrons on left contact = %Le\n", Ln);

printf_log(sim,"Holes on right contact = %Le\n", Rp);
printf_log(sim,"Electrons on right contact = %Le\n", Rn);

FILE *contacts=fopena(get_output_path(sim),"contacts.dat","w");
fprintf (contacts,"%Le\n", Lp);
fprintf (contacts,"%Le\n", Ln);

fprintf (contacts,"%Le\n", Rp);
fprintf (contacts,"%Le\n", Rn);
fclose(contacts);
}

int band;
for (i=0;i<in->ymeshpoints;i++)
{
	phi_ramp=delta_phi*(in->ymesh[i]/in->ymesh[in->ymeshpoints-1]);

	in->Fi[i]=Ef;

	in->Fn[i]=Ef;
	in->Fp[i]=Ef;

	in->phi[i]=phi_ramp;

	in->x[i]=in->phi[i]+in->Fn[i];
	in->xp[i]= -(in->phi[i]+in->Fp[i]);

	in->Ec[i]= -in->phi[i]-in->Xi[i];
	if (in->Ec[i]<in->Fi[i])
	{
		in->phi[i]= -(in->Fi[i]+in->Xi[i]);
		in->Ec[i]= -in->phi[i]-in->Xi[i];
	}

	in->Ev[i]= -in->phi[i]-in->Xi[i]-in->Eg[i];
	if (in->Ev[i]>in->Fi[i])
	{
		in->phi[i]= -(in->Fi[i]+in->Xi[i]+in->Eg[i]);
		in->Ev[i]= -in->phi[i]-in->Xi[i]-in->Eg[i];

		in->Ec[i]= -in->phi[i]-in->Xi[i];
	}


	gdouble t=in->Fi[i]-in->Ec[i];
	gdouble tp=in->Ev[i]-in->Fi[i];

	in->n[i]=in->Nc[i]*exp(((t)*Q)/(kb*in->Te[i]));
	in->p[i]=in->Nv[i]*exp(((tp)*Q)/(kb*in->Th[i]));

//printf("%Le %Le\n",t,tp);
//getchar();
	in->mun[i]=get_n_mu(in,in->imat[i]);
	in->mup[i]=get_p_mu(in,in->imat[i]);

	for (band=0;band<in->srh_bands;band++)
	{
		in->Fnt[i][band]= -in->phi[i]-in->Xi[i]+dos_srh_get_fermi_n(in,in->n[i], in->p[i],band,in->imat[i],in->Te[i]);
		in->Fpt[i][band]= -in->phi[i]-in->Xi[i]-in->Eg[i]-dos_srh_get_fermi_p(in,in->n[i], in->p[i],band,in->imat[i],in->Th[i]);

		in->xt[i][band]=in->phi[i]+in->Fnt[i][band];
		in->nt[i][band]=get_n_pop_srh(sim,in,in->xt[i][band]+in->tt[i],in->Te[i],band,in->imat[i]);
		in->dnt[i][band]=get_dn_pop_srh(sim,in,in->xt[i][band]+in->tt[i],in->Te[i],band,in->imat[i]);


		in->xpt[i][band]= -(in->phi[i]+in->Fpt[i][band]);
		in->pt[i][band]=get_p_pop_srh(sim,in,in->xpt[i][band]-in->tpt[i],in->Th[i],band,in->imat[i]);
		in->dpt[i][band]=get_dp_pop_srh(sim,in,in->xpt[i][band]-in->tpt[i],in->Th[i],band,in->imat[i]);
	}

}

in->Vl=0.0;
in->Vr=delta_phi;
in->Vbi=delta_phi;
init_dump(sim,in);
//getchar();
if (in->stoppoint==1) exit(0);
return;
}

