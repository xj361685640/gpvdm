//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

/** @file exp.h
	@brief Functions to meaure physical things from the device, such as current.
*/


#ifndef exp_h
#define exp_h
#include <device.h>

gdouble get_jn_avg(struct device *in);
gdouble get_jp_avg(struct device *in);
gdouble get_charge_change(struct device *in);
void cal_J_drift_diffusion(struct device *in);
gdouble get_Jn_diffusion(struct device *in);
gdouble get_Jn_drift(struct device *in);
gdouble get_Jp_diffusion(struct device *in);
gdouble get_Jp_drift(struct device *in);
gdouble get_avg_field(struct device *in);
gdouble get_np_tot(struct device *in);
void reset_npequlib(struct device *in);
void get_avg_np_pos(struct device *in,gdouble *nx,gdouble *px);
gdouble get_background_charge(struct device *in);
void reset_np_save(struct device *in);
gdouble get_n_trapped_charge(struct device *in);
gdouble get_p_trapped_charge(struct device *in);
gdouble get_avg_recom(struct device *in);
gdouble get_avg_recom_n(struct device *in);
gdouble get_avg_recom_p(struct device *in);
gdouble get_avg_Rn(struct device *in);
gdouble get_avg_Rp(struct device *in);
gdouble get_avg_k(struct device *in);
gdouble get_avg_mue(struct device *in);
gdouble get_avg_muh(struct device *in);
gdouble get_free_n_charge(struct device *in);
gdouble get_free_p_charge(struct device *in);
gdouble get_free_n_charge_delta(struct device *in);
gdouble get_free_p_charge_delta(struct device *in);
gdouble get_total_n_trapped_charge(struct device *in);
gdouble get_total_p_trapped_charge(struct device *in);
gdouble get_n_trapped_charge_delta(struct device *in);
gdouble get_p_trapped_charge_delta(struct device *in);
gdouble get_avg_relax_n(struct device *in);
gdouble get_avg_relax_p(struct device *in);
gdouble get_avg_J(struct device *in);
gdouble get_free_np_avg(struct device *in);
gdouble get_extracted_np(struct device *in);
gdouble get_extracted_k(struct device *in);
gdouble get_charge_delta(struct device *in);
gdouble get_I_recomb(struct device *in);
gdouble get_J_left(struct device *in);
gdouble get_J_right(struct device *in);
gdouble get_J_recom(struct device *in);
gdouble get_I_ce(struct simulation *sim,struct device *in);
gdouble get_equiv_I(struct simulation *sim,struct device *in);
gdouble carrier_count_get_rn(struct device *in);
gdouble carrier_count_get_rp(struct device *in);

gdouble carrier_count_get_n(struct device *in);
gdouble carrier_count_get_p(struct device *in);
void carrier_count_reset(struct device *in);
void carrier_count_add(struct device *in);
gdouble get_extracted_n(struct device *in);
gdouble get_extracted_p(struct device *in);
gdouble get_equiv_V(struct simulation *sim,struct device *in);
gdouble get_equiv_J(struct simulation *sim,struct device *in);
gdouble get_I(struct device *in);
gdouble get_J(struct device *in);
gdouble get_charge(struct device *in);
gdouble get_avg_gen(struct device *in);
void set_orig_charge_den(struct device *in);
gdouble get_total_np(struct device *in);
gdouble get_charge_tot(struct device *in);
gdouble get_tot_photons_abs(struct device *in);
gdouble get_i_intergration(struct device *in);
gdouble get_avg_J_std(struct device *in);
gdouble get_max_Jsc(struct device *in);
#endif
