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



#include "sim.h"
#include "dump.h"
#include <dos.h>

void update_y_array(struct simulation *sim,struct device *in,int z,int x)
{
int y=0;
int band=0;

	for (y=0;y<in->ymeshpoints;y++)
	{
		in->Fn[z][x][y]=in->x[z][x][y]-in->phi[z][x][y];
		in->Fp[z][x][y]= -in->xp[z][x][y]-in->phi[z][x][y];

		in->Ec[z][x][y]= -in->phi[z][x][y]-in->Xi[z][x][y];
		in->Ev[z][x][y]= -in->phi[z][x][y]-in->Xi[z][x][y]-in->Eg[z][x][y];

		in->dn[z][x][y]=get_dn_den(in,in->x[z][x][y]+in->t[z][x][y],in->Te[z][x][y],in->imat[z][x][y]);
		in->n[z][x][y]=get_n_den(in,in->x[z][x][y]+in->t[z][x][y],in->Te[z][x][y],in->imat[z][x][y]);
		in->dndphi[z][x][y]=get_dn_den(in,in->x[z][x][y]+in->t[z][x][y],in->Te[z][x][y],in->imat[z][x][y]);
		in->dp[z][x][y]=get_dp_den(in,in->xp[z][x][y]-in->tp[z][x][y],in->Th[z][x][y],in->imat[z][x][y]);
		in->p[z][x][y]=get_p_den(in,in->xp[z][x][y]-in->tp[z][x][y],in->Th[z][x][y],in->imat[z][x][y]);
		in->dpdphi[z][x][y]= -get_dp_den(in,in->xp[z][x][y]-in->tp[z][x][y],in->Th[z][x][y],in->imat[z][x][y]);
//printf("%d one %d\n",i,in->imat[y]);

		in->wn[z][x][y]=get_n_w(in,in->x[z][x][y]+in->t[z][x][y],in->Te[z][x][y],in->imat[z][x][y]);
		in->wp[z][x][y]=get_p_w(in,in->xp[z][x][y]-in->tp[z][x][y],in->Th[z][x][y],in->imat[z][x][y]);

		in->mun[z][x][y]=get_n_mu(in,in->imat[z][x][y]);
		in->mup[z][x][y]=get_p_mu(in,in->imat[z][x][y]);

		if (in->ntrapnewton)
		{
			in->nt_all[z][x][y]=0.0;
			for (band=0;band<in->srh_bands;band++)
			{
				in->Fnt[z][x][y][band]=in->xt[z][x][y][band]-in->phi[z][x][y];

				in->srh_n_r1[z][x][y][band]=get_n_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,srh_1,in->imat[z][x][y]);
				in->srh_n_r2[z][x][y][band]=get_n_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,srh_2,in->imat[z][x][y]);
				in->srh_n_r3[z][x][y][band]=get_n_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,srh_3,in->imat[z][x][y]);
				in->srh_n_r4[z][x][y][band]=get_n_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,srh_4,in->imat[z][x][y]);
				in->dsrh_n_r1[z][x][y][band]=get_dn_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,srh_1,in->imat[z][x][y]);
				in->dsrh_n_r2[z][x][y][band]=get_dn_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,srh_2,in->imat[z][x][y]);
				in->dsrh_n_r3[z][x][y][band]=get_dn_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,srh_3,in->imat[z][x][y]);
				in->dsrh_n_r4[z][x][y][band]=get_dn_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,srh_4,in->imat[z][x][y]);

				in->nt[z][x][y][band]=get_n_pop_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,in->imat[z][x][y]);
				in->dnt[z][x][y][band]=get_dn_pop_srh(sim,in,in->xt[z][x][y][band]+in->tt[z][x][y],in->Te[z][x][y],band,in->imat[z][x][y]);
				in->nt_all[z][x][y]+=in->nt[z][x][y][band];

			}
		}

		if (in->ptrapnewton)
		{
			in->pt_all[z][x][y]=0.0;
			for (band=0;band<in->srh_bands;band++)
			{
				in->Fpt[z][x][y][band]= -in->xpt[z][x][y][band]-in->phi[z][x][y];

				in->srh_p_r1[z][x][y][band]=get_p_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,srh_1,in->imat[z][x][y]);
				in->srh_p_r2[z][x][y][band]=get_p_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,srh_2,in->imat[z][x][y]);
				in->srh_p_r3[z][x][y][band]=get_p_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,srh_3,in->imat[z][x][y]);
				in->srh_p_r4[z][x][y][band]=get_p_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,srh_4,in->imat[z][x][y]);
				in->dsrh_p_r1[z][x][y][band]=get_dp_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,srh_1,in->imat[z][x][y]);
				in->dsrh_p_r2[z][x][y][band]=get_dp_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,srh_2,in->imat[z][x][y]);
				in->dsrh_p_r3[z][x][y][band]=get_dp_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,srh_3,in->imat[z][x][y]);
				in->dsrh_p_r4[z][x][y][band]=get_dp_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,srh_4,in->imat[z][x][y]);

				in->pt[z][x][y][band]=get_p_pop_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,in->imat[z][x][y]);
				in->dpt[z][x][y][band]=get_dp_pop_srh(sim,in,in->xpt[z][x][y][band]-in->tpt[z][x][y],in->Th[z][x][y],band,in->imat[z][x][y]);
				in->pt_all[z][x][y]+=in->pt[z][x][y][band];
				//printf("%le\n",in->pt[z][x][y][band]);
			}
		}

		}

}


