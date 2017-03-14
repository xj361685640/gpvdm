//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <i.h>
#include <exp.h>
#include <dos.h>
#include "sim.h"
#include "dump.h"
#include "buffer.h"
#include "dynamic_store.h"
#include "memory.h"
#include "contacts.h"
#include <lang.h>
#include <cal_path.h>


static int unused __attribute__((unused));

void dump_dynamic_init(struct simulation *sim,struct dynamic_store *store,struct device *in)
{
int x=0;
int y=0;
int z=0;


if (get_dump_status(sim,dump_dynamic)==TRUE)
{
	inter_init(sim,&(store->charge_change));
	inter_init(sim,&(store->jout));
	inter_init(sim,&(store->jn_avg));
	inter_init(sim,&(store->jp_avg));
	inter_init(sim,&(store->dynamic_jn));
	inter_init(sim,&(store->dynamic_jp));
	inter_init(sim,&(store->jnout_mid));
	inter_init(sim,&(store->jpout_mid));
	inter_init(sim,&(store->iout));
	inter_init(sim,&(store->iout_left));
	inter_init(sim,&(store->iout_right));
	inter_init(sim,&(store->gexout));
	inter_init(sim,&(store->ntrap));
	inter_init(sim,&(store->ptrap));
	inter_init(sim,&(store->ntrap_delta_out));
	inter_init(sim,&(store->ptrap_delta_out));
	inter_init(sim,&(store->nfree));
	inter_init(sim,&(store->pfree));
	inter_init(sim,&(store->nfree_delta_out));
	inter_init(sim,&(store->pfree_delta_out));
	inter_init(sim,&(store->Rnpout));
	inter_init(sim,&(store->nfree_to_ptrap));
	inter_init(sim,&(store->pfree_to_ntrap));
	inter_init(sim,&(store->Rnout));
	inter_init(sim,&(store->Rpout));
	inter_init(sim,&(store->nrelax_out));
	inter_init(sim,&(store->prelax_out));
	inter_init(sim,&(store->tpc_mue));
	inter_init(sim,&(store->tpc_muh));
	inter_init(sim,&(store->tpc_mu_avg));
	inter_init(sim,&(store->tpc_filledn));
	inter_init(sim,&(store->tpc_filledp));
	inter_init(sim,&(store->dynamic_np));
	inter_init(sim,&(store->only_n));
	inter_init(sim,&(store->only_p));
	inter_init(sim,&(store->E_field));
	inter_init(sim,&(store->dynamic_Vapplied));
	inter_init(sim,&(store->dynamic_charge_tot));
	inter_init(sim,&(store->dynamic_pl));
	inter_init(sim,&(store->dynamic_jn_drift));
	inter_init(sim,&(store->dynamic_jn_diffusion));

	inter_init(sim,&(store->dynamic_jp_drift));
	inter_init(sim,&(store->dynamic_jp_diffusion));

	inter_init(sim,&(store->dynamic_qe));

	inter_init(sim,&(store->srh_n_r1));
	inter_init(sim,&(store->srh_n_r2));
	inter_init(sim,&(store->srh_n_r3));
	inter_init(sim,&(store->srh_n_r4));

	inter_init(sim,&(store->srh_p_r1));
	inter_init(sim,&(store->srh_p_r2));
	inter_init(sim,&(store->srh_p_r3));
	inter_init(sim,&(store->srh_p_r4));

	inter_init(sim,&(store->band_bend));

	malloc_3d_gdouble(in,&(store->band_snapshot));
	for (z=0;z<in->zmeshpoints;z++)
	{
		for (x=0;x<in->xmeshpoints;x++)
		{
			for (y=0;y<in->ymeshpoints;y++)
			{
				store->band_snapshot[z][x][y]=in->phi[z][x][y];
			}
		}
	}
}
}

