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
#include <dump.h>
#include <ntricks.h>
#include <dynamic_store.h>
#include <sys/stat.h>
#include <exp.h>
#include "fxdomain.h"
#include <inp.h>
#include <buffer.h>
#include <gui_hooks.h>
#include <lang.h>
#include <fx.h>
#include <fit_sin.h>
#include <log.h>
#include <cal_path.h>

static int unused __attribute__ ((unused));

struct fxdomain fxdomain_config;

void sim_fxdomain(struct simulation *sim, struct device *in)
{
	time_enable_everything(TRUE);
	struct buffer buf;
	buffer_init(&buf);
	struct dynamic_store store;
	dump_dynamic_init(sim, &store, in);

	struct istruct out_i;
	inter_init(&out_i);

	struct istruct out_v;
	inter_init(&out_v);

	struct istruct out_fx;
	inter_init(&out_fx);

	struct istruct real_imag;
	inter_init(&real_imag);

	struct stat st = { 0 };
	char out_dir[1000];
	join_path(2, out_dir, get_output_path(sim), "frequency");

	if (stat(out_dir, &st) == -1) {
		mkdir(out_dir, 0700);
	}

	char config_file_name[200];

	gdouble lambda = 0.0;
	gdouble stop_time = 0.0;

	if (find_config_file
	    (sim, config_file_name, get_input_path(sim), in->simmode,
	     "fxdomain") != 0) {
		ewe(sim, "%s %s %s\n", _("no fxdomain config file found"),
		    get_input_path(sim), in->simmode);
	}

	fxdomain_load_config(sim, &fxdomain_config, in, config_file_name);

	int number = strextract_int(config_file_name);

	ntricks_externv_set_load(fxdomain_config.fxdomain_Rload);
	fx_load_mesh(sim, in, number);
	gdouble fx = 0.0;
	gdouble i0 = 0;
	gdouble Plight = 0.0;
	gdouble V = 0.0;
	int step = 0;
	gdouble cut_time = 0.0;
	int period_pos = 0;
	int period = 0;
	char send_data[200];
	V = fxdomain_config.fxdomain_Vexternal;
	Plight = light_get_sun((&in->mylight));
	int total_steps =
	    fxdomain_config.fxdomain_points * fxdomain_config.fxdomain_n *
	    fx_points();
	int cur_total_step = 0;
	do {
		V = fxdomain_config.fxdomain_Vexternal;

		fx = fx_get_fx();

		printf_log(sim, "Running frequency %Lf\n", fx);

		in->go_time = FALSE;

		in->time = 0.0;

		time_init(in);

		lambda = (1.0 / fx);
		in->dt = lambda / ((gdouble) fxdomain_config.fxdomain_points);
		stop_time = lambda * fxdomain_config.fxdomain_n;

		light_solve_and_update(sim, in, &(in->mylight), 0.0);
		step = 0;

		if (fxdomain_config.fxdomain_sim_mode == fxdomain_load) {
			sim_externalv(sim, in, V);
			ntricks_externv_newton(sim, in, V, FALSE);
		} else
		    if (fxdomain_config.fxdomain_sim_mode ==
			fxdomain_open_circuit) {
			in->Vapplied = in->Vbi;
			fxdomain_newton_sim_voc(sim, in);
			fxdomain_newton_sim_voc_fast(sim, in, FALSE);
		} else {
			ewe(sim, _("fxdomain mode not known\n"));
		}

		device_timestep(sim, in);

		in->go_time = TRUE;

		carrier_count_reset(in);
		reset_np_save(in);

		cut_time = 0.0;
		period_pos = 0;
		period = 0;
		do {

			if (fxdomain_config.fxdomain_voltage_modulation == TRUE) {
				V = fxdomain_config.fxdomain_Vexternal +
				    fxdomain_config.
				    fxdomain_voltage_modulation_max * sin(2 *
									  PI *
									  fx *
									  in->
									  time);
			} else {
				Plight =
				    light_get_sun((&in->mylight)) +
				    fxdomain_config.
				    fxdomain_light_modulation_max * sin(2 * PI *
									fx *
									in->
									time);
			}
			light_set_sun((&in->mylight), Plight);
			light_solve_and_update(sim, in, &(in->mylight), 0.0);

			if (fxdomain_config.fxdomain_sim_mode == fxdomain_load) {
				i0 = ntricks_externv_newton(sim, in, V, TRUE);
			} else
			    if (fxdomain_config.fxdomain_sim_mode ==
				fxdomain_open_circuit) {
				V = in->Vapplied;
				i0 = fxdomain_newton_sim_voc_fast(sim, in,
								  TRUE);
			} else {
				ewe(sim, _("fxdomain mode not known\n"));
			}

			if (get_dump_status(sim, dump_print_text) == TRUE) {
				printf_log(sim, "%s=%Le %s=%d %.1Le ",
					   _("fxdomain time"), in->time,
					   _("step"), step, in->last_error);
				printf_log(sim,
					   "Vtot=%Lf %s = %Le mA (%Le A/m^2)\n",
					   V, _("current"), get_I(in) / 1e-3,
					   get_J(in));
			}

			dump_dynamic_add_data(sim, &store, in, in->time);

			sprintf(send_data, "percent:%Lf",
				(gdouble) (cur_total_step) /
				(gdouble) (total_steps));
			gui_send_data(send_data);

			char temp[200];
			fx_with_units(temp, fx);
			sprintf(send_data, "text:%s", temp);
			gui_send_data(send_data);

			cur_total_step++;
			//dump_write_to_disk(in);

			//inter_append(&out_i,in->time,i0);
			//inter_append(&out_v,in->time,V);

			device_timestep(sim, in);

			step++;

			if (period >=
			    (fxdomain_config.fxdomain_n -
			     fxdomain_config.periods_to_fit)) {
				inter_append(&out_i, cut_time, i0);
				inter_append(&out_v, cut_time, V);
				cut_time += in->dt;
			}

			if (period_pos >= fxdomain_config.fxdomain_points) {
				period++;
				period_pos = 0;
			}

			period_pos++;

			if (in->time > stop_time)
				break;

		} while (1);
		gdouble v_mag = 0.0;
		gdouble i_mag = 0.0;
		gdouble v_delta = 0.0;
		gdouble i_delta = 0.0;
		gdouble real = 0.0;
		gdouble imag = 0.0;

		fit_sin(sim, &i_mag, &i_delta, &out_i, fx, "i");
		fit_sin(sim, &v_mag, &v_delta, &out_v, fx, "v");
		printf_log(sim, "v_delta=%Le i_delta=%Le\n", v_delta, i_delta);
		gdouble dphi = 2 * PI * fx * (i_delta - v_delta);
		printf_log(sim, "delta_phi=%Lf\n", dphi);

		gdouble mag = i_mag;
		real = mag * cosl(dphi);
		imag = mag * sinl(dphi);

		inter_append(&real_imag, real, imag);

		inter_append(&out_fx, fx, fx);

		printf_log(sim, "%Lf %Lf\n", real, imag);
		char temp[2000];
		buffer_malloc(&buf);
		buf.y_mul = 1e3;
		buf.x_mul = 1e6;
		strcpy(buf.title, _("Time - current"));
		strcpy(buf.type, _("xy"));
		strcpy(buf.x_label, _("Time"));
		strcpy(buf.y_label, _("Current"));
		strcpy(buf.x_units, _("us"));
		strcpy(buf.y_units, _("m"));
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, out_i.x, out_i.data, out_i.len);
		sprintf(temp, "fxdomain_i%d.dat", (int)fx);
		buffer_dump_path(out_dir, temp, &buf);
		buffer_free(&buf);

		buffer_malloc(&buf);
		buf.y_mul = 1.0;
		buf.x_mul = 1e6;
		strcpy(buf.title, _("Time - Voltage"));
		strcpy(buf.type, _("xy"));
		strcpy(buf.x_label, _("Time"));
		strcpy(buf.y_label, _("Voltage"));
		strcpy(buf.x_units, _("us"));
		strcpy(buf.y_units, _("V"));
		buf.logscale_x = 0;
		buf.logscale_y = 0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf, out_v.x, out_v.data, out_v.len);
		sprintf(temp, "fxdomain_v%d.dat", (int)fx);
		buffer_dump_path(out_dir, temp, &buf);
		buffer_free(&buf);

		fx_step();

		inter_reset(&out_i);
		inter_reset(&out_v);

	} while (fx_run() == TRUE);

	dump_dynamic_save(sim, out_dir, &store);
	dump_dynamic_free(sim, &store);

	buffer_malloc(&buf);
	buf.y_mul = 1e3;
	buf.x_mul = 1e3;
	strcpy(buf.title, _("Real - Imaginary"));
	strcpy(buf.type, _("xy"));
	strcpy(buf.x_label, _("Re(i)"));
	strcpy(buf.y_label, _("Im(i)"));
	strcpy(buf.x_units, _("mA"));
	strcpy(buf.y_units, _("mA"));
	buf.logscale_x = 0;
	buf.logscale_y = 0;
	buffer_add_info(&buf);
	buffer_add_xy_data_z_label(&buf, real_imag.x, real_imag.data, out_fx.x,
				   real_imag.len);
	buffer_dump_path(get_output_path(sim), "fxdomain_real_imag.dat", &buf);
	buffer_free(&buf);

	in->go_time = FALSE;

	inter_free(&out_i);
	inter_free(&out_v);
	inter_free(&out_fx);
	inter_free(&real_imag);
	time_memory_free();
}

