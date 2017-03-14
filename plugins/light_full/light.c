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


#include <stdio.h>
#include <stdlib.h>
#include <util.h>
#include <dump_ctrl.h>
#include <complex_solver.h>
#include <const.h>
#include <light.h>

#include <device.h>
#include <light_interface.h>

#include <functions.h>
#include <log.h>

//static gdouble min_light_error=1e-10;

EXPORT void light_dll_ver(struct simulation *sim)
{
        printf_log(sim,"%s\n","Full transfer matrix based light model");
}


EXPORT int light_dll_solve_lam_slice(struct simulation *sim,struct light *in,int lam)
{
if (in->sun_E[lam]==0.0)
{
	return 0;
}
if (get_dump_status(sim,dump_optics)==TRUE)
{
	char one[100];
	sprintf(one,"%s %Lf nm\n","Solve optical slice at",in->l[lam]*1e9);
	waveprint(sim,one,in->l[lam]*1e9);
}
int i;

gdouble complex Epl=0.0+0.0*I;
//gdouble complex Epc=0.0+0.0*I;
//gdouble complex Epr=0.0+0.0*I;

//gdouble complex Enl=0.0+0.0*I;
//gdouble complex Enc=0.0+0.0*I;
//gdouble complex Enr=0.0+0.0*I;

gdouble complex rc=0.0+0.0*I;
gdouble complex tc=0.0+0.0*I;
gdouble complex rr=0.0+0.0*I;
gdouble complex tr=0.0+0.0*I;

gdouble complex nbar_c=0.0;
gdouble complex xi_c=0.0;

gdouble complex nbar_r=0.0;
gdouble complex xi_r=0.0;

gdouble complex rl=0.0+0.0*I;
gdouble complex tl=0.0+0.0*I;
gdouble complex nbar_l=0.0;
gdouble complex xi_l=0.0;

gdouble complex fp=0.0;
gdouble complex fn=0.0;

int ittr=0;
int pos=0;
//gdouble error=1000;
gdouble l=in->l[lam];

//gdouble nc;

gdouble tot_error=0.0;
int quit=FALSE;
gdouble test=FALSE;
gdouble dx=in->x[2]-in->x[1];

do
{

	pos=0;

	for (i=0;i<in->points;i++)
	{

		rc=in->r[lam][i];
		tc=in->t[lam][i];



		if (i==0)
		{
			rl=in->r[lam][i];
			tl=in->t[lam][i];

			nbar_l=in->nbar[lam][i];
			Epl=in->sun_E[lam]+0.0*I;

		}else
		{
			rl=in->r[lam][i-1];
			tl=in->t[lam][i-1];

			nbar_l=in->nbar[lam][i-1];
			Epl=0.0;
		}

		if (i==in->points-1)
		{
			rr=in->r[lam][i];
			tr=in->t[lam][i];
			nbar_r=in->nbar[lam][i];
		}else
		{
			rr=in->r[lam][i+1];
			tr=in->t[lam][i+1];
			nbar_r=in->nbar[lam][i+1];
		}


		nbar_c=in->nbar[lam][i];

		xi_l=((2*PI)/l)*nbar_l;
		xi_c=((2*PI)/l)*nbar_c;
		xi_r=((2*PI)/l)*nbar_r;

		gdouble complex pa=-tl;
		gdouble complex pbp=cexp(xi_c*dx*I);

		gdouble complex pbn=rl*cexp(-xi_c*dx*I);

		gdouble complex na=-tc;//Enc
		gdouble complex nbp=rc*cexp(xi_r*dx*I);//Enr*
		gdouble complex nbn=cexp(-xi_r*dx*I);


		//getchar();
		fp=0.0+0.0*I;
		fn=0.0+0.0*I;

		if (i==0)
		{
			fp=Epl;
			pa=0.0+0.0*I;
		}else
		{
			fp=0.0+0.0*I;
		}




		if (i!=0)
		{
			in->Ti[pos]=i;
			in->Tj[pos]=i-1;
			in->Tx[pos]=(double)gcreal(pa);
			in->Txz[pos]=(double)gcimag(pa);
			pos++;
		}

		in->Ti[pos]=i;
		in->Tj[pos]=i;
		in->Tx[pos]=(double)gcreal(pbp);
		in->Txz[pos]=(double)gcimag(pbp);
		pos++;

		in->Ti[pos]=i;
		in->Tj[pos]=in->points+i;
		in->Tx[pos]=(double)gcreal(pbn);
		in->Txz[pos]=(double)gcimag(pbn);
		pos++;


		if (i!=in->points-1)
		{
			in->Ti[pos]=in->points+i;
			in->Tj[pos]=i+1;
			in->Tx[pos]=(double)gcreal(nbp);
			in->Txz[pos]=(double)gcimag(nbp);
			pos++;
		}

		in->Ti[pos]=in->points+i;
		in->Tj[pos]=in->points+i;
		in->Tx[pos]=(double)gcreal(na);
		in->Txz[pos]=(double)gcimag(na);
		pos++;

		if (i!=in->points-1)
		{
			in->Ti[pos]=in->points+i;
			in->Tj[pos]=in->points+i+1;
			in->Tx[pos]=(double)gcreal(nbn);
			in->Txz[pos]=(double)gcimag(nbn);
			pos++;
		}

		in->b[i]=(double)gcreal(fp);
		in->bz[i]=(double)gcimag(fp);

		in->b[in->points+i]=(double)gcreal(fn);
		in->bz[in->points+i]=(double)gcimag(fn);


	}

	//complex_solver_print(sim,in->M,in->N,in->Ti,in->Tj, in->Tx, in->Txz,in->b,in->bz);
	//complex_solver_dump_matrix(in->M,in->N,in->Ti,in->Tj, in->Tx, in->Txz,in->b,in->bz);

	if (pos!=in->N)
	{
		printf_log(sim,"not solving because number does not match%d %d\n",pos,in->N);
		exit(0);
	}

	complex_solver(sim,in->M,in->N,in->Ti,in->Tj, in->Tx, in->Txz,in->b,in->bz);

	for (i=0;i<in->points;i++)
	{
		gdouble update;

		update=in->b[i];
		in->Ep[lam][i]=(gdouble)update;

		update=in->bz[i];
		in->Epz[lam][i]=(gdouble)update;

		update=in->b[in->points+i];
		in->En[lam][i]=(gdouble)update;

		update=in->bz[in->points+i];
		in->Enz[lam][i]=(gdouble)update;
	}

	ittr++;

if (test==TRUE)
{
	if (ittr>1) quit=TRUE;
	getchar();
}else
{
	quit=TRUE;
}
}while(quit==FALSE);

if (test==TRUE)
{
	printf_log(sim,"Error in optical model tot error=%Le\n",tot_error);
}

//getchar();
return 0;
}