void dump_dynamic_save(struct simulation *sim,char *outputpath,struct dynamic_store *store)
{
int i;
int sub=TRUE;
char temp[200];
struct buffer buf;
buffer_init(&buf);

if (get_dump_status(sim,dump_dynamic)==TRUE)
{

	if (get_dump_status(sim,dump_norm_time_to_one)==TRUE)
	{
		buf.norm_x_axis=TRUE;
	}

	if (get_dump_status(sim,dump_norm_y_axis)==TRUE)
	{
		buf.norm_y_axis=TRUE;
	}

	char out_dir[1000];
	join_path(2, out_dir,outputpath,"dynamic");
	struct stat st = {0};

	if (stat(out_dir, &st) == -1)
	{
		mkdir(out_dir, 0700);
	}

	char outpath[200];

	sprintf(outpath,"%s%s",out_dir,"dynamic_jn_mid.dat");
	inter_save(&(store->jnout_mid),outpath);

	struct istruct one;
	inter_copy(&one,&(store->jnout_mid),TRUE);
	inter_deriv(&one,&(store->jnout_mid));
	sprintf(outpath,"%s%s",out_dir,"dynamic_djn.dat");
	inter_save(&one,outpath);
	inter_free(&one);

	sprintf(outpath,"%s%s",out_dir,"dynamic_jp_mid.dat");
	inter_save(&(store->jpout_mid),outpath);

	inter_copy(&one,&(store->jpout_mid),TRUE);
	inter_deriv(&one,&(store->jpout_mid));
	sprintf(outpath,"%s%s",out_dir,"dynamic_djp.dat");
	inter_save(&one,outpath);
	inter_free(&one);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	sprintf(buf.title,"%s + %s",_("Hole drift current"),_(" Hole diffusion current"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Time");
	strcpy(buf.data_label,"Hole current density");
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"A m^{-2}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_jp_drift).len;
	buf.z=1;
	buffer_add_info(&buf);

	for (i=0;i<(store->dynamic_jp_drift).len;i++)
	{
		sprintf(temp,"%Le %Le\n",(store->dynamic_jp_drift).x[i],(store->dynamic_jp_drift).data[i]+(store->dynamic_jp_diffusion).data[i]);
		buffer_add_string(&buf,temp);
	}
	buffer_dump_path(sim,out_dir,"dynamic_jp_drift_plus_diffusion.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	sprintf(buf.title,"%s + %s",_("Electron drift current"),_("Electron diffusion current"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Hole current density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"A m^{-2}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_jn_drift).len;
	buf.z=1;
	buffer_add_info(&buf);
	for (i=0;i<(store->dynamic_jn_drift).len;i++)
	{
		sprintf(temp,"%Le %Le\n",(store->dynamic_jn_drift).x[i],(store->dynamic_jn_drift).data[i]+(store->dynamic_jn_diffusion).data[i]);
		buffer_add_string(&buf,temp);
	}
	buffer_dump_path(sim,out_dir,"dynamic_jn_drift_plus_diffusion.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Current density at contacts"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Current density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"A m^{-2}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->jout).len;
	buf.z=1;
	buffer_add_info(&buf);
	if (sub==TRUE)
	{
		inter_sub_gdouble(&(store->jout),(store->jout).data[0]);
		inter_mul(&(store->jout),-1.0);
	}
	buffer_add_xy_data(&buf,(store->jout).x, (store->jout).data, (store->jout).len);
	buffer_dump_path(sim,out_dir,"dynamic_j.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Change in charge distribution"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("percent"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"\\%");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->charge_change).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->charge_change).x, (store->charge_change).data, (store->charge_change).len);
	buffer_dump_path(sim,out_dir,"dynamic_charge_change.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	sprintf(buf.title,"%s",_("Electron drift current"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Electron current density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"A m^{-2}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_jn_drift).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_jn_drift).x, (store->dynamic_jn_drift).data, (store->dynamic_jn_drift).len);
	buffer_dump_path(sim,out_dir,"dynamic_jn_drift.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Electron diffusion current"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Electron current density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"A m^{-2}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_jn_diffusion).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_jn_diffusion).x, (store->dynamic_jn_diffusion).data, (store->dynamic_jn_diffusion).len);
	buffer_dump_path(sim,out_dir,"dynamic_jn_diffusion.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	sprintf(buf.title,"%s",_("Hole drift current"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Hole current density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"A m^{-2}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_jp_drift).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_jp_drift).x, (store->dynamic_jp_drift).data, (store->dynamic_jp_drift).len);
	buffer_dump_path(sim,out_dir,"dynamic_jp_drift.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	sprintf(buf.title,"%s",_("Hole diffusion current"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Hole current density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"A m^{-2}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_jp_diffusion).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_jp_diffusion).x, (store->dynamic_jp_diffusion).data, (store->dynamic_jp_diffusion).len);
	buffer_dump_path(sim,out_dir,"dynamic_jp_diffusion.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	sprintf(buf.title,"%s",_("Jn at contacts"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Electron current density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"A m^{-2}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_jn).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_jn).x, (store->dynamic_jn).data, (store->dynamic_jn).len);
	buffer_dump_path(sim,out_dir,"dynamic_jn_contacts.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Jp at contacts"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Hole current density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"A m^{-2}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_jp).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_jp).x, (store->dynamic_jp).data, (store->dynamic_jp).len);
	buffer_dump_path(sim,out_dir,"dynamic_jp_contacts.dat",&buf);
	buffer_free(&buf);

	sprintf(outpath,"%s%s",out_dir,"dynamic_jn_avg.dat");
	inter_save(&(store->jn_avg),outpath);

	sprintf(outpath,"%s%s",out_dir,"dynamic_jp_avg.dat");
	inter_save(&(store->jp_avg),outpath);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("External Current"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Current"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"Amps");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->iout).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->iout).x, (store->iout).data, (store->iout).len);
	buffer_dump_path(sim,out_dir,"dynamic_i.dat",&buf);
	buffer_free(&buf);

	sprintf(outpath,"%s%s",out_dir,"dynamic_i_left.dat");
	inter_save(&(store->iout_left),outpath);

	sprintf(outpath,"%s%s",out_dir,"dynamic_i_right.dat");
	inter_save(&(store->iout_right),outpath);


	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Free carrier generation rate"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Generation rate"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->gexout).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->gexout).x, (store->gexout).data, (store->gexout).len);
	buffer_dump_path(sim,out_dir,"dynamic_gex.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Dynamic quantum efficiency"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Percent"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"\%");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_qe).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_qe).x, (store->dynamic_qe).data, (store->dynamic_qe).len);
	buffer_dump_path(sim,out_dir,"dynamic_qe.dat",&buf);
	buffer_free(&buf);


	gdouble sum=inter_intergrate(&(store->nfree_to_ptrap));
	FILE *out=fopen("dynamic_Rn_int.dat","w");
	fprintf(out,"%Le",sum);
	fclose(out);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Free hole recombination"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Recombination"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->pfree_to_ntrap).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->pfree_to_ntrap).x, (store->pfree_to_ntrap).data,(store->pfree_to_ntrap).len);
	buffer_dump_path(sim,out_dir,"dynamic_pf_to_nt.dat",&buf);
	buffer_free(&buf);


	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Free electron recombination"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Recombination"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->nfree_to_ptrap).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->nfree_to_ptrap).x, (store->nfree_to_ptrap).data,(store->nfree_to_ptrap).len);
	buffer_dump_path(sim,out_dir,"dynamic_nf_to_pt.dat",&buf);
	buffer_free(&buf);

	

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	sprintf(buf.title,"%s - %s",_("Free electron loss"),_("time"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Free electron loss"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->Rnout).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->Rnout).x, (store->Rnout).data, (store->Rnout).len);
	buffer_dump_path(sim,out_dir,"dynamic_Rn.dat",&buf);
	buffer_free(&buf);


	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	sprintf(buf.title,"%s - %s",_("Free hole loss"),_("time"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Free hole loss"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->Rpout).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->Rpout).x, (store->Rpout).data, (store->Rpout).len);
	buffer_dump_path(sim,out_dir,"dynamic_Rp.dat",&buf);
	buffer_free(&buf);

	sum=inter_intergrate(&(store->pfree_to_ntrap));
	out=fopen("dynamic_Rp_int.dat","w");
	fprintf(out,"%Le",sum);
	fclose(out);

	inter_make_cumulative(&(store->nfree_to_ptrap));
	//inter_div_gdouble(&nfree_to_ptrap,in->stark_den);
	sprintf(outpath,"%s%s",out_dir,"dynamic_Rn_cumulative.dat");
	inter_save(&(store->nfree_to_ptrap),outpath);

	inter_make_cumulative(&(store->pfree_to_ntrap));
	//inter_div_gdouble(&pfree_to_ntrap,in->stark_den);
	sprintf(outpath,"%s%s",out_dir,"dynamic_Rp_cumulative.dat");
	inter_save(&(store->pfree_to_ntrap),outpath);


	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Electron relaxation"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Relaxation"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->nrelax_out).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->nrelax_out).x, (store->nrelax_out).data, (store->nrelax_out).len);
	buffer_dump_path(sim,out_dir,"dynamic_nrelax.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	strcpy(buf.title,_("Hole relaxation"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Relaxation"));
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->prelax_out).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->prelax_out).x, (store->prelax_out).data, (store->prelax_out).len);
	buffer_dump_path(sim,out_dir,"dynamic_prelax.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Trapped electron density"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Electron density"));
	strcpy(buf.x_units,"$\\mu s$");
	strcpy(buf.data_units,"$m^{-3}$");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->ntrap).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->ntrap).x, (store->ntrap).data, (store->ntrap).len);
	buffer_dump_path(sim,out_dir,"dynamic_nt.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Trapped hole density"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Hole density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->ptrap).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->ptrap).x, (store->ptrap).data, (store->ptrap).len);
	buffer_dump_path(sim,out_dir,"dynamic_pt.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Free electron density"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Electron density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->nfree).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->nfree).x, (store->nfree).data, (store->nfree).len);
	buffer_dump_path(sim,out_dir,"dynamic_nf.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Free hole density"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Hole density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->pfree).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->pfree).x, (store->pfree).data, (store->pfree).len);
	buffer_dump_path(sim,out_dir,"dynamic_pf.dat",&buf);
	buffer_free(&buf);

	sprintf(outpath,"%s%s",out_dir,"dynamic_nfree_delta.dat");
	inter_save(&(store->nfree_delta_out),outpath);

	sprintf(outpath,"%s%s",out_dir,"dynamic_pfree_delta.dat");
	inter_save(&(store->pfree_delta_out),outpath);

	sprintf(outpath,"%s%s",out_dir,"dynamic_ntrap_delta.dat");
	inter_save(&(store->ntrap_delta_out),outpath);

	sprintf(outpath,"%s%s",out_dir,"dynamic_ptrap_delta.dat");
	inter_save(&(store->ptrap_delta_out),outpath);

	sprintf(outpath,"%s%s",out_dir,"dynamic_filledn.dat");
	inter_save(&(store->tpc_filledn),outpath);

	sprintf(outpath,"%s%s",out_dir,"dynamic_Rn-p.dat");
	inter_save(&(store->Rnpout),outpath);

	sprintf(outpath,"%s%s",out_dir,"dynamic_filledp.dat");
	inter_save(&(store->tpc_filledp),outpath);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Electron mobility"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Mobility"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{2}V^{-1}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->tpc_mue).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->tpc_mue).x, (store->tpc_mue).data,(store->tpc_mue).len);
	buffer_dump_path(sim,out_dir,"dynamic_mue.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Hole mobility"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Mobility"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{2}V^{-1}s^{-1}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->tpc_muh).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->tpc_muh).x, (store->tpc_muh).data,(store->tpc_muh).len);
	buffer_dump_path(sim,out_dir,"dynamic_muh.dat",&buf);
	buffer_free(&buf);


	sprintf(outpath,"%s%s",out_dir,"dynamic_mu_avg.dat");
	inter_save(&(store->tpc_mu_avg),outpath);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Total electron density"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Electron density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->only_n).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->only_n).x, (store->only_n).data, (store->only_n).len);
	buffer_dump_path(sim,out_dir,"dynamic_n.dat",&buf);
	buffer_free(&buf);


	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Total hole density"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Hole density"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"m^{-3}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->only_p).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->only_p).x, (store->only_p).data, (store->only_p).len);
	buffer_dump_path(sim,out_dir,"dynamic_p.dat",&buf);
	buffer_free(&buf);


	//inter_sub_gdouble(&dynamic_np,dynamic_np.data[0]);
	sprintf(outpath,"%s%s",out_dir,"dynamic_np.dat");
	inter_save(&(store->dynamic_np),outpath);

	inter_norm(&(store->dynamic_np),1.0);
	sprintf(outpath,"%s%s",out_dir,"dynamic_np_norm.dat");
	inter_save(&(store->dynamic_np),outpath);


	sprintf(outpath,"%s%s",out_dir,"dynamic_E_field.dat");
	inter_div_gdouble(&(store->E_field),(store->E_field).data[0]);
	inter_save(&(store->E_field),outpath);


	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e6;
	strcpy(buf.title,_("Voltage applied to diode"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("Voltage"));
	strcpy(buf.x_units,"\\mu s");
	strcpy(buf.data_units,"V");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=(store->dynamic_Vapplied).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_Vapplied).x, (store->dynamic_Vapplied).data, (store->dynamic_Vapplied).len);
	buffer_dump_path(sim,out_dir,"dynamic_Vapplied.dat",&buf);
	buffer_free(&buf);


	sprintf(outpath,"%s%s",out_dir,"dynamic_charge_tot.dat");
	inter_sub_gdouble(&(store->dynamic_charge_tot),(store->dynamic_charge_tot).data[0]);
	inter_save(&(store->dynamic_charge_tot),outpath);

	inter_chop(&(store->dynamic_pl),1.0e-9, 1.0);
	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	strcpy(buf.title,_("PL intensity"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("PL Intensity"));
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"au");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->dynamic_pl).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_pl).x, (store->dynamic_pl).data, (store->dynamic_pl).len);
	buffer_dump_path(sim,out_dir,"dynamic_pl.dat",&buf);
	buffer_free(&buf);

	gdouble max=inter_get_max(&(store->dynamic_pl));
	inter_div_gdouble(&(store->dynamic_pl),max);
	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	strcpy(buf.title,_("PL intensity normalized"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("PL Intensity"));
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"au");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->dynamic_pl).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->dynamic_pl).x, (store->dynamic_pl).data, (store->dynamic_pl).len);
	buffer_dump_path(sim,out_dir,"dynamic_pl_norm.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	sprintf(buf.title,"%s - %s",_("time"),"srh_n_r1");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,"srh_n_r1");
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"m^{-3} s^{-1}");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->srh_n_r1).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->srh_n_r1).x, (store->srh_n_r1).data, (store->srh_n_r1).len);
	buffer_dump_path(sim,out_dir,"dynamic_srh_n_r1.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	sprintf(buf.title,"%s - %s",_("time"),"srh_n_r2");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,"srh_n_r2");
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->srh_n_r2).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->srh_n_r2).x, (store->srh_n_r2).data, (store->srh_n_r2).len);
	buffer_dump_path(sim,out_dir,"dynamic_srh_n_r2.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	sprintf(buf.title,"%s - %s",_("time"),"srh_n_r3");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,"srh_n_r3");
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->srh_n_r3).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->srh_n_r3).x, (store->srh_n_r3).data, (store->srh_n_r3).len);
	buffer_dump_path(sim,out_dir,"dynamic_srh_n_r3.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	sprintf(buf.title,"%s %s",_("time"),"srh_n_r4");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,"srh_n_r4");
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->srh_n_r4).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->srh_n_r4).x, (store->srh_n_r4).data, (store->srh_n_r4).len);
	buffer_dump_path(sim,out_dir,"dynamic_srh_n_r4.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	sprintf(buf.title,"%s %s",_("time"),"srh_p_r1");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,"srh_p_r1");
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->srh_p_r1).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->srh_p_r1).x, (store->srh_p_r1).data, (store->srh_p_r1).len);
	buffer_dump_path(sim,out_dir,"dynamic_srh_p_r1.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	sprintf(buf.title,"%s - %s",_("time"),"srh_p_r2");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,"srh_p_r2");
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->srh_p_r2).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->srh_p_r2).x, (store->srh_p_r2).data, (store->srh_p_r2).len);
	buffer_dump_path(sim,out_dir,"dynamic_srh_p_r2.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	sprintf(buf.title,"%s - %s",_("time"),"srh_p_r3");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,"srh_p_r3");
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->srh_p_r3).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->srh_p_r3).x, (store->srh_p_r3).data, (store->srh_p_r3).len);
	buffer_dump_path(sim,out_dir,"dynamic_srh_p_r3.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	sprintf(buf.title,"%s - %s",_("time"),"srh_p_r4");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,"srh_p_r4");
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,"m^{-3}s^{-1}");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->srh_p_r4).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->srh_p_r4).x, (store->srh_p_r4).data, (store->srh_p_r4).len);
	buffer_dump_path(sim,out_dir,"dynamic_srh_p_r4.dat",&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	sprintf(buf.title,"%s %s",_("time"),_("band bend (percent)"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Time"));
	strcpy(buf.data_label,_("band bend"));
	strcpy(buf.x_units,"s");
	strcpy(buf.data_units,_("percent"));
	buf.logscale_x=1;
	buf.logscale_y=1;
	buf.x=1;
	buf.y=(store->band_bend).len;
	buf.z=1;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,(store->band_bend).x, (store->band_bend).data, (store->band_bend).len);
	buffer_dump_path(sim,out_dir,"dynamic_band_bend.dat",&buf);
	buffer_free(&buf);
}

}

