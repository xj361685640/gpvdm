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

/** @file fdtd_dump.c
	@brief Dumps the fdtd fields.
*/

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
#include <inp.h>
#include <sim.h>
#include <log.h>
#include <fdtd.h>

#include "vec.h"

void fdtd_dump(struct simulation *sim,struct fdtd_data *data)
{
	int z=0;
	int x=0;
	int y=0;
	FILE *out;

	out=fopen("./Ex.dat","w");
	for (z=0;z<data->zlen;z++)
	{
		for (x=0;x<data->xlen;x++)
		{

			for (y=0;y<data->ylen;y++)
			{
				fprintf(out,"%le %le %le\n",data->x_mesh[x],data->y_mesh[y],data->Ex[z][x][y]);
			}

			fprintf(out,"\n");
		}
	
	}
	fclose(out);

	out=fopen("./epsilonr.dat","w");
	for (z=0;z<data->zlen;z++)
	{
		for (x=0;x<data->xlen;x++)
		{

			for (y=0;y<data->ylen;y++)
			{
				fprintf(out,"%le %le %le\n",data->x_mesh[x],data->y_mesh[y],data->epsilon_r[z][x][y]);
			}
		
			fprintf(out,"\n");
		}


	}

	fclose(out);
	
}
