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
#include "inp.h"
#include "log.h"
#include <cal_path.h>

void dump_load_config(struct simulation* sim,struct device *in)
{
	int dump;
	struct inp_file inp;
	inp_init(sim,&inp);

	inp_load_from_path(sim,&inp,get_input_path(sim),"dump.inp");

	inp_check(sim,&inp,1.41);

	dump=inp_search_english(sim,&inp,"#plot");
	set_dump_status(sim,dump_plot,dump);

	dump=inp_search_english(sim,&inp,"#newton_dump");
	set_dump_status(sim,dump_newton, dump);

	dump=inp_search_english(sim,&inp,"#dump_dynamic");
	set_dump_status(sim,dump_dynamic, dump);

	in->stop_start=inp_search_english(sim,&inp,"#startstop");

	in->dumpitdos=inp_search_english(sim,&inp,"#dumpitdos");

	inp_search_string(sim,&inp,in->plot_file,"#plotfile");

	inp_search_gdouble(sim,&inp,&(in->start_stop_time),"#start_stop_time");

	inp_search_int(sim,&inp,&(dump),"#dump_iodump");
	set_dump_status(sim,dump_iodump,dump);

	dump=inp_search_english(sim,&inp,"#dump_optics");
	set_dump_status(sim,dump_optics, dump);

	dump=inp_search_english(sim,&inp,"#dump_optics_verbose");
	set_dump_status(sim,dump_optics_verbose, dump);

	dump=inp_search_english(sim,&inp,"#dump_norm_time_to_one");
	set_dump_status(sim,dump_norm_time_to_one, dump);

	dump=inp_search_english(sim,&inp,"#dump_norm_y_axis");
	set_dump_status(sim,dump_norm_y_axis, dump);

	dump=inp_search_english(sim,&inp,"#dump_1d_slices");
	set_dump_status(sim,dump_1d_slices, dump);

	dump=inp_search_english(sim,&inp,"#dump_energy_slice_switch");
	set_dump_status(sim,dump_energy_slice_switch, dump);

	inp_search_int(sim,&inp,&(in->dump_slicepos),"#dump_energy_slice_pos");
	if (in->dump_slicepos>=in->ymeshpoints) in->dump_slicepos=0;

	dump=inp_search_english(sim,&inp,"#dump_print_newtonerror");
	set_dump_status(sim,dump_print_newtonerror, dump);

	dump=inp_search_english(sim,&inp,"#dump_write_converge");
	set_dump_status(sim,dump_write_converge, dump);

	dump=inp_search_english(sim,&inp,"#dump_print_converge");
	set_dump_status(sim,dump_print_converge, dump);

	dump=inp_search_english(sim,&inp,"#dump_print_pos_error");
	set_dump_status(sim,dump_print_pos_error, dump);

	dump=inp_search_english(sim,&inp,"#dump_pl");
	set_dump_status(sim,dump_pl, dump);

	dump=inp_search_english(sim,&inp,"#dump_zip_files");
	set_dump_status(sim,dump_zip_files, dump);

	dump=inp_search_english(sim,&inp,"#dump_write_out_band_structure");
	set_dump_status(sim,dump_write_out_band_structure, dump);

	dump=inp_search_english(sim,&inp,"#dump_equilibrium");
	set_dump_status(sim,dump_equilibrium, dump);

	dump=inp_search_english(sim,&inp,"#dump_first_guess");
	set_dump_status(sim,dump_first_guess, dump);

	dump=inp_search_english(sim,&inp,"#dump_optical_probe");
	set_dump_status(sim,dump_optical_probe, dump);

	dump=inp_search_english(sim,&inp,"#dump_optical_probe_spectrum");
	set_dump_status(sim,dump_optical_probe_spectrum, dump);


	sim->log_level=inp_search_english(sim,&inp,"#dump_log_level");

	dump=inp_search_english(sim,&inp,"#dump_print_text");
	set_dump_status(sim,dump_print_text, dump);

	dump=inp_search_english(sim,&inp,"#dump_optics_summary");
	set_dump_status(sim,dump_optics_summary, dump);

	dump=inp_search_english(sim,&inp,"#dump_info_text");
	set_dump_status(sim,dump_info_text, dump);

	dump=inp_search_english(sim,&inp,"#dump_built_in_voltage");
	set_dump_status(sim,dump_built_in_voltage, dump);

	dump=inp_search_english(sim,&inp,"#dump_ray_trace_map");
	set_dump_status(sim,dump_ray_trace_map, dump);

	inp_free(sim,&inp);


}
