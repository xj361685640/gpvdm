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

#include <math.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <inp.h>
#include <sim.h>
#include <log.h>
#include <fdtd.h>

#include "vec.h"

void dump_all(struct simulation *sim,float* y, float* z)
{
int i;
int j;
printf_log(sim,"Dumping\n");
FILE *out=fopen("./Ex_final.dat","w");

for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		printf_log(out,"%le %le %le\n",z[i],y[j],Ex[i][j]);
	}

printf_log(out,"\n");
}
fclose(out);
printf_log(sim,"Dumping\n");
out=fopen("./Ey_final.dat","w");

for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		printf_log(out,"%le %le %le\n",z[i],y[j],Ey[i][j]);
	}

printf_log(out,"\n");
}
fclose(out);
printf_log(sim,"Dumping\n");
out=fopen("./Ez_final.dat","w");
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		printf_log(out,"%le %le %le\n",z[i],y[j],Ex[i][j]);
	}

printf_log(out,"\n");
}
fclose(out);
printf_log(sim,"Dumping\n");
out=fopen("./epsilon_r_final.dat","w");

for (j=0;j<ylen;j++)
{

	for (i=0;i<zlen;i++)
	{
		printf_log(out,"%le %le %le\n",z[i],y[j],epsilon_r[i][j]);
	}

printf_log(out,"\n");
}
fclose(out);
}