void dump_dynamic_add_data(struct simulation *sim,struct dynamic_store *store,struct device *in, gdouble x_value)
{
int x=0;
int y=0;
int z=0;
gdouble Vapplied=0.0;

if (get_dump_status(sim,dump_dynamic)==TRUE)
{

	inter_append(&(store->jn_avg),x_value,get_jn_avg(in));
	inter_append(&(store->jp_avg),x_value,get_jp_avg(in));

	inter_append(&(store->dynamic_jn),x_value,(in->Jnleft[0][0]+in->Jnright[0][0])/2.0);
	inter_append(&(store->dynamic_jp),x_value,(in->Jpleft[0][0]+in->Jpright[0][0])/2.0);

	inter_append(&(store->charge_change),x_value,get_charge_change(in));
	inter_append(&(store->jout),x_value,get_J(in));

	inter_append(&(store->dynamic_jn_drift),x_value,get_Jn_drift(in));
	inter_append(&(store->dynamic_jn_diffusion),x_value,get_Jn_diffusion(in));

	inter_append(&(store->dynamic_jp_drift),x_value,get_Jp_drift(in));
	inter_append(&(store->dynamic_jp_diffusion),x_value,get_Jp_diffusion(in));

	inter_append(&(store->jnout_mid),x_value,in->Jn[in->zmeshpoints/2][in->xmeshpoints/2][in->ymeshpoints/2]);

	inter_append(&(store->jpout_mid),x_value,in->Jp[in->zmeshpoints/2][in->xmeshpoints/2][in->ymeshpoints/2]);

	inter_append(&(store->iout),x_value,get_equiv_I(sim,in));

	inter_append(&(store->iout_left),x_value,(get_J_left(in))*(in->xlen*in->zlen)/2.0);

	inter_append(&(store->iout_right),x_value,(get_J_right(in))*(in->xlen*in->zlen)/2.0);

	inter_append(&(store->gexout),x_value,in->Gn[0][0][0]);

	inter_append(&(store->nfree_to_ptrap),x_value,get_avg_recom_n(in));
	inter_append(&(store->pfree_to_ntrap),x_value,get_avg_recom_p(in));

	inter_append(&(store->Rnout),x_value,get_avg_Rn(in));
	inter_append(&(store->Rpout),x_value,get_avg_Rp(in));

	inter_append(&(store->Rnpout),x_value,get_avg_recom_n(in)-get_avg_recom_p(in));

	inter_append(&(store->nrelax_out),x_value,get_avg_relax_n(in));
	inter_append(&(store->prelax_out),x_value,get_avg_relax_p(in));


	inter_append(&(store->ntrap),x_value,get_n_trapped_charge(in));

	long double val=(in->phi[0][0][(in->ymeshpoints/2)+1]-in->phi[0][0][(in->ymeshpoints/2)])/(in->ymesh[(in->ymeshpoints/2)+1]-in->ymesh[(in->ymeshpoints/2)]);
	inter_append(&(store->E_field),x_value,val);

	Vapplied=contact_get_voltage(sim,in,0);
	inter_append(&(store->dynamic_Vapplied),x_value,Vapplied);

	inter_append(&(store->dynamic_charge_tot),x_value,get_charge_tot(in));

	inter_append(&(store->dynamic_pl),x_value,in->pl_intensity);

	inter_append(&(store->ptrap),x_value,get_p_trapped_charge(in));

	inter_append(&(store->ntrap_delta_out),x_value,get_n_trapped_charge_delta(in));

	inter_append(&(store->ptrap_delta_out),x_value,get_p_trapped_charge_delta(in));

	inter_append(&(store->nfree),x_value,get_free_n_charge(in));

	inter_append(&(store->pfree),x_value,get_free_p_charge(in));

	inter_append(&(store->nfree_delta_out),x_value,get_free_n_charge_delta(in));

	inter_append(&(store->pfree_delta_out),x_value,get_free_p_charge_delta(in));


	inter_append(&(store->tpc_mue),x_value,fabs(get_avg_mue(in)));
	inter_append(&(store->tpc_muh),x_value,fabs(get_avg_muh(in)));

	inter_append(&(store->tpc_mu_avg),x_value,(fabs(get_avg_mue(in))+fabs(get_avg_muh(in)))/2.0);

	inter_append(&(store->tpc_filledn),x_value,get_dos_filled_n(in));
	inter_append(&(store->tpc_filledp),x_value,get_dos_filled_p(in));

	inter_append(&(store->dynamic_np),x_value,get_extracted_np(in));
	inter_append(&(store->only_n),x_value,get_extracted_n(in)/2.0);
	inter_append(&(store->only_p),x_value,get_extracted_p(in)/2.0);

	inter_append(&(store->dynamic_qe),x_value,-100.0*(get_J(in)/in->ylen)/get_avg_gen(in)/Q);

	int i;
	int band;
	gdouble srh_n_r1=0.0;
	gdouble srh_n_r2=0.0;
	gdouble srh_n_r3=0.0;
	gdouble srh_n_r4=0.0;
	gdouble srh_p_r1=0.0;
	gdouble srh_p_r2=0.0;
	gdouble srh_p_r3=0.0;
	gdouble srh_p_r4=0.0;

	for (z=0;z<in->zmeshpoints;z++)
	{
		for (x=0;x<in->xmeshpoints;x++)
		{
			for (y=0;y<in->ymeshpoints;y++)
			{
				for (band=0;band<in->srh_bands;band++)
				{
					srh_n_r1+=in->n[z][x][y]*in->srh_n_r1[z][x][y][band];
					srh_n_r2+=in->srh_n_r2[z][x][y][band];
					srh_n_r3+=in->p[z][x][y]*in->srh_n_r3[z][x][y][band];
					srh_n_r4+=in->srh_n_r4[z][x][y][band];

					srh_p_r1+=in->p[z][x][y]*in->srh_p_r1[z][x][y][band];
					srh_p_r2+=in->srh_p_r2[z][x][y][band];
					srh_p_r3+=in->n[z][x][y]*in->srh_p_r3[z][x][y][band];
					srh_p_r4+=in->srh_p_r4[z][x][y][band];
				}
			}
		}
	}

	inter_append(&(store->srh_n_r1),x_value,srh_n_r1/((gdouble)(in->ymeshpoints*in->srh_bands)));
	inter_append(&(store->srh_n_r2),x_value,srh_n_r2/((gdouble)(in->ymeshpoints*in->srh_bands)));
	inter_append(&(store->srh_n_r3),x_value,srh_n_r3/((gdouble)(in->ymeshpoints*in->srh_bands)));
	inter_append(&(store->srh_n_r4),x_value,srh_n_r4/((gdouble)(in->ymeshpoints*in->srh_bands)));

	inter_append(&(store->srh_p_r1),x_value,srh_p_r1/((gdouble)(in->ymeshpoints*in->srh_bands)));
	inter_append(&(store->srh_p_r2),x_value,srh_p_r2/((gdouble)(in->ymeshpoints*in->srh_bands)));
	inter_append(&(store->srh_p_r3),x_value,srh_p_r3/((gdouble)(in->ymeshpoints*in->srh_bands)));
	inter_append(&(store->srh_p_r4),x_value,srh_p_r4/((gdouble)(in->ymeshpoints*in->srh_bands)));

	gdouble tot=0.0;

	for (z=0;z<in->zmeshpoints;z++)
	{
		for (x=0;x<in->xmeshpoints;x++)
		{
			for (y=0;y<in->ymeshpoints;y++)
			{
				tot+=fabs(store->band_snapshot[z][x][y]-in->phi[z][x][y])/fabs(in->phi[z][x][y]);
			}
		}
	}

	tot/=(gdouble)(in->ymeshpoints);
	inter_append(&(store->band_bend),x_value,tot);

}

}

