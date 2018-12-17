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
#include <inp.h>
#include <sim.h>
#include <log.h>
#include <fdtd.h>

#include "vec.h"


void solve_E(struct simulation *sim,struct fdtd_data *data)
{
	int z=0;
	int y=0;
	//float Cx=(data->dt2/(epsilon0f*data->dx));
	float Cy=(data->dt2/(epsilon0f*data->dy));
	float Cz=(data->dt2/(epsilon0f*data->dz));

	for (y=1;y<data->ylen-1;y++)
	{
		for (z=1;z<(data->zlen-1);z++)
		{
			data->Ex[z][y]=data->Ex_last[z][y]+(data->Hz[z][y]-data->Hz[z][y-1])*data->epsilon_r[z][y]*Cy-(data->Hy[z][y]-data->Hy[z-1][y])*data->epsilon_r[z][y]*Cz;
			data->Ey[z][y]=data->Ey_last[z][y]+(data->Hx[z][y]-data->Hx[z-1][y])*data->epsilon_r[z][y]*Cz;
			data->Ez[z][y]=data->Ez_last[z][y]-(data->Hx[z][y]-data->Hx[z][y-1])*data->epsilon_r[z][y]*Cy;
			if (isnan(data->Ex[z][y])||isinf(data->Ex[z][y]))
			{
				printf("nan = %f %f %e %e %e\n",data->Ex[z][y],data->Ex_last[z][y],data->Hz[z][y],data->Hz[z][y-1],data->epsilon_r[z][y]*Cy);
				exit(0);
			}
		}
	}

}



void solve_H(struct simulation *sim,struct fdtd_data *data)
{

//int thread;
int i;
int j;
float Cy=(data->dt2/(mu0f*data->dy));
//float Cx=(data->dt2/(mu0f*data->dx));
float Cz=(data->dt2/(mu0f*data->dy));

for (j=0;j<(data->ylen-1);j++)
{
	for (i=0;i<(data->zlen-1);i++)
	{
		data->Hx[i][j]=data->Hx_last[i][j]-(data->Ez[i][j+1]-data->Ez[i][j])*Cy+(data->Ey[i+1][j]-data->Ey[i][j])*Cz;
		data->Hy[i][j]=data->Hy_last[i][j]-(data->Ex[i+1][j]-data->Ex[i][j])*Cz;
		data->Hz[i][j]=data->Hz_last[i][j]+(data->Ex[i][j+1]-data->Ex[i][j])*Cy;
	}
}

}

void fdtd_excitation(struct simulation *sim,struct fdtd_data *data)
{
	int j=0;
	int i=20;

	j=data->ylen-20;
	double start=data->z_mesh[1];
	double stop=data->z_mesh[data->zlen-2];

	//double stop=data->y_mesh[data->ylen-2];

	for (i=data->zlen*0.5;i<data->zlen*0.5+data->zlen*0.1;i++)
	{

		//if ((data->y_mesh[j]>2e-9)&&(data->y_mesh[j]<4e-9))
		//if (j==5)
		{
			double a=0.0;
			double b=0.0;
			double c=0.0;
			double phi=0.0;
			double theta=0.0;


			double dot=0.0;
			phi=45.0;
			theta=45.0;
			double shift=0.0;

			a=sin(theta*(2.0*M_PI/360.0))*cos(phi*(2.0*M_PI/360.0));
			b=sin(theta*(2.0*M_PI/360.0))*sin(phi*(2.0*M_PI/360.0));
			c=cos(theta*(2.0*M_PI/360.0));
			//printf(" a=%lf b=%lf c=%lf\n",a,b,c);
			//getchar();
			dot=tan(2.0*M_PI*(shift)/360.0)*(data->z_mesh[i]-start)*2.0*M_PI/data->lambda;
			//dot=tan(2.0*M_PI*(shift)/360.0)*(data->y_mesh[j]-start)*2.0*M_PI/data->lambda;
			double mod=1.0;
			//dot=0.0;
			data->Ex[i][j]=mod*a*sin(dot-data->time*data->omega);
			data->Ey[i][j]=mod*b*sin(dot-data->time*data->omega);
			data->Ez[i][j]=mod*c*sin(dot-data->time*data->omega);
			//printf("%f %e\n",data->Ex[i][j],data->time);
		}
		//printf("%lf %lf %lf %lf\n",sin(dot),a,b,c);
		//Hy[i][j]=cos(((double)step)/(2.0*3.14159*10.0));s
	}
}

void fdtd_time_step(struct simulation *sim,struct fdtd_data *data)
{
int i;
int j;

	for (i=0;i<data->zlen;i++)
	{

		memcpy ( data->Ex_last[i], data->Ex[i], sizeof(double)*data->ylen );
		memcpy ( data->Ey_last[i], data->Ey[i], sizeof(double)*data->ylen );
		memcpy ( data->Ez_last[i], data->Ez[i], sizeof(double)*data->ylen );
	}

	for (i=0;i<data->zlen;i++)
	{

		memcpy ( data->Hx_last[i], data->Hx[i], sizeof(double)*data->ylen );
		memcpy ( data->Hy_last[i], data->Hy[i], sizeof(double)*data->ylen );
		memcpy ( data->Hz_last[i], data->Hz[i], sizeof(double)*data->ylen );
	}

	data->time+=data->dt;
}
void fdtd_solve_step(struct simulation *sim,struct fdtd_data *data)
{
	//fdtd_opencl_solve_step(sim,data);

	solve_E(sim,data);
	fdtd_excitation(sim,data);
	solve_H(sim,data);


	fdtd_time_step(sim,data);
	//usleep(100);
}


