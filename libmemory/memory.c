//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.rodmack.com
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
#include <string.h>
#include <lang.h>
#include <complex_solver.h>
#include "sim.h"
#include "dump.h"
#include "mesh.h"
#include <math.h>
#include "log.h"
#include <solver_interface.h>
#include "memory.h"

void device_alloc_traps(struct device *in)
{
	malloc_srh_bands(in, &(in->nt));
	malloc_srh_bands(in, &(in->ntlast));

	malloc_srh_bands(in, &(in->xt));
	malloc_srh_bands(in, &(in->dnt));
	malloc_srh_bands(in, &(in->srh_n_r1));
	malloc_srh_bands(in, &(in->srh_n_r2));
	malloc_srh_bands(in, &(in->srh_n_r3));
	malloc_srh_bands(in, &(in->srh_n_r4));
	malloc_srh_bands(in, &(in->dsrh_n_r1));
	malloc_srh_bands(in, &(in->dsrh_n_r2));
	malloc_srh_bands(in, &(in->dsrh_n_r3));
	malloc_srh_bands(in, &(in->dsrh_n_r4));
	malloc_srh_bands(in, &(in->Fnt));
	malloc_srh_bands(in, &(in->ntb_save));

	malloc_srh_bands(in, &(in->nt_r1));
	malloc_srh_bands(in, &(in->nt_r2));
	malloc_srh_bands(in, &(in->nt_r3));
	malloc_srh_bands(in, &(in->nt_r4));

	malloc_srh_bands(in, &(in->pt));
	malloc_srh_bands(in, &(in->ptlast));

	malloc_srh_bands(in, &(in->xpt));
	malloc_srh_bands(in, &(in->dpt));
	malloc_srh_bands(in, &(in->srh_p_r1));
	malloc_srh_bands(in, &(in->srh_p_r2));
	malloc_srh_bands(in, &(in->srh_p_r3));
	malloc_srh_bands(in, &(in->srh_p_r4));
	malloc_srh_bands(in, &(in->dsrh_p_r1));
	malloc_srh_bands(in, &(in->dsrh_p_r2));
	malloc_srh_bands(in, &(in->dsrh_p_r3));
	malloc_srh_bands(in, &(in->dsrh_p_r4));
	malloc_srh_bands(in, &(in->ptb_save));
	malloc_srh_bands(in, &(in->Fpt));

	malloc_srh_bands(in, &(in->pt_r1));
	malloc_srh_bands(in, &(in->pt_r2));
	malloc_srh_bands(in, &(in->pt_r3));
	malloc_srh_bands(in, &(in->pt_r4));
}

void device_free_traps(struct device *in)
{
	free_srh_bands(in, in->nt);
	free_srh_bands(in, in->xt);
	free_srh_bands(in, in->dnt);
	free_srh_bands(in, in->srh_n_r1);
	free_srh_bands(in, in->srh_n_r2);
	free_srh_bands(in, in->srh_n_r3);
	free_srh_bands(in, in->srh_n_r4);
	free_srh_bands(in, in->dsrh_n_r1);
	free_srh_bands(in, in->dsrh_n_r2);
	free_srh_bands(in, in->dsrh_n_r3);
	free_srh_bands(in, in->dsrh_n_r4);
	free_srh_bands(in, in->Fnt);
	free_srh_bands(in, in->ntb_save);

	free_srh_bands(in, in->nt_r1);
	free_srh_bands(in, in->nt_r2);
	free_srh_bands(in, in->nt_r3);
	free_srh_bands(in, in->nt_r4);

	free_srh_bands(in, in->ntlast);

	free_srh_bands(in, in->pt);
	free_srh_bands(in, in->dpt);
	free_srh_bands(in, in->xpt);
	free_srh_bands(in, in->srh_p_r1);
	free_srh_bands(in, in->srh_p_r2);
	free_srh_bands(in, in->srh_p_r3);
	free_srh_bands(in, in->srh_p_r4);
	free_srh_bands(in, in->dsrh_p_r1);
	free_srh_bands(in, in->dsrh_p_r2);
	free_srh_bands(in, in->dsrh_p_r3);
	free_srh_bands(in, in->dsrh_p_r4);
	free_srh_bands(in, in->Fpt);
	free_srh_bands(in, in->ptb_save);

	free_srh_bands(in, in->pt_r1);
	free_srh_bands(in, in->pt_r2);
	free_srh_bands(in, in->pt_r3);
	free_srh_bands(in, in->pt_r4);

	free_srh_bands(in, in->ptlast);

}

