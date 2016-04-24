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


#include <stdio.h>
#include <exp.h>
#include "util.h"
#include "sim.h"
#include "dos.h"
#include "dump.h"
#include "complex_solver.h"
#include "log.h"
#include <cal_path.h>

void find_n0(struct simulation *sim,struct device *in)
{
int i;
printf_log(sim,"Finding n0\n");
gdouble oldsun=light_get_sun(&(in->mylight));
gdouble oldv=in->Vapplied;
in->Vapplied=0;
char temp[200];

light_set_sun(&(in->mylight),0);

light_solve_and_update(sim,in,&(in->mylight),0.0);

if (get_dump_status(sim,dump_equilibrium)==TRUE)
{
	join_path(2,temp,get_output_path(sim),"equilibrium");
	dump_1d_slice(sim,in,temp);
}


for (i=0;i<in->ymeshpoints;i++)
{
	in->B[i]=0.0;
}

gdouble save_clamp=in->electrical_clamp;
int save_ittr=in->max_electrical_itt;
gdouble save_electricalerror=in->min_cur_error;

in->electrical_clamp=in->electrical_clamp0;
in->max_electrical_itt=in->max_electrical_itt0;
in->min_cur_error=in->electrical_error0;

solve_all(sim,in);



in->max_electrical_itt=save_ittr;
in->electrical_clamp=save_clamp;
in->min_cur_error=save_electricalerror;

solve_all(sim,in);

for (i=0;i<in->ymeshpoints;i++)
{
	in->B[i]=get_dos_B(in,in->imat[i]);
}

reset_np_save(in);
reset_npequlib(in);

FILE *outfile;
outfile=fopena(get_output_path(sim),"voc_mue.dat","w");
fprintf(outfile,"%Le",get_avg_mue(in));
fclose(outfile);

outfile=fopena(get_output_path(sim),"voc_muh.dat","w");
fprintf(outfile,"%Le",get_avg_muh(in));
fclose(outfile);


light_set_sun(&(in->mylight),oldsun);
in->Vapplied=oldv;
light_solve_and_update(sim,in,&(in->mylight),0.0);
printf_log(sim,"Exit finding n0\n");
}
