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


/** @file i.c
	@brief Simple functions to read in scientific data from text files and perform simple maths on the data.
*/
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE
#include <stdio.h>
#include <device.h>
#include <string.h>
#include <dump.h>

static int unused __attribute__((unused));
static char* unused_pchar __attribute__((unused));

void device_init(struct device *in)
{
	in->remesh= -1;
	in->newmeshsize= -1;
	in->Jnleft=NULL;
	in->Jnright=NULL;
	in->Jpleft=NULL;
	in->Jpright=NULL;
	in->n_contact_r=NULL;
	in->n_contact_l=NULL;

	in->phi= NULL;
	in->Nad= NULL;
	in->G= NULL;
	in->Gn= NULL;
	in->Gp= NULL;
	in->n= NULL;
	in->p= NULL;
	in->dn= NULL;
	in->dndphi= NULL;
	in->dp= NULL;
	in->dpdphi= NULL;
	in->Eg= NULL;
	in->Xi= NULL;
	in->Ev= NULL;
	in->Ec= NULL;
	in->Rfree= NULL;

	in->mun= NULL;
	in->mup= NULL;

	in->Dn= NULL;
	in->Dp= NULL;

	in->epsilonr= NULL;

	in->Fn= NULL;
	in->Fp= NULL;
	in->Nc= NULL;
	in->Nv= NULL;
	in->Tl= NULL;
	in->Te= NULL;
	in->Th= NULL;
	in->ymesh= NULL;
	in->R= NULL;
	in->Fi= NULL;
	in->imat= NULL;
	in->imat_epitaxy= NULL;
	in->Jn= NULL;
	in->Jp= NULL;

	in->Jn_diffusion= NULL;
	in->Jn_drift= NULL;

	in->Jp_diffusion= NULL;
	in->Jp_drift= NULL;

	in->Vapplied_r=NULL;
	in->Vapplied_l=NULL;

	in->Vl= -1.0;
	in->Vr= -1.0;
	in->x= NULL;
	in->t= NULL;
	in->xp= NULL;
	in->tp= NULL;
	in->kf= NULL;
	in->kd= NULL;
	in->kr= NULL;

	in->Rn= NULL;
	in->Rp= NULL;
	in->kl= NULL;
	in->ke= NULL;
	in->kh= NULL;
	in->Hl= NULL;
	in->He= NULL;
	in->Hh= NULL;
	in->Habs= NULL;
	in->excite_conv= -1;
	in->thermal_conv= -1;
	in->newton_enable_external_thermal= -1;

	in->deltaFln= -1.0;
	in->deltaFlp= -1.0;
	in->deltaFrn= -1.0;
	in->deltaFrp= -1.0;

	in->Rbi_k= NULL;

	in->ex= NULL;
	in->Dex= NULL;
	in->Hex= NULL;

	in->nf_save= NULL;
	in->pf_save= NULL;
	in->nt_save= NULL;
	in->pt_save= NULL;

	in->nfequlib= NULL;
	in->pfequlib= NULL;
	in->ntequlib= NULL;
	in->ptequlib= NULL;

	in->ntb_save= NULL;
	in->ptb_save= NULL;

	in->phi_save= NULL;

	in->xlen= -1.0;
	in->ylen= -1.0;
	in->zlen= -1.0;

	in->N= -1;
	in->M= -1;
	in->Ti= NULL;
	in->Tj= NULL;
	in->Tx= NULL;
	in->b= NULL;
	in->Tdebug= NULL;

	in->lr_pcontact= -1;
	in->invert_applied_bias= -1;

//math
	in->max_electrical_itt= -1;
	in->electrical_clamp= -1.0;
	in->max_electrical_itt0= -1;
	in->electrical_clamp0= -1.0;
	in->electrical_error0= -1.0;
	in->math_enable_pos_solver= -1.0;
	in->min_cur_error= -1.0;
	in->Pmax_voltage= -1.0;
	in->pos_max_ittr= -1;
	strcpy(in->solver_name,"");
	strcpy(in->newton_name,"");

//Device characterisation
	in->Voc= -1.0;
	in->Jsc= -1.0;
	in->FF= -1.0;
	in->Pmax= -1.0;
	in->Tll= -1.0;
	in->Tlr= -1.0;
	in->Tliso= -1;
	in->dt= -1.0;
	in->srh_sim= -1;
	in->go_time= -1;
	in->time= -1.0;
	in->nlast= NULL;
	in->plast= NULL;
	in->ntrapnewton= -1;
	in->ptrapnewton= -1;

	in->stop= -1;
	in->Rshort= -1.0;
	in->onlypos= -1;
	in->odes= -1;
	in->last_error= -1.0;
	in->posclamp= -1.0;
	in->srh_bands= -1;
	in->wn= NULL;
	in->wp= NULL;
//mesh
	in->meshdata_z= NULL;
	in->meshdata_x= NULL;
	in->meshdata_y= NULL;

	in->zmeshpoints= -1;
	in->xmeshpoints= -1;
	in->ymeshpoints= -1;

	in->zmeshlayers= -1;
	in->xmeshlayers= -1;
	in->ymeshlayers= -1;

//n traps
	in->nt_all= NULL;
	in->nt= NULL;
	in->ntlast= NULL;
	in->dnt= NULL;
	in->srh_n_r1= NULL;
	in->srh_n_r2= NULL;
	in->srh_n_r3= NULL;
	in->srh_n_r4= NULL;
	in->dsrh_n_r1= NULL;
	in->dsrh_n_r2= NULL;
	in->dsrh_n_r3= NULL;
	in->dsrh_n_r4= NULL;
	in->Fnt= NULL;
	in->xt= NULL;
	in->tt= NULL;

	in->nt_r1= NULL;
	in->nt_r2= NULL;
	in->nt_r3= NULL;
	in->nt_r4= NULL;
//p traps
	in->pt_all= NULL;
	in->pt= NULL;
	in->ptlast= NULL;
	in->dpt= NULL;
	in->srh_p_r1= NULL;
	in->srh_p_r2= NULL;
	in->srh_p_r3= NULL;
	in->srh_p_r4= NULL;
	in->dsrh_p_r1= NULL;
	in->dsrh_p_r2= NULL;
	in->dsrh_p_r3= NULL;
	in->dsrh_p_r4= NULL;
	in->Fpt= NULL;
	in->xpt= NULL;
	in->tpt= NULL;

	in->pt_r1= NULL;
	in->pt_r2= NULL;
	in->pt_r3= NULL;
	in->pt_r4= NULL;

	in->A= -1.0;
	in->Vol= -1.0;

	in->Rshunt= -1.0;
	in->Rcontact= -1.0;
	in->Rload= -1.0;
	in->L= -1.0;


	in->lr_bias= -1;

	in->interfaceleft= -1;
	in->interfaceright= -1;
	in->phibleft= -1.0;
	in->phibright= -1.0;
	in->vl_e= -1.0;
	in->vl_h= -1.0;
	in->vr_e= -1.0;
	in->vr_h= -1.0;
	in->stop_start= -1;
	in->externalv= -1.0;
	in->Ilast= -1.0;
	in->timedumpcount= -1;
	strcpy(in->simmode,"");
	in->area= -1.0;

	in->nrelax= NULL;
	in->ntrap_to_p= NULL;
	in->prelax= NULL;
	in->ptrap_to_n= NULL;



	in->lcharge= -1.0;
	in->rcharge= -1.0;

	in->l_electrons= -1.0;
	in->l_holes= -1.0;
	in->r_electrons= -1.0;
	in->r_holes= -1.0;


	in->dumpitdos= -1;


	in->t_big_offset= -1.0;

	in->C= -1.0;
	in->other_layers= -1.0;
	in->last_ittr= -1;

	in->kl_in_newton= -1;
	in->config_kl_in_newton= -1;
	in->B= NULL;
	in->xnl_left= -1.0;
	in->xpl_left= -1.0;
	in->stoppoint= -1;
	in->ilast= -1.0;

	in->newton_clever_exit= -1;
	strcpy(in->plot_file,"");

	in->start_stop_time= -1.0;


	in->Is= -1.0;
	in->n_id= -1.0;
	in->Igen= -1.0;

	in->n_orig= NULL;
	in->p_orig= NULL;
	in->n_orig_f= NULL;
	in->p_orig_f= NULL;
	in->n_orig_t= NULL;
	in->p_orig_t= NULL;
	in->nofluxl= -1;

	in->Vbi= -1.0;
	in->newton_min_itt= -1;
	in->vbi= -1.0;
	in->avg_gen= -1.0;
	in->dump_slicepos= -1;
	in->pl_intensity= -1.0;

	in->Rext= -1.0;
	in->Cext= -1.0;
	in->VCext_last= -1.0;
	in->VCext= -1.0;
	in->newton_last_ittr= -1;
	in->phi_mul= -1.0;

	//Newton
	in->newton_dntrap=NULL;
	in->newton_dntrapdntrap=NULL;
	in->newton_dntrapdn=NULL;
	in->newton_dntrapdp=NULL;
	in->newton_dJdtrapn=NULL;
	in->newton_dJpdtrapn=NULL;

	in->newton_dptrapdp=NULL;
	in->newton_dptrapdptrap=NULL;
	in->newton_dptrap=NULL;
	in->newton_dptrapdn=NULL;
	in->newton_dJpdtrapp=NULL;
	in->newton_dJdtrapp=NULL;
	in->newton_dphidntrap=NULL;
	in->newton_dphidptrap=NULL;
	in->newton_ntlast=NULL;
	in->newton_ptlast=NULL;

	in->tm_sun=NULL;
	in->tm_voltage=NULL;
	in->tm_laser=NULL;
	in->tm_time_mesh=NULL;
	in->tm_fs_laser=NULL;
	in->tm_mesh_len=-1;
	in->tm_use_mesh=-1;
	in->tm_mesh_pos=-1;
	in->ncontacts=-1;
	in->active_contact=-1;

	//LED
	in->led_on=FALSE;
	in->led_wavelength=0.0;

}