void device_free(struct simulation *sim,struct device *in)
{

	//1d
	free(in->xmesh);
	free(in->ymesh);
	free(in->zmesh);
	free(in->dxmesh);
	free(in->dymesh);
	free(in->dzmesh);
	//2d
	free_zx_gdouble(in,in->Vapplied_r);
	free_zx_gdouble(in,in->Vapplied_l);
	free_zx_gdouble(in,in->Jnleft);
	free_zx_gdouble(in,in->Jnright);
	free_zx_gdouble(in,in->Jpleft);
	free_zx_gdouble(in,in->Jpright);
	free_zx_int(in,in->n_contact_r);
	free_zx_int(in,in->n_contact_l);

	//3d
	free_3d_gdouble(in,in->phi);
	free_3d_gdouble(in,in->B);
	free_3d_gdouble(in,in->Nad);
	free_3d_gdouble(in,in->n);
	free_3d_gdouble(in,in->p);
	free_3d_gdouble(in,in->dn);
	free_3d_gdouble(in,in->dp);
	free_3d_gdouble(in,in->dndphi);
	free_3d_gdouble(in,in->dpdphi);
	free_3d_gdouble(in,in->Eg);
	free_3d_gdouble(in,in->Xi);
	free_3d_gdouble(in,in->Ev);
	free_3d_gdouble(in,in->Ec);
	free_3d_gdouble(in,in->mun);
	free_3d_gdouble(in,in->mup);
	free_3d_gdouble(in,in->Dn);
	free_3d_gdouble(in,in->Dp);
	free_3d_gdouble(in,in->Fn);
	free_3d_gdouble(in,in->Fp);

	free_3d_gdouble(in,in->Nc);
	free_3d_gdouble(in,in->Nv);
	free_3d_gdouble(in,in->G);
	free_3d_gdouble(in,in->Gn);
	free_3d_gdouble(in,in->Gp);
	free_3d_gdouble(in,in->Photon_gen);
	free_3d_gdouble(in,in->Tl);
	free_3d_gdouble(in,in->Te);
	free_3d_gdouble(in,in->Th);
	free_3d_gdouble(in,in->R);
	free_3d_gdouble(in,in->Fi);
	free_3d_gdouble(in,in->Jn);
	free_3d_gdouble(in,in->Jp);
	free_3d_gdouble(in,in->Jn_x);
	free_3d_gdouble(in,in->Jp_x);
	free_3d_gdouble(in,in->Jn_drift);
	free_3d_gdouble(in,in->Jn_diffusion);
	free_3d_gdouble(in,in->Jp_drift);
	free_3d_gdouble(in,in->Jp_diffusion);
	free_3d_gdouble(in,in->x);
	free_3d_gdouble(in,in->t);
	free_3d_gdouble(in,in->xp);
	free_3d_gdouble(in,in->tp);
	free_3d_gdouble(in,in->ex);
	free_3d_gdouble(in,in->Dex);
	free_3d_gdouble(in,in->Hex);
	free_3d_gdouble(in,in->epsilonr);

	free_3d_gdouble(in,in->kf);
	free_3d_gdouble(in,in->kd);
	free_3d_gdouble(in,in->kr);
	free_3d_gdouble(in,in->Rfree);
	free_3d_gdouble(in,in->Rn);
	free_3d_gdouble(in,in->Rp);
	free_3d_gdouble(in,in->Rn_srh);
	free_3d_gdouble(in,in->Rp_srh);
	free_3d_gdouble(in,in->kl);
	free_3d_gdouble(in,in->ke);
	free_3d_gdouble(in,in->kh);
	free_3d_gdouble(in,in->Hl);
	free_3d_gdouble(in,in->He);
	free_3d_gdouble(in,in->Hh);
	free_3d_gdouble(in,in->Habs);
	free_3d_gdouble(in,in->nlast);
	free_3d_gdouble(in,in->plast);

	free_3d_gdouble(in,in->wn);
	free_3d_gdouble(in,in->wp);

	free_3d_gdouble(in,in->nt_all);

	free_3d_gdouble(in,in->tt);
	free_3d_gdouble(in,in->Rbi_k);

	free_3d_gdouble(in,in->pt_all);


	free_3d_gdouble(in,in->tpt);

	free_3d_gdouble(in,in->nf_save);
	free_3d_gdouble(in,in->pf_save);
	free_3d_gdouble(in,in->nt_save);
	free_3d_gdouble(in,in->pt_save);

	free_3d_gdouble(in,in->nfequlib);
	free_3d_gdouble(in,in->pfequlib);
	free_3d_gdouble(in,in->ntequlib);
	free_3d_gdouble(in,in->ptequlib);

	free_3d_gdouble(in,in->nrelax);
	free_3d_gdouble(in,in->ntrap_to_p);
	free_3d_gdouble(in,in->prelax);
	free_3d_gdouble(in,in->ptrap_to_n);

	free_3d_gdouble(in,in->n_orig);
	free_3d_gdouble(in,in->p_orig);
	free_3d_gdouble(in,in->n_orig_f);
	free_3d_gdouble(in,in->p_orig_f);
	free_3d_gdouble(in,in->n_orig_t);
	free_3d_gdouble(in,in->p_orig_t);

	free_3d_gdouble(in,in->phi_save);

	free_3d_int(in,in->imat);
	free_3d_int(in,in->imat_epitaxy);


	//Free solvers
	solver_free(sim);
	complex_solver_free(sim);
	printf_log(sim,"%s %i %s\n", _("Solved"), in->odes, _("Equations"));
}

