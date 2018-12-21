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

/** @file pl.c
	@brief Peform PL spectra.
*/


#include <stdio.h>
#include <dump.h>
#include <string.h>
#include <exp.h>
#include <dos.h>
#include "sim.h"
#include "i.h"
#include "buffer.h"
#include "pl.h"
#include <cal_path.h>

static long double light_energy=0.0;

long double calculate_photon_energy(struct istruct* in)
{
int i;
long double tot=0.0;

	for (i=0;i<in->len;i++)
	{
		tot+=in->data[i]*in->x[i]*Q;
	}

return tot;
}

void exp_cal_emission(struct simulation *sim,int number,struct device *in)
{
long double Re_h=0.0;
long double Re_e=0.0;
long double Rh_e=0.0;
long double Rh_h=0.0;
long double Rfree_e_h=0.0;

long double dEe_e=0.0;
long double dEe_h=0.0;
long double dEh_e=0.0;
long double dEh_h=0.0;


char name[100];
char out_dir[400];

int x;
int y;
int z;

int band;
struct buffer buf;
char temp[200];
int mat=0;
long double pl_fe_fh=0.0;
long double pl_fe_te=0.0;
long double pl_te_fh=0.0;
long double pl_th_fe=0.0;
long double pl_ft_th=0.0;

int pl_enabled=0;
char snapshot_dir[200];
char sim_name[200];

long double Vexternal=get_equiv_V(sim,in);
//char zip_file_name[400];

buffer_init(&buf);
//sprintf(zip_file_name,"%s/snapshots.zip",get_output_path(sim));
//buffer_zip_set_name(&buf,zip_file_name);

struct istruct fe_to_fh;
struct istruct fe_to_te;
struct istruct te_to_fh;
struct istruct fh_to_th;
struct istruct th_to_fe;

//struct istruct luminescence_tot;

long double max_Eg=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{

			if (in->Eg[z][x][y]>max_Eg)
			{
				max_Eg=in->Eg[z][x][y];
			}
		}
	}
}

//inter_init_mesh(&photons,40,0.0,2.254);

inter_init(sim,&fe_to_fh);
inter_init(sim,&fe_to_te);
inter_init(sim,&te_to_fh);
inter_init(sim,&fh_to_th);
inter_init(sim,&th_to_fe);
//inter_init(&luminescence_tot);

//double Re_e=0.0;
int pl_data_added=FALSE;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			mat=in->imat[z][x][y];
			pl_enabled=get_pl_enabled(in,mat);

				if (pl_enabled==TRUE)
				{
					pl_fe_fh=get_pl_fe_fh(in,mat);
					pl_fe_te=get_pl_fe_te(in,mat);
					pl_te_fh=get_pl_te_fh(in,mat);
					pl_th_fe=get_pl_th_fe(in,mat);
					pl_ft_th=get_pl_ft_th(in,mat);

					pl_data_added=TRUE;
					Rfree_e_h=in->Rfree[z][x][y]*pl_fe_fh;
					if (Rfree_e_h<0.0)
					{
						Rfree_e_h=0.0;
					}

					inter_append(&fe_to_fh,in->Eg[z][x][y],Rfree_e_h);

					for (band=0;band<in->srh_bands;band++)
					{
						//electrons
						dEe_e= -dos_get_band_energy_n(in,band,mat);
						Re_e=(in->nt_r1[z][x][y][band]-in->nt_r2[z][x][y][band])*pl_fe_te;	//electron capture - electron emission for an electron trap
						inter_append(&fe_to_te,dEe_e,Re_e);

						dEe_h=get_dos_Eg(in,mat)-dEe_e;
						Re_h=(in->nt_r3[z][x][y][band]-in->nt_r4[z][x][y][band])*pl_te_fh;	//hole capture-hole emission for an electron trap
						inter_append(&te_to_fh,dEe_h,Re_h);

						//holes
						dEh_e=get_dos_Eg(in,mat)-dEh_h;
						Rh_e=(in->pt_r3[z][x][y][band]-in->pt_r4[z][x][y][band])*pl_th_fe;	//electron capture - electron emission for a hole trap
						inter_append(&th_to_fe,dEh_e,Rh_e);

						dEh_h= -dos_get_band_energy_p(in,band,mat);
						Rh_h=(in->pt_r1[z][x][y][band]-in->pt_r2[z][x][y][band])*pl_ft_th;	//hole capture - hole emission for a hole trap
						inter_append(&fh_to_th,dEh_h,Rh_h);


					}
					
					in->Photon_gen[z][x][y]=Rfree_e_h;//+Re_e+Re_h+Rh_e+Rh_h;
				}
		}
	}
}

