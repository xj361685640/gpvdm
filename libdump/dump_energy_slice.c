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


#include <string.h>
#include <stdio.h>
#include <dos.h>
#include "sim.h"
#include "dump.h"
#include "buffer.h"

void dump_energy_slice(struct simulation *sim,char *out_dir,struct device *in)
{
struct buffer buf;
buffer_init(&buf);
char name[200];
int band=0;
int i=in->dump_slicepos;

char outpath[200];

struct istruct dump1;
inter_init(&dump1);

struct istruct dump2;
inter_init(&dump2);

struct istruct dump3;
inter_init(&dump3);

struct istruct dump4;
inter_init(&dump4);

struct istruct dump5;
inter_init(&dump5);

struct istruct dump6;
inter_init(&dump6);

struct istruct dump9;
inter_init(&dump9);

struct istruct dump10;
inter_init(&dump10);

struct istruct dump_nt;
inter_init(&dump_nt);

struct istruct dump_pt;
inter_init(&dump_pt);

int mat=in->imat[0][0][in->ymeshpoints/2];
for (band=0;band<in->srh_bands;band++)
{
	inter_append(&dump1,get_dos_E_n(in,band,mat),in->n[0][0][i]*in->srh_n_r1[0][0][i][band]-in->srh_n_r2[0][0][i][band]);
	inter_append(&dump2,get_dos_E_p(in,band,mat),in->p[0][0][i]*in->srh_p_r1[0][0][i][band]-in->srh_p_r2[0][0][i][band]);
	inter_append(&dump3,get_dos_E_n(in,band,mat),in->nt[0][0][i][band]-in->ntb_save[0][0][i][band]);
	inter_append(&dump4,get_dos_E_p(in,band,mat),in->pt[0][0][i][band]-in->ptb_save[0][0][i][band]);
	inter_append(&dump5,get_dos_E_n(in,band,mat),in->p[0][0][i]*in->srh_n_r3[0][0][i][band]-in->srh_n_r4[0][0][i][band]);
	inter_append(&dump6,get_dos_E_p(in,band,mat),in->n[0][0][i]*in->srh_p_r3[0][0][i][band]-in->srh_p_r4[0][0][i][band]);
	inter_append(&dump9,get_dos_E_n(in,band,mat),in->Fnt[0][0][i][band]);
	inter_append(&dump10,get_dos_E_p(in,band,mat),in->Fpt[0][0][i][band]);
	inter_append(&dump_nt,get_dos_E_n(in,band,mat),in->nt[0][0][i][band]);
	inter_append(&dump_pt,get_dos_E_p(in,band,mat),in->pt[0][0][i][band]);

}

sprintf(outpath,"%senergy_slice_nt_cap_%s",out_dir,".dat");
//inter_save(&dump1,outpath);
inter_free(&dump1);

sprintf(outpath,"%senergy_slice_pt_cap_%s",out_dir,".dat");
//inter_save(&dump2,outpath);
inter_free(&dump2);


sprintf(outpath,"%senergy_slice_nt_delta_%s",out_dir,".dat");
//inter_save(&dump3,outpath);
inter_free(&dump3);

sprintf(outpath,"%senergy_slice_pt_delta_%s",out_dir,".dat");
//inter_save(&dump4,outpath);
inter_free(&dump4);


sprintf(outpath,"%senergy_slice_nt_recom_%s",out_dir,".dat");
//inter_save(&dump5,outpath);
inter_free(&dump5);

sprintf(outpath,"%senergy_slice_pt_recom_%s",out_dir,".dat");
//inter_save(&dump6,outpath);
inter_free(&dump6);

sprintf(outpath,"%senergy_slice_fn_%s",out_dir,".dat");
//inter_save(&dump9,outpath);
inter_free(&dump9);

sprintf(outpath,"%senergy_slice_fp_%s",out_dir,".dat");
//inter_save(&dump10,outpath);
inter_free(&dump10);

buffer_malloc(&buf);
sprintf(name,"energy_slice_nt.dat");
buf.y_mul=1.0;
buf.x_mul=1.0;
strcpy(buf.title,"Energy - trap ocupation");
strcpy(buf.type,"xy");
strcpy(buf.x_label,"Energy");
strcpy(buf.y_label,"Ocupation");
strcpy(buf.x_units,"eV");
strcpy(buf.y_units,"m^{-3} eV^{-1}");
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,dump_nt.x, dump_nt.data, dump_nt.len);
buffer_dump_path(sim,out_dir,name,&buf);
buffer_free(&buf);
inter_free(&dump_nt);

buffer_malloc(&buf);
sprintf(name,"energy_slice_pt.dat");
buf.y_mul=1.0;
buf.x_mul=1.0;
strcpy(buf.title,"Energy - trap ocupation");
strcpy(buf.type,"xy");
strcpy(buf.x_label,"Energy");
strcpy(buf.y_label,"Ocupation");
strcpy(buf.x_units,"eV");
strcpy(buf.y_units,"m^{-3} eV^{-1}");
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,dump_pt.x, dump_pt.data, dump_pt.len);
buffer_dump_path(sim,out_dir,name,&buf);
buffer_free(&buf);
inter_free(&dump_pt);

}