void device_get_memory(struct simulation *sim,struct device *in)
{
	in->odes = 0;

	if ((in->ymeshpoints<1)||(in->xmeshpoints<1)||(in->zmeshpoints<1))
	{
		ewe(sim,"%s\n",_("I can't allocate a device with less than 1 mesh point."));
	}

	if ((in->ymeshpoints>50000)||(in->xmeshpoints>50000)||(in->zmeshpoints>50000))
	{
		ewe(sim,"%s\n",_("You are asking me to simulate a device with more than 50000 mesh points, although I could do this I am not going to because it seems a bad idea to me."));
	}

	in->Ti = NULL;
	in->Tj = NULL;
	in->Tx = NULL;
	in->b = NULL;
	in->Tdebug = NULL;

	//1d
	in->zmesh = (gdouble *) malloc(in->zmeshpoints * sizeof(gdouble));
	memset(in->zmesh, 0, in->zmeshpoints * sizeof(gdouble));

	in->xmesh = (gdouble *) malloc(in->xmeshpoints * sizeof(gdouble));
	memset(in->xmesh, 0, in->xmeshpoints * sizeof(gdouble));

	in->ymesh = (gdouble *) malloc(in->ymeshpoints * sizeof(gdouble));
	memset(in->ymesh, 0, in->ymeshpoints * sizeof(gdouble));

	in->dzmesh = (gdouble *) malloc(in->zmeshpoints * sizeof(gdouble));
	memset(in->dzmesh, 0, in->zmeshpoints * sizeof(gdouble));

	in->dxmesh = (gdouble *) malloc(in->xmeshpoints * sizeof(gdouble));
	memset(in->dxmesh, 0, in->xmeshpoints * sizeof(gdouble));

	in->dymesh = (gdouble *) malloc(in->ymeshpoints * sizeof(gdouble));
	memset(in->dymesh, 0, in->ymeshpoints * sizeof(gdouble));

	//2d
	malloc_zx_gdouble(in,&(in->Vapplied_r));
	malloc_zx_gdouble(in,&(in->Vapplied_l));
	malloc_zx_gdouble(in,&(in->Jnleft));
	malloc_zx_gdouble(in,&(in->Jnright));
	malloc_zx_gdouble(in,&(in->Jpleft));
	malloc_zx_gdouble(in,&(in->Jpright));
	malloc_zx_int(in,&(in->n_contact_r));
	malloc_zx_int(in,&(in->n_contact_l));

	//3d
	malloc_3d_gdouble(in,&(in->nf_save));

	malloc_3d_gdouble(in,&(in->pf_save));

	malloc_3d_gdouble(in,&(in->nt_save));

	malloc_3d_gdouble(in,&(in->pt_save));

	malloc_3d_gdouble(in,&(in->nfequlib));

	malloc_3d_gdouble(in,&(in->pfequlib));

	malloc_3d_gdouble(in,&(in->ntequlib));

	malloc_3d_gdouble(in,&(in->ptequlib));

	malloc_3d_gdouble(in,&(in->Habs));

	malloc_3d_gdouble(in,&(in->phi));

	malloc_3d_gdouble(in,&(in->B));

	malloc_3d_gdouble(in,&(in->Nad));

	malloc_3d_gdouble(in,&(in->n));

	malloc_3d_gdouble(in,&(in->p));

	malloc_3d_gdouble(in,&(in->dn));

	malloc_3d_gdouble(in,&(in->dp));

	malloc_3d_gdouble(in,&(in->dndphi));

	malloc_3d_gdouble(in,&(in->dpdphi));

	malloc_3d_gdouble(in,&(in->Eg));

	malloc_3d_gdouble(in,&(in->Fn));

	malloc_3d_gdouble(in,&(in->Fp));

	malloc_3d_gdouble(in,&(in->Xi));

	malloc_3d_gdouble(in,&(in->Ev));

	malloc_3d_gdouble(in,&(in->Ec));

	malloc_3d_gdouble(in,&(in->mun));

	malloc_3d_gdouble(in,&(in->mup));

	malloc_3d_gdouble(in,&(in->Dn));

	malloc_3d_gdouble(in,&(in->Dp));

	malloc_3d_gdouble(in,&(in->Nc));

	malloc_3d_gdouble(in,&(in->Nv));

	malloc_3d_gdouble(in,&(in->G));

	malloc_3d_gdouble(in,&(in->Gn));

	malloc_3d_gdouble(in,&(in->Photon_gen));	

	malloc_3d_gdouble(in,&(in->Gp));

	malloc_3d_gdouble(in,&(in->Tl));

	malloc_3d_gdouble(in,&(in->Te));

	malloc_3d_gdouble(in,&(in->Th));

	malloc_3d_gdouble(in,&(in->R));

	malloc_3d_gdouble(in,&(in->Fi));

	malloc_3d_gdouble(in,&(in->Jn));

	malloc_3d_gdouble(in,&(in->Jp));

	malloc_3d_gdouble(in,&(in->Jn_x));

	malloc_3d_gdouble(in,&(in->Jp_x));
	
	malloc_3d_gdouble(in,&(in->Jn_drift));

	malloc_3d_gdouble(in,&(in->Jn_diffusion));

	malloc_3d_gdouble(in,&(in->Jp_drift));

	malloc_3d_gdouble(in,&(in->Jp_diffusion));

	malloc_3d_gdouble(in,&(in->x));

	malloc_3d_gdouble(in,&(in->t));

	malloc_3d_gdouble(in,&(in->xp));

	malloc_3d_gdouble(in,&(in->tp));

	malloc_3d_gdouble(in,&(in->kf));

	malloc_3d_gdouble(in,&(in->kd));

	malloc_3d_gdouble(in,&(in->kr));

	malloc_3d_gdouble(in,&(in->Rfree));

	malloc_3d_gdouble(in,&(in->Rn));
	malloc_3d_gdouble(in,&(in->Rp));

	malloc_3d_gdouble(in,&(in->Rn_srh));
	malloc_3d_gdouble(in,&(in->Rp_srh));

	malloc_3d_gdouble(in,&(in->ex));

	malloc_3d_gdouble(in,&(in->Dex));

	malloc_3d_gdouble(in,&(in->Hex));

	malloc_3d_gdouble(in,&(in->epsilonr));

	malloc_3d_gdouble(in,&(in->kl));

	malloc_3d_gdouble(in,&(in->ke));

	malloc_3d_gdouble(in,&(in->kh));

	malloc_3d_gdouble(in,&(in->Hl));

	malloc_3d_gdouble(in,&(in->He));

	malloc_3d_gdouble(in,&(in->Hh));

	malloc_3d_gdouble(in,&(in->nlast));

	malloc_3d_gdouble(in,&(in->plast));

	malloc_3d_gdouble(in,&(in->wn));

	malloc_3d_gdouble(in,&(in->wp));

	malloc_3d_gdouble(in,&(in->nt_all));

	malloc_3d_gdouble(in,&(in->phi_save));

	malloc_3d_gdouble(in,&(in->tt));


	malloc_3d_gdouble(in,&(in->pt_all));

	malloc_3d_gdouble(in,&(in->tpt));

	malloc_3d_gdouble(in,&(in->Rbi_k));

	malloc_3d_gdouble(in,&(in->nrelax));

	malloc_3d_gdouble(in,&(in->ntrap_to_p));

	malloc_3d_gdouble(in,&(in->prelax));

	malloc_3d_gdouble(in,&(in->ptrap_to_n));

	malloc_3d_gdouble(in,&(in->n_orig));

	malloc_3d_gdouble(in,&(in->p_orig));

	malloc_3d_gdouble(in,&(in->n_orig_f));

	malloc_3d_gdouble(in,&(in->p_orig_f));

	malloc_3d_gdouble(in,&(in->n_orig_t));

	malloc_3d_gdouble(in,&(in->p_orig_t));

	malloc_3d_int(in,&(in->imat));
	malloc_3d_int(in,&(in->imat_epitaxy));


}
