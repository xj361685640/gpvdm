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
#include <exp.h>
#include "sim.h"
#include "dump.h"
#include "ntricks.h"
#include "gui_hooks.h"
#include <plot.h>

static int unused __attribute__ ((unused));

struct newton_math_state math_save_state;

void newton_push_state(struct device *in)
{
	math_save_state.min_cur_error = in->min_cur_error;
	math_save_state.max_electrical_itt = in->max_electrical_itt;
	math_save_state.newton_min_itt = in->newton_min_itt;
	math_save_state.electrical_clamp = in->electrical_clamp;
	math_save_state.newton_clever_exit = in->newton_clever_exit;
}

void newton_pop_state(struct device *in)
{
	in->min_cur_error = math_save_state.min_cur_error;
	in->max_electrical_itt = math_save_state.max_electrical_itt;
	in->newton_min_itt = math_save_state.newton_min_itt;
	in->electrical_clamp = math_save_state.electrical_clamp;
	in->newton_clever_exit = math_save_state.newton_clever_exit;
}

void ramp_externalv(struct simulation *sim, struct device *in, gdouble from,
		    gdouble to)
{
	gdouble V = from;
	gdouble dV = 0.1;
	if ((to - from) < 0.0)
		dV *= -1.0;
	printf("dV=%Le\n", dV);
	printf("Ramping: from=%Le to=%Le\n", from, to);

	if (fabs(to - from) <= fabs(dV))
		return;

	do {
		V += dV;
		if (get_dump_status(sim, dump_print_text) == TRUE)
			printf("ramp: %Lf %Lf %d\n", V, to, in->kl_in_newton);
		sim_externalv(in, V);

		plot_now(sim, "jv.plot");
		gui_send_data("pulse");

		if (fabs(in->Vapplied - to) < fabs(dV)) {
			break;
		}

	} while (1);

	if (V != to) {
		V = to;
		sim_externalv(in, V);
	}

	return;
}

void ramp(struct simulation *sim, struct device *in, gdouble from, gdouble to,
	  gdouble steps)
{
	in->kl_in_newton = FALSE;
	solver_realloc(in);

	in->Vapplied = from;
	newton_push_state(in);
	gdouble dV = 0.20;
	in->min_cur_error = 1e-5;
	in->max_electrical_itt = 12;
	in->newton_min_itt = 3;
	in->electrical_clamp = 2.0;
	in->newton_clever_exit = FALSE;
	if ((to - from) < 0.0)
		dV *= -1.0;
	printf("dV=%Le\n", dV);
	printf("Ramping: from=%Le to=%Le\n", from, to);

	if (fabs(to - from) <= fabs(dV))
		return;

	do {
		in->Vapplied += dV;
//if (in->Vapplied<-4.0) dV= -0.3;
		if (get_dump_status(sim, dump_print_text) == TRUE)
			printf("ramp: %Lf %Lf %d\n", in->Vapplied, to,
			       in->kl_in_newton);
		solve_all(in);
		plot_now(sim, "jv_vars.plot");
//sim_externalv(in,in->cevoltage);

		if (fabs(in->Vapplied - to) < fabs(dV)) {
//save_state(in,to);
			break;
		}

	} while (1);

	newton_pop_state(in);

	if (in->Vapplied != to) {
		in->Vapplied = to;
		solve_all(in);
	}

	printf("Finished with ramp\n");
	return;
}

void save_state(struct device *in, gdouble to)
{
//<clean>
	printf("Dumping state\n");
	int i;
	int band;
	FILE *state;
	state = fopena(in->outputpath, "state.dat", "w");

	fprintf(state, "%Le ", to);

	for (i = 0; i < in->ymeshpoints; i++) {
		fprintf(state, "%Le %Le %Le ", in->phi[i], in->x[i], in->xp[i]);

		for (band = 0; band < in->srh_bands; band++) {
			fprintf(state, "%Le %Le ", in->xt[i][band],
				in->xpt[i][band]);
		}

	}
	fclose(state);
//</clean>
}

int load_state(struct device *in, gdouble voltage)
{
//<clean>
	printf("Load state\n");
	int i;
	int band;
	gdouble vtest;
	FILE *state;
	state = fopena(in->outputpath, "state.dat", "r");
	if (!state) {
		printf("State not found\n");
		return FALSE;
	}

	unused = fscanf(state, "%Le", &vtest);
	printf("%Le %Le", voltage, vtest);
	if (vtest != voltage) {
		printf("State not found\n");
		return FALSE;
	}
	printf("Loading state\n");

	in->Vapplied = vtest;

	for (i = 0; i < in->ymeshpoints; i++) {
		unused =
		    fscanf(state, "%Le %Le %Le ", &(in->phi[i]), &(in->x[i]),
			   &(in->xp[i]));

		for (band = 0; band < in->srh_bands; band++) {
			unused =
			    fscanf(state, "%Le %Le ", &(in->xt[i][band]),
				   &(in->xpt[i][band]));
		}

	}
	fclose(state);
	return TRUE;
//</clean>
}

gdouble sim_externalv_ittr(struct device * in, gdouble wantedv)
{
	gdouble clamp = 0.1;
	gdouble step = 0.01;
	gdouble e0;
	gdouble e1;
	gdouble i0;
	gdouble i1;
	gdouble deriv;
	gdouble Rs = in->Rcontact;
	solve_all(in);
	i0 = get_I(in);

	gdouble itot = i0 + in->Vapplied / in->Rshunt;

	e0 = fabs(itot * Rs + in->Vapplied - wantedv);
	in->Vapplied += step;
	solve_all(in);

	i1 = get_I(in);
	itot = i1 + in->Vapplied / in->Rshunt;

	e1 = fabs(itot * Rs + in->Vapplied - wantedv);
//printf("%Le\n",e1);
	deriv = (e1 - e0) / step;
	step = -e1 / deriv;
//step=step/(1.0+fabs(step/clamp));
	in->Vapplied += step;
	int count = 0;
	int max = 1000;
	do {
		e0 = e1;
		solve_all(in);
		itot = i1 + in->Vapplied / in->Rshunt;
		e1 = fabs(itot * Rs + in->Vapplied - wantedv);
//printf("error=%Le Vapplied=%Le \n",e1,in->Vapplied);
		deriv = (e1 - e0) / step;
		step = -e1 / deriv;
//gdouble clamp=0.01;
//if (e1<clamp) clamp=e1/100.0;
//step=step/(1.0+fabs(step/clamp));
		step = step / (1.0 + fabs(step / clamp));
		in->Vapplied += step;
		if (count > max)
			break;
		count++;
	} while (e1 > 1e-8);

	gdouble ret = get_I(in) + in->Vapplied / in->Rshunt;
//getchar();
	return ret;
}

gdouble sim_externalv(struct device * in, gdouble wantedv)
{
	in->kl_in_newton = FALSE;
	solver_realloc(in);
	sim_externalv_ittr(in, wantedv);
	return 0.0;
}

void solve_all(struct device *in)
{
	solve_cur(in);
}