inter_mul(&fe_to_fh,in->ylen/((long double)in->ymeshpoints));
inter_mul(&fe_to_te,in->ylen/((long double)in->ymeshpoints));
inter_mul(&te_to_fh,in->ylen/((long double)in->ymeshpoints));
inter_mul(&th_to_fe,in->ylen/((long double)in->ymeshpoints));
inter_mul(&fh_to_th,in->ylen/((long double)in->ymeshpoints));


sprintf(temp,"%d",number);

strextract_name(sim_name,in->simmode);
sprintf(snapshot_dir,"snapshots");

join_path(3,out_dir,get_output_path(sim),snapshot_dir,temp);



//inter_dump(&fe_to_fh);
//inter_sort(&fe_to_fh);
if (pl_data_added==TRUE)
{
	inter_sort(&fe_to_te);
	inter_sort(&te_to_fh);
	inter_sort(&th_to_fe);
	inter_sort(&fh_to_th);

	inter_join_bins(&fe_to_fh,0.01);
	inter_join_bins(&fe_to_te,0.01);
	inter_join_bins(&te_to_fh,0.01);
	inter_join_bins(&th_to_fe,0.01);
	inter_join_bins(&fh_to_th,0.01);

	light_energy=0.0;

	light_energy+=calculate_photon_energy(&fe_to_fh);
	light_energy+=calculate_photon_energy(&fe_to_te);
	light_energy+=calculate_photon_energy(&te_to_fh);
	light_energy+=calculate_photon_energy(&th_to_fe);
	light_energy+=calculate_photon_energy(&fh_to_th);

	buffer_malloc(&buf);
	sprintf(name,"%s","fe_to_fh.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,"PL Spectra Free electron to free hole");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Energy");
	strcpy(buf.data_label,"Intensity");
	strcpy(buf.x_units,"eV");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=Vexternal;
	buffer_add_info(sim,&buf);
	buffer_add_xy_data(sim,&buf,fe_to_fh.x, fe_to_fh.data, fe_to_fh.len);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s","te_to_fh.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,"PL Spectra Free hole to trapped electron");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Energy");
	strcpy(buf.data_label,"Intensity");
	strcpy(buf.x_units,"eV");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=Vexternal;
	buffer_add_info(sim,&buf);
	buffer_add_xy_data(sim,&buf,te_to_fh.x, te_to_fh.data, te_to_fh.len);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s","fe_to_te.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,"PL Spectra free electron to trapped electron");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Energy");
	strcpy(buf.data_label,"Intensity");
	strcpy(buf.x_units,"eV");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=Vexternal;
	buffer_add_info(sim,&buf);
	buffer_add_xy_data(sim,&buf,fe_to_te.x, fe_to_te.data, fe_to_te.len);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);


	buffer_malloc(&buf);
	sprintf(name,"%s","th_to_fe.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,"PL Spectra Free electron to trapped hole");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Energy");
	strcpy(buf.data_label,"Intensity");
	strcpy(buf.x_units,"eV");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=Vexternal;
	buffer_add_info(sim,&buf);
	buffer_add_xy_data(sim,&buf,th_to_fe.x, th_to_fe.data, th_to_fe.len);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);


	buffer_malloc(&buf);
	sprintf(name,"%s","fh_to_th.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,"PL Spectra free hole to trapped hole");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Energy");
	strcpy(buf.data_label,"Intensity");
	strcpy(buf.x_units,"eV");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=Vexternal;
	buffer_add_info(sim,&buf);
	buffer_add_xy_data(sim,&buf,fh_to_th.x, fh_to_th.data, fh_to_th.len);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);
	
	/*inter_copy(&luminescence_tot,&fe_to_fh,TRUE);	
	inter_add(&luminescence_tot,&fe_to_te);
	inter_add(&luminescence_tot,&te_to_fh);
	inter_add(&luminescence_tot,&th_to_fe);
	inter_add(&luminescence_tot,&fh_to_th);

	buffer_malloc(&buf);
	sprintf(name,"%s","luminescence.dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,"Luminescence spectra");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Energy");
	strcpy(buf.data_label,"Intensity");
	strcpy(buf.x_units,"eV");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buf.Vexternal=Vexternal;
	buffer_add_info(sim,&buf);
	buffer_add_xy_data(&buf,luminescence_tot.x, luminescence_tot.data, luminescence_tot.len);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);*/

}

inter_free(&fe_to_fh);
inter_free(&fe_to_te);
inter_free(&te_to_fh);
inter_free(&th_to_fe);
inter_free(&fh_to_th);
//inter_free(&luminescence_tot);

return;
}

long double pl_get_light_energy()
{
return light_energy;
}

