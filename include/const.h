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



#ifndef h_const
#define h_const

//Physical constants
#define PATHLEN	512
#define STR_MAX	1024
#define epsilon0 (long double)8.85418782e-12			// m-3 kg-1 s4 A2;
#define epsilon0f (float)8.85418782e-12			// m-3 kg-1 s4 A2;

#define mu0f (float)1.25663706e-6			//

#define hp (long double)6.62606896e-34			//J S Wikipeda
#define PI (long double)3.14159265358979323846

#define PIf (float)3.14159265358979323846

#define hbar (long double)(6.62606896e-34/(2.0*PI))	//Calculated
#define kb (long double)1.3806504e-23			//J K-1 Wiki
#define Q (long double)1.602176487e-19			//C Wikipeda
#define m0 (long double)9.10938215e-31 			//Kg Wikipeda
#define cl  (long double)2.99792458e8			//m/s Wikipieda
#define clf  (float)2.99792458e8			//m/s Wikipieda


//SRH constants
#define srh_1	1
#define srh_2	2
#define srh_3	3
#define srh_4	4
#define interface_schottky	 1

//TRUE/FALSE
#define TRUE 1
#define FALSE 0
#define LEFT 0
#define RIGHT 1
#define TOP 0
#define BOTTOM 1

#define FIT_SIMPLEX 0
#define FIT_NEWTON 1
#define FIT_BFGS 2

//tpv light
#define tpv_set_light 0
#define tpv_set_voltage 1
#define tpv_mode_laser	0
#define tpv_mode_sun 1

//sim modes
#define pulse_open_circuit 0
#define pulse_load 1
#define pulse_ideal_diode_ideal_load 2
#define fxdomain_open_circuit 0
#define fxdomain_load 1

//dump control
#define dump_optics 1
#define dump_newton 2
#define dump_plot 3
#define dump_stop_plot 4
#define dump_opt_for_fit 5
#define dump_write_converge 6
#define dump_print_text 7
#define dump_exit_on_dos_error 8
#define dump_energy_slice_switch 9
#define dump_zip_files 10
#define dump_lock 11
#define dump_norm_time_to_one 12
#define dump_band_structure 14
#define dump_print_newtonerror 15
#define dump_print_converge 16
#define dump_first_guess 17
#define dump_1d_slices 18
#define dump_print_pos_error 19
#define dump_optics_verbose 20
#define dump_pl 21
#define dump_dynamic 22
#define dump_norm_y_axis 24
#define dump_write_out_band_structure 25
#define dump_optics_summary 26
#define dump_optical_probe 27
#define dump_info_text 28
#define dump_optical_probe_spectrum 29
#define dump_ray_trace_map 31
#define dump_file_access_log 32
#define dump_fx 33
#define dump_use_cache 34
#define dump_write_headers 37
#define dump_remove_dos_cache 38

//dos types
#define dos_exp		0
#define dos_an		1
#define dos_fd		2
#define dos_exp_fd 	3
#define dos_read 	5


	#ifndef windows
		#include <linux/limits.h>
	#endif

#endif