void dump_dynamic_free(struct simulation *sim,struct device *in,struct dynamic_store *store)
{
if (get_dump_status(sim,dump_dynamic)==TRUE)
{
	inter_free(&(store->charge_change));
	inter_free(&(store->jout));
	inter_free(&(store->jn_avg));
	inter_free(&(store->jp_avg));
	inter_free(&(store->dynamic_jn));
	inter_free(&(store->dynamic_jp));
	inter_free(&(store->jnout_mid));
	inter_free(&(store->jpout_mid));
	inter_free(&(store->iout));
	inter_free(&(store->iout_left));
	inter_free(&(store->iout_right));
	inter_free(&(store->gexout));
	inter_free(&(store->nfree_to_ptrap));
	inter_free(&(store->pfree_to_ntrap));
	inter_free(&(store->Rnout));
	inter_free(&(store->Rpout));
	inter_free(&(store->nrelax_out));
	inter_free(&(store->prelax_out));
	inter_free(&(store->ntrap));
	inter_free(&(store->ptrap));
	inter_free(&(store->ntrap_delta_out));
	inter_free(&(store->ptrap_delta_out));
	inter_free(&(store->nfree));
	inter_free(&(store->pfree));
	inter_free(&(store->nfree_delta_out));
	inter_free(&(store->pfree_delta_out));
	inter_free(&(store->tpc_filledn));
	inter_free(&(store->Rnpout));
	inter_free(&(store->tpc_filledp));
	inter_free(&(store->tpc_mue));
	inter_free(&(store->tpc_muh));
	inter_free(&(store->tpc_mu_avg));
	inter_free(&(store->only_n));
	inter_free(&(store->only_p));
	inter_free(&(store->dynamic_np));
	inter_free(&(store->E_field));
	inter_free(&(store->dynamic_Vapplied));
	inter_free(&(store->dynamic_charge_tot));
	inter_free(&(store->dynamic_pl));
	inter_free(&(store->dynamic_jn_drift));
	inter_free(&(store->dynamic_jn_diffusion));
	inter_free(&(store->dynamic_jp_drift));
	inter_free(&(store->dynamic_jp_diffusion));
	inter_free(&(store->dynamic_qe));

	inter_free(&(store->srh_n_r1));
	inter_free(&(store->srh_n_r2));
	inter_free(&(store->srh_n_r3));
	inter_free(&(store->srh_n_r4));

	inter_free(&(store->srh_p_r1));
	inter_free(&(store->srh_p_r2));
	inter_free(&(store->srh_p_r3));
	inter_free(&(store->srh_p_r4));

	inter_free(&(store->band_bend));

	free_3d_gdouble(in,store->band_snapshot);
}
}

