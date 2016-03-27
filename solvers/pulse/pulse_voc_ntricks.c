//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
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
#include <math.h>
#include <dump.h>
#include <sim.h>
#include "pulse.h"
#include <log.h>
#include <dll_interface.h>

void newton_aux_pulse_voc(struct device *in, gdouble V, gdouble * i,
			  gdouble * didv, gdouble * didphi, gdouble * didxil,
			  gdouble * didxipl, gdouble * didphir,
			  gdouble * didxir, gdouble * didxipr)
{
	return;
}

gdouble pulse_newton_sim_voc_fast(struct device * in, int do_LC)
{
	pulse_newton_sim_voc(in);
	return get_I(in) + in->C * (in->Vapplied - in->Vapplied_last) +
	    in->Vapplied / in->Rshunt;
}

gdouble pulse_newton_sim_voc(struct device * in)
{
	printf_log("Looking for Voc\n");
	gdouble C = in->C;
	gdouble clamp = 0.1;
	gdouble step = 0.01;
	gdouble e0;
	gdouble e1;
	gdouble i0;
	gdouble i1;
	gdouble deriv;
	gdouble Rdrain = pulse_config.pulse_Rload + in->Rcontact;

	(*fun->solve_all) (in);
	i0 = get_I(in);
	e0 = fabs(i0 + in->Vapplied * (1.0 / in->Rshunt - 1.0 / Rdrain));

	in->Vapplied += step;
	(*fun->solve_all) (in);
	i1 = get_I(in);
	e1 = fabs(i1 + in->Vapplied * (1.0 / in->Rshunt - 1.0 / Rdrain));

	deriv = (e1 - e0) / step;
	step = -e1 / deriv;

	step = step / (1.0 + fabs(step / clamp));
	in->Vapplied += step;

	int count = 0;
	int max = 200;
	do {
		e0 = e1;
		(*fun->solve_all) (in);
		i1 = get_I(in);
		e1 = fabs(i1 +
			  in->Vapplied * (1.0 / in->Rshunt - 1.0 / Rdrain));
		deriv = (e1 - e0) / step;
		step = -e1 / deriv;

		step = step / (1.0 + fabs(step / clamp));
		in->Vapplied += step;
		if (get_dump_status(dump_print_text) == TRUE) {
			printf_log
			    ("%d pulse voc find Voc Vapplied=%Lf step=%Le error=%Le\n",
			     count, in->Vapplied, step, e1);
		}
		if (count > max)
			break;
		count++;

	} while (e1 > 1e-12);

	gdouble ret = in->Vapplied - C * (i1 - in->Ilast) / in->dt;
	return ret;
}

void pulse_set_light_for_voc(struct device *in, gdouble Voc)
{
}
