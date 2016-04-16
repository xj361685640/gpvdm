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

#include <sim.h>
#include <exp.h>
#include "jv.h"
#include <dump.h>
#include <dynamic_store.h>
#include "ntricks.h"
#include <inp.h>
#include <buffer.h>
#include <gui_hooks.h>
#include <pl.h>
#include <log.h>
#include <lang.h>
#include <remesh.h>
#include <plot.h>
#include <cal_path.h>

static int unused __attribute__ ((unused));

void sim_jv(struct simulation *sim, struct device *in)
{
	FILE *outf = fopen("o.dat", "w");
	fclose(outf);
	printf_log(sim, _("Running JV simulation\n"));
	struct buffer buf;
	buffer_init(&buf);

	struct dynamic_store store;
	dump_dynamic_init(sim, &store, in);

	struct jv config;
	jv_load_config(sim, &config, in);
	gdouble V = 0.0;
	gdouble Vstop = config.Vstop;
	gdouble Vstep = config.Vstep;
	int ittr = 0;
	gdouble J;
	gdouble Pden;
	int first = TRUE;
	gdouble Vlast;
	gdouble Jlast;
	gdouble Pdenlast;
	gdouble Vexternal;

	struct istruct ivexternal;
	inter_init(&ivexternal);

	struct istruct jvexternal;
	inter_init(&jvexternal);

	struct istruct jv;
	inter_init(&jv);

	struct istruct jvavg;
	inter_init(&jvavg);

	struct istruct charge;
	inter_init(&charge);

	struct istruct charge_tot;
	inter_init(&charge_tot);

	struct istruct klist;
	inter_init(&klist);

	struct istruct lv;
	inter_init(&lv);

	in->Vapplied = 0.0;
/*if (gfabs(config.Vstart-in->Vapplied)>0.2)
{
	ramp_externalv(in,0.0,config.Vstart);
}

in->Vapplied=config.Vstart;

sim_externalv(in,in->Vapplied);
*/
	remesh_reset(in, in->Vapplied);
//if (in->remesh==TRUE)
//{
//      
//}

	gdouble sun_orig = light_get_sun(&(in->mylight));
	light_set_sun(&(in->mylight), sun_orig * config.jv_light_efficiency);
	light_solve_and_update(sim, in, &(in->mylight), 0.0);
	printf("rod\n");

	newton_set_min_ittr(in, 30);
	in->Vapplied = config.Vstart;
	V = in->Vapplied;
	newton_sim_jv(sim, in);
	newton_set_min_ittr(in, 0);

//gdouble k_voc=0.0;
	gdouble n_voc = 0.0;
	gdouble r_voc = 0.0;
	gdouble nsc = 0.0;
	gdouble n_trap_voc = 0.0;
	gdouble p_trap_voc = 0.0;
	gdouble n_free_voc = 0.0;
	gdouble p_free_voc = 0.0;
	gdouble np_voc_tot = 0.0;
	gdouble r_pmax = 0.0;
	gdouble n_pmax = 0.0;
	printf("rod2\n");

	do {

		in->Vapplied = V;
		newton_sim_jv(sim, in);

		J = get_equiv_J(in);

		Vexternal = get_equiv_V(in);

		gui_send_data("pulse");

		if (ittr > 0) {

			inter_append(&jvexternal, get_equiv_V(in),
				     get_equiv_J(in));
			inter_append(&jvavg, V, get_avg_J(in));
			inter_append(&jv, V, get_J(in));
			inter_append(&ivexternal, get_equiv_V(in),
				     get_equiv_I(in));

		}

		ittr++;

		inter_append(&charge, get_equiv_V(in), get_extracted_np(in));
		inter_append(&charge_tot, get_equiv_V(in), get_np_tot(in));
/*
		FILE *deriv=fopen("dfn.dat","w");
		for (r=0;r<in->ymeshpoints-1;r++)
		{
			gdouble d=(in->Fn[r+1]-in->Fn[r])/(in->ymesh[r+1]-in->ymesh[r]);
			d*=(in->n[r]+in->n[r+1])/2;
			d*=(in->mun[r]+in->mun[r+1])/2;

			fprintf(deriv,"%Le %Le\n",in->ymesh[r],d);
		}
		fclose(deriv);
		getchar();
*/
		Pden = gfabs(J * Vexternal);

		//printf("Plotted\n");
		plot_now(sim, "jv.plot");
		stop_start(sim, in);
		dump_dynamic_add_data(sim, &store, in, get_equiv_V(in));

		if (first == FALSE) {

			//check if we have crossed 0V
			if ((Vlast <= 0) && (Vexternal >= 0.0)) {
				in->Jsc =
				    Jlast + (J - Jlast) * (0 - Vlast) / (V -
									 Vlast);
				nsc = get_extracted_np(in);
				printf_log(sim, "nsc=%Le\n", nsc);
				printf_log(sim, "Jsc = %Le\n", in->Jsc);
			}

			if ((Jlast <= 0) && (J >= 0.0)) {
				in->Voc =
				    Vlast + (Vexternal - Vlast) * (0 -
								   Jlast) / (J -
									     Jlast);
				printf_log(sim, "Voc = %Le\n", in->Voc);
				//k_voc=get_avg_recom(in)/pow(get_extracted_np(in),2.0);
				r_voc = get_avg_recom(in);
				n_voc = get_extracted_np(in);
				np_voc_tot = get_total_np(in);
				n_trap_voc = get_n_trapped_charge(in);
				n_free_voc = get_free_n_charge(in);
				p_trap_voc = get_p_trapped_charge(in);
				p_free_voc = get_free_p_charge(in);

			}

			if ((Pden > Pdenlast) && (Vexternal > 0.0) && (J < 0.0)) {
				in->Pmax = Pden;
				in->Pmax_voltage = Vexternal;
				r_pmax = get_avg_recom(in);
				n_pmax = get_extracted_np(in);
			}

			if (Vexternal > Vstop)
				break;

		}

		if (get_dump_status(sim, dump_print_converge) == TRUE) {
			printf_log(sim,
				   "V=%Lf %Lf current = %Le mA (%Le A/m^2) %Le\n",
				   V, Vexternal, get_I(in) / 1e-3, J,
				   in->last_error);
		}

		Jlast = J;
		Vlast = Vexternal;
		Pdenlast = Pden;
		first = FALSE;

		dump_write_to_disk(sim, in);

		outf = fopen("o.dat", "a");
		fprintf(outf, "%Le %Le\n", V, J);
		fclose(outf);

		inter_append(&lv, get_equiv_V(in), pl_get_light_energy());

		V += Vstep;
		Vstep *= config.jv_step_mul;
		//dialog_set_progress ((in->Vstart+V)/(in->Vstop-in->Vstart));
		if ((Vstep >= 0) && (V > Vstop)) {
			in->stop = TRUE;
		}

		if ((Vstep < 0) && (V < Vstop)) {
			in->stop = TRUE;
		}

		if (get_equiv_J(in) > config.jv_max_j) {
			in->stop = TRUE;
		}

		if (in->stop == TRUE) {
			break;
		}
		inter_append(&klist, get_extracted_np(in),
			     get_avg_recom(in) /
			     (pow(get_extracted_np(in), 2.0)));

		stop_start(sim, in);

	} while (1);
//printf("exit\n");

	in->FF = gfabs(in->Pmax / (in->Jsc * in->Voc));

	if (get_dump_status(sim, dump_print_text) == TRUE) {
		printf_log(sim, "Voc= %Lf (V)\n", in->Voc);
		printf_log(sim, "Jsc= %Lf (A/m^2)\n", in->Jsc);
		printf_log(sim, "Pmax= %Lf (W/m^2)\n", in->Pmax);
		printf_log(sim, "Voltage to get Pmax= %Lf (V)\n",
			   in->Pmax_voltage);
		printf_log(sim, "FF= %Lf\n", in->FF * 100.0);
		printf_log(sim, "Efficiency= %Lf percent\n",
			   gfabs(in->Pmax / light_get_sun(&(in->mylight)) /
				 1000) * 100.0);
	}

	FILE *out;
	out = fopena(get_input_path(sim), "sim_info.dat", "w");
	fprintf(out, "#ff\n%Le\n", in->FF);
	fprintf(out, "#pce\n%Le\n",
		gfabs(in->Pmax / (light_get_sun(&(in->mylight)) / 1000)) *
		100.0);
	fprintf(out, "#voc\n%Le\n", in->Voc);
	fprintf(out, "#voc_tau\n%Le\n", n_voc / r_voc);
	fprintf(out, "#voc_R\n%Le\n", r_voc);
	fprintf(out, "#jv_voc_k\n%Le\n", r_voc / n_voc);
	fprintf(out, "#jv_pmax_n\n%Le\n", n_pmax);
	fprintf(out, "#jv_pmax_tau\n%Le\n", n_pmax / r_pmax);
	fprintf(out, "#voc_nt\n%Le\n", n_trap_voc);
	fprintf(out, "#voc_pt\n%Le\n", p_trap_voc);
	fprintf(out, "#voc_nf\n%Le\n", n_free_voc);
	fprintf(out, "#voc_pf\n%Le\n", p_free_voc);
	fprintf(out, "#jsc\n%Le\n", in->Jsc);
	fprintf(out, "#jv_jsc_n\n%Le\n", nsc);
	fprintf(out, "#jv_vbi\n%Le\n", in->vbi);
	fprintf(out, "#jv_gen\n%Le\n", get_avg_gen(in));
	fprintf(out, "#voc_np_tot\n%Le\n", np_voc_tot);
	fprintf(out, "#end");
	fclose(out);

	if (get_dump_status(sim, dump_iodump) == TRUE) {

		inter_save_a(&klist, get_output_path(sim), "k.dat");
		inter_free(&klist);
	}

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Charge density - Applied voltage");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Charge density");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "m^{-3}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, charge.x, charge.data, charge.len);
	buffer_dump_path(get_output_path(sim), "charge.dat", &buf);
	buffer_free(&buf);

	inter_save_a(&charge_tot, get_output_path(sim), "charge_tot.dat");
	inter_free(&charge_tot);

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Current density - Applied voltage");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Current density");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "A m^{-2}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, jvexternal.x, jvexternal.data, jvexternal.len);
	buffer_dump_path(get_output_path(sim), "jv.dat", &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Current density - Applied voltage");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Current density");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "A m^{-2}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, jv.x, jv.data, jv.len);
	buffer_dump_path(get_output_path(sim), "jv_internal.dat", &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Current density - Applied voltage");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Current density");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "A m^{-2}");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, jvavg.x, jvavg.data, jvavg.len);
	buffer_dump_path(get_output_path(sim), "jv_avg.dat", &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul = 1.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Current - Applied voltage");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Current");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "A");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, jvexternal.x, jvexternal.data, jvexternal.len);
	buffer_dump_path(get_output_path(sim), "iv.dat", &buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	buf.y_mul = 1000.0;
	buf.x_mul = 1.0;
	strcpy(buf.title, "Voltage - Light generated");
	strcpy(buf.type, "xy");
	strcpy(buf.x_label, "Applied Voltage");
	strcpy(buf.y_label, "Light power");
	strcpy(buf.x_units, "Volts");
	strcpy(buf.y_units, "mW");
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf, lv.x, lv.data, lv.len);
	buffer_dump_path(get_output_path(sim), "lv.dat", &buf);
	buffer_free(&buf);

	inter_free(&jvexternal);
	inter_free(&jv);
	inter_free(&jvavg);
	inter_free(&charge);
	inter_free(&ivexternal);
	inter_free(&lv);

	dump_dynamic_save(sim, get_output_path(sim), &store);
	dump_dynamic_free(sim, &store);

	light_set_sun(&(in->mylight), sun_orig);
}

void jv_load_config(struct simulation *sim, struct jv *in, struct device *dev)
{
	struct inp_file inp;
	inp_init(sim, &inp);
	inp_load_from_path(sim, &inp, get_input_path(sim), "jv.inp");
	inp_check(sim, &inp, 1.21);
	inp_search_gdouble(sim, &inp, &(in->Vstart), "#Vstart");
	inp_search_gdouble(sim, &inp, &(in->Vstop), "#Vstop");
	inp_search_gdouble(sim, &inp, &(in->Vstep), "#Vstep");
	inp_search_gdouble(sim, &inp, &(in->jv_step_mul), "#jv_step_mul");
	inp_search_gdouble(sim, &inp, &(in->jv_light_efficiency),
			   "#jv_light_efficiency");
	inp_search_gdouble(sim, &inp, &(in->jv_max_j), "#jv_max_j");
	in->jv_light_efficiency = gfabs(in->jv_light_efficiency);
	inp_free(sim, &inp);

}