void init_mat_arrays(struct device *in)
{
int x=0;
int y=0;
int z=0;

	for (z=0;z<in->zmeshpoints;z++)
	{

		for (x=0;x<in->xmeshpoints;x++)
		{

			for (y=0;y<in->ymeshpoints;y++)
			{
				printf("%d %d %d\n",z,x,y);
				in->Tl[z][x][y]=in->Tll+in->ymesh[y]*(in->Tlr-in->Tll)/in->ylen;
				in->Te[z][x][y]=in->Tll+in->ymesh[y]*(in->Tlr-in->Tll)/in->ylen;
				in->Th[z][x][y]=in->Tll+in->ymesh[y]*(in->Tlr-in->Tll)/in->ylen;
				in->ex[z][x][y]=0.0;
				in->Hex[z][x][y]=0.0;
				//if ((i>in->ymeshpoints/2)&&(i<in->ymeshpoints/2+10)) in->Hex[z][x][y]=1e9;
				in->epsilonr[z][x][y]=get_dos_epsilonr(in,in->imat[z][x][y]);

				//printf("ended\n");
				in->Eg[z][x][y]=get_dos_Eg(in,in->imat[z][x][y]);
				in->B[z][x][y]=get_dos_B(in,in->imat[z][x][y]);
				in->Dex[z][x][y]=0.0;//get_mat_param(&(in->mat.l[in->imat[z][x][y]]),mat_Dex);

				in->Xi[z][x][y]=get_dos_Xi(in,in->imat[z][x][y]);

				in->Ec[z][x][y]= -in->Xi[z][x][y];

				in->Ev[z][x][y]= -in->Xi[z][x][y]-in->Eg[z][x][y];

				//printf("%d %e %e\n",i,in->mun[z][x][y],in->mup[z][x][y]);

				in->Nc[z][x][y]=get_Nc_free(in,in->imat[z][x][y]);

				in->Nv[z][x][y]=get_Nv_free(in,in->imat[z][x][y]);

				in->mun[z][x][y]=get_n_mu(in,in->imat[z][x][y]);
				in->mup[z][x][y]=get_p_mu(in,in->imat[z][x][y]);

				in->kf[z][x][y]=0.0;//get_mat_param(&(in->mat.l[in->imat[z][x][y]]),mat_kf);
				in->kl[z][x][y]=in->thermal_kl;//get_mat_param(&(in->mat.l[in->imat[z][x][y]]),mat_kl);
				in->ke[z][x][y]=get_n_mu(in,in->imat[z][x][y]);
				in->kh[z][x][y]=get_p_mu(in,in->imat[z][x][y]);

				in->Hl[z][x][y]=0.0;
				in->He[z][x][y]=0.0;
				in->Hh[z][x][y]=0.0;
				in->Habs[z][x][y]=0.0;

				in->t[z][x][y]=in->Xi[z][x][y];
				in->tp[z][x][y]=in->Xi[z][x][y]+in->Eg[z][x][y];

				in->tt[z][x][y]=in->Xi[z][x][y];
				in->tpt[z][x][y]=in->Xi[z][x][y]+in->Eg[z][x][y];

			}
		}
	}
}

