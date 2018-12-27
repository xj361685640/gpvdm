//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file dynamic_store.h
	@brief Store information as the simulation progresses such as voltage, current or carrier density, these are 1D arrays as a function of time of simulation step.
*/

#ifndef dynamic_store_h
#define dynamic_store_h
#include "i.h"

struct dynamic_store
{
	struct istruct charge_change;
	struct istruct jout;
	struct istruct jn_avg;
	struct istruct jp_avg;
	struct istruct dynamic_jn;
	struct istruct dynamic_jp;
	struct istruct jnout_mid;
	struct istruct jpout_mid;
	struct istruct iout;
	struct istruct iout_left;
	struct istruct iout_right;
	struct istruct gexout;
	struct istruct ntrap;
	struct istruct ptrap;
	struct istruct ntrap_delta_out;
	struct istruct ptrap_delta_out;
	struct istruct nfree;
	struct istruct pfree;
	struct istruct nfree_delta_out;
	struct istruct pfree_delta_out;
	struct istruct Rnpout;
	struct istruct nfree_to_ptrap;
	struct istruct pfree_to_ntrap;
	struct istruct Rnout;
	struct istruct Rpout;

	struct istruct nrelax_out;
	struct istruct prelax_out;
	struct istruct tpc_mue;
	struct istruct tpc_muh;
	struct istruct tpc_mu_avg;
	struct istruct tpc_filledn;
	struct istruct tpc_filledp;
	struct istruct dynamic_np;
	struct istruct only_n;
	struct istruct only_p;
	struct istruct E_field;
	struct istruct dynamic_pl;
	struct istruct dynamic_Vapplied;
	struct istruct dynamic_charge_tot;
	struct istruct dynamic_jn_drift;
	struct istruct dynamic_jn_diffusion;

	struct istruct dynamic_jp_drift;
	struct istruct dynamic_jp_diffusion;
	struct istruct dynamic_qe;

	struct istruct srh_n_r1;
	struct istruct srh_n_r2;
	struct istruct srh_n_r3;
	struct istruct srh_n_r4;

	struct istruct srh_p_r1;
	struct istruct srh_p_r2;
	struct istruct srh_p_r3;
	struct istruct srh_p_r4;

	struct istruct band_bend;

	gdouble ***band_snapshot;
};
#endif