void fxdomain_load_config(struct simulation *sim, struct fxdomain *in,
			  struct device *dev, char *config_file_name)
{

	char name[200];
	struct inp_file inp;
	inp_init(sim, &inp);
	inp_load_from_path(sim, &inp, get_input_path(sim), config_file_name);
	inp_check(sim, &inp, 1.0);

	inp_search_string(sim, &inp, name, "#fxdomain_sim_mode");
	in->fxdomain_sim_mode = english_to_bin(sim, name);

	inp_search_gdouble(sim, &inp, &(in->fxdomain_Rload), "#fxdomain_Rload");
	inp_search_int(sim, &inp, &(in->fxdomain_points), "#fxdomain_points");
	inp_search_int(sim, &inp, &(in->fxdomain_n), "#fxdomain_n");
	inp_search_gdouble(sim, &inp, &(in->fxdomain_Vexternal),
			   "#fxdomain_Vexternal");
	inp_search_gdouble(sim, &inp, &(in->fxdomain_voltage_modulation_max),
			   "#fxdomain_voltage_modulation_max");
	inp_search_gdouble(sim, &inp, &(in->fxdomain_light_modulation_max),
			   "#fxdomain_light_modulation_max");
	inp_search_int(sim, &inp, &(in->fxdomain_voltage_modulation),
		       "#fxdomain_voltage_modulation");
	inp_search_int(sim, &inp, &(in->periods_to_fit), "#periods_to_fit");

	inp_search_int(sim, &inp, &(in->fxdomain_do_fit), "#fxdomain_do_fit");
	inp_search_gdouble(sim, &inp, &(in->fxdomain_L), "#fxdomain_L");

	inp_free(sim, &inp);

}
