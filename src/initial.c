//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2017 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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



#include <stdlib.h>
#include <dos.h>
#include "sim.h"
#include "dump.h"
#include "log.h"
#include <cal_path.h>
#include <buffer.h>
#include <lang.h>
#include <string.h>

void init_dump(struct simulation *sim,struct device *in)
{
struct buffer buf;
char out_dir[400];
char name[400];

if (get_dump_status(sim,dump_iodump)==TRUE)
{

	strcpy(out_dir,"");

	buffer_init(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s","init_Fi.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	sprintf(buf.title,"%s - %s",_("Equilibrium Fermi-level"),_("position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,"Fi");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"eV");
	strcpy(buf.section_one,_("1D position space output"));
	strcpy(buf.section_two,_("Transport"));
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=0.0;
	buffer_add_info(&buf);
	buffer_add_3d_device_data(&buf,in,  in->Fi);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s","init_Ec.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	sprintf(buf.title,"%s - %s",_("LUMO"),_("position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,"E_{c}");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"eV");
	strcpy(buf.section_one,_("1D position space output"));
	strcpy(buf.section_two,_("Transport"));
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=0.0;
	buffer_add_info(&buf);
	buffer_add_3d_device_data(&buf,in,  in->Ec);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s","init_Ev.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	sprintf(buf.title,"%s - %s",_("HOMO"),_("position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,"E_{v}");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"eV");
	strcpy(buf.section_one,_("1D position space output"));
	strcpy(buf.section_two,_("Transport"));
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=0.0;
	buffer_add_info(&buf);
	buffer_add_3d_device_data(&buf,in,  in->Ev);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s","init_n.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	sprintf(buf.title,"%s - %s",_("Electron density"),_("position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,"n");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"m^{-3}");
	strcpy(buf.section_one,_("1D position space output"));
	strcpy(buf.section_two,_("Transport"));
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=0.0;
	buffer_add_info(&buf);
	buffer_add_3d_device_data(&buf,in,  in->n);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s","init_p.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	sprintf(buf.title,"%s - %s",_("Hole density"),_("position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,_("n"));
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"m^{-3}");
	strcpy(buf.section_one,_("1D position space output"));
	strcpy(buf.section_two,_("Transport"));
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=0.0;
	buffer_add_info(&buf);
	buffer_add_3d_device_data(&buf,in,  in->p);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);
}
}


void get_initial(struct simulation *sim,struct device *in)
{
int i;
int z;
int x;
int y;

gdouble Ef=0.0;
gdouble phi_ramp=0.0;
gdouble Eg=0.0;
gdouble Xi=0.0;
gdouble charge_left=0.0;
gdouble charge_right=0.0;
gdouble top_l=0.0;
gdouble top_r=0.0;


Ef=0.0;
phi_ramp=0.0;
Eg=in->Eg[0][0][0];
Xi=in->Xi[0][0][0];
charge_left=in->lcharge;
charge_right=in->rcharge;
top_l=0.0;
top_r=0.0;

if (in->interfaceleft==TRUE)
{
	top_l=in->phibleft-Eg;
}else
{
	if (in->lr_pcontact==LEFT)
	{
		top_l=get_top_from_p(in,charge_left,in->Te[0][0][0],in->imat[0][0][0]);
	}else
	{
		top_l= -(in->Eg[0][0][0]+get_top_from_n(in,charge_left,in->Te[0][0][0],in->imat[0][0][0]));
	}
}

if (in->interfaceright==TRUE)
{
	top_r= -in->phibright;
}else
{
	if (in->lr_pcontact==LEFT)
	{
		top_r=get_top_from_n(in,charge_right,in->Te[0][0][in->ymeshpoints-1],in->imat[0][0][in->ymeshpoints-1]);
	}else
	{
		top_r= -(Eg+get_top_from_p(in,charge_right,in->Te[0][0][in->ymeshpoints-1],in->imat[0][0][in->ymeshpoints-1]));
	}
}

if (get_dump_status(sim,dump_info_text)==TRUE)
{
	printf_log(sim,"check1= %Le %Le\n",get_p_den(in,top_l,in->Te[0][0][0],in->imat[0][0][0]),charge_left);
	printf_log(sim,"check2= %Le %Le\n",get_n_den(in,top_r,in->Te[0][0][in->ymeshpoints-1],in->imat[0][0][in->ymeshpoints-1]),charge_right);
}

gdouble delta_phi=top_l+top_r+in->Eg[0][0][0]+in->Xi[0][0][0]-in->Xi[0][0][in->ymeshpoints-1];
gdouble test_l= -in->Xi[0][0][0]+top_r;
gdouble test_r= -in->Xi[0][0][0]-in->Eg[0][0][0]-top_l;
in->vbi=delta_phi;
if (get_dump_status(sim,dump_print_text)==TRUE)
{
printf_log(sim,"delta=%Le\n",delta_phi);
printf_log(sim,">>>>top_l= %Le\n",top_l+Eg);
printf_log(sim,">>>>top_r= %Le\n",-top_r);
printf_log(sim,"left= %Le right = %Le  %Le %Le\n",test_l,test_r,test_r-test_l,delta_phi);
printf_log(sim,"%Le %Le %Le %Le %Le\n",top_l,top_r,Eg,delta_phi,in->phi[0][0][0]);
}

Ef= -(top_l+Xi+Eg);

gdouble Lp=get_p_den(in,(-in->Xi[0][0][0]-in->phi[0][0][0]-Eg)-Ef,in->Th[0][0][0],in->imat[0][0][0]);
gdouble Ln=get_n_den(in,Ef-(-in->Xi[0][0][0]-in->phi[0][0][0]),in->Te[0][0][0],in->imat[0][0][0]);
gdouble Rp=get_p_den(in,(-in->Xi[0][0][in->ymeshpoints-1]-delta_phi-Eg)-Ef,in->Th[0][0][in->ymeshpoints-1],in->imat[0][0][in->ymeshpoints-1]);
gdouble Rn=get_n_den(in,Ef-(-in->Xi[0][0][in->ymeshpoints-1]-delta_phi),in->Te[0][0][in->ymeshpoints-1],in->imat[0][0][in->ymeshpoints-1]);

in->l_electrons=Ln;
in->l_holes=Lp;
in->r_electrons=Rn;
in->r_holes=Rp;

if (get_dump_status(sim,dump_built_in_voltage)==TRUE)
{
printf_log(sim,"Ef=%Le\n",Ef);
printf_log(sim,"Holes on left contact = %Le\n", Lp);
printf_log(sim,"Electrons on left contact = %Le\n", Ln);

printf_log(sim,"Holes on right contact = %Le\n", Rp);
printf_log(sim,"Electrons on right contact = %Le\n", Rn);

FILE *contacts=fopena(get_output_path(sim),"initial.dat","w");
fprintf (contacts,"#left_holes\n");
fprintf (contacts,"%Le\n", Lp);
fprintf (contacts,"#left_electrons\n");
fprintf (contacts,"%Le\n", Ln);

fprintf (contacts,"#right_holes\n");
fprintf (contacts,"%Le\n", Rp);
fprintf (contacts,"#right_electrons\n");
fprintf (contacts,"%Le\n", Rn);
fprintf (contacts,"#Vbi\n");
fprintf (contacts,"%Le\n", in->vbi);
fprintf (contacts,"#end\n");
fclose(contacts);
}

int band;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			phi_ramp=delta_phi*(in->ymesh[y]/in->ymesh[in->ymeshpoints-1]);

			in->Fi[z][x][y]=Ef;

			in->Fn[z][x][y]=Ef;
			in->Fp[z][x][y]=Ef;

			in->phi[z][x][y]=phi_ramp;

			in->x[z][x][y]=in->phi[z][x][y]+in->Fn[z][x][y];
			in->xp[z][x][y]= -(in->phi[z][x][y]+in->Fp[z][x][y]);

			in->Ec[z][x][y]= -in->phi[z][x][y]-in->Xi[z][x][y];
			if (in->Ec[z][x][y]<in->Fi[z][x][y])
			{
				in->phi[z][x][y]= -(in->Fi[z][x][y]+in->Xi[z][x][y]);
				in->Ec[z][x][y]= -in->phi[z][x][y]-in->Xi[z][x][y];
			}

			in->Ev[z][x][y]= -in->phi[z][x][y]-in->Xi[z][x][y]-in->Eg[z][x][y];
			if (in->Ev[z][x][y]>in->Fi[z][x][y])
			{
				in->phi[z][x][y]= -(in->Fi[z][x][y]+in->Xi[z][x][y]+in->Eg[z][x][y]);
				in->Ev[z][x][y]= -in->phi[z][x][y]-in->Xi[z][x][y]-in->Eg[z][x][y];

				in->Ec[z][x][y]= -in->phi[z][x][y]-in->Xi[z][x][y];
			}


			gdouble t=in->Fi[z][x][y]-in->Ec[z][x][y];
			gdouble tp=in->Ev[z][x][y]-in->Fi[z][x][y];

			in->n[z][x][y]=in->Nc[z][x][y]*exp(((t)*Q)/(kb*in->Te[z][x][y]));
			in->p[z][x][y]=in->Nv[z][x][y]*exp(((tp)*Q)/(kb*in->Th[z][x][y]));

		//printf("%Le %Le\n",t,tp);
		//getchar();
			in->mun[z][x][y]=get_n_mu(in,in->imat[z][x][y]);
			in->mup[z][x][y]=get_p_mu(in,in->imat[z][x][y]);

			for (band=0;band<in->srh_bands;band++)
			{
				in->Fnt[z][x][y][band]= -in->phi[z][x][y]-in->Xi[z][x][y]+dos_srh_get_fermi_n(in,in->n[z][x][y], in->p[z][x][y],band,in->imat[z][x][y],in->Te[z][x][y]);
				in->Fpt[z][x][y][band]= -in->phi[z][x][y]-in->Xi[z][x][y]-in->Eg[z][x][y]-dos_srh_get_fermi_p(in,in->n[z][x][y], in->p[z][x][y],band,in->imat[z][x][y],in->Th[z][x][y]);

				in->xt[z][x][y][band]=in->phi[z][x][y]+in->Fnt[z][x][y][band];
				in->nt[z][x][y][band]=get_n_pop_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,in->imat[z][x][y]);
				in->dnt[z][x][y][band]=get_dn_pop_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,in->imat[z][x][y]);


				in->xpt[z][x][y][band]= -(in->phi[z][x][y]+in->Fpt[z][x][y][band]);
				in->pt[z][x][y][band]=get_p_pop_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,in->imat[z][x][y]);
				in->dpt[z][x][y][band]=get_dp_pop_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,in->imat[z][x][y]);
			}

		}
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

