//    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for 1st, 2nd and 3rd generation solar cells.
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "device.h"
#include "light.h"
#include "dll_interface.h"
#include "log.h"
#include "complex_solver.h"
#include "sim.h"

static struct dll_interface pointers;

void dll_interface_fixup()
{
	pointers.printf_log = &printf_log;
	pointers.waveprint = &waveprint;
	pointers.get_dump_status = &get_dump_status;
	pointers.light_dump_1d = &light_dump_1d;
	pointers.light_solve_optical_problem = &light_solve_optical_problem;
	pointers.light_free_memory = &light_free_memory;
	pointers.light_transfer_gen_rate_to_device =
	    &light_transfer_gen_rate_to_device;
	pointers.complex_solver = &complex_solver;
	pointers.get_n_den = &get_n_den;
	pointers.get_dn_den = &get_dn_den;
	pointers.get_n_w = &get_n_w;
	pointers.get_p_den = &get_p_den;
	pointers.get_dp_den = &get_dp_den;
	pointers.get_p_w = &get_p_w;
	pointers.B = &B;
	pointers.dB = &dB;
	pointers.dump_matrix = &dump_matrix;
	pointers.ewe = &ewe;
	pointers.get_dump_status = &get_dump_status;
	pointers.solver = &solver;
	pointers.get_J = &get_J;
	pointers.get_I = &get_I;
	pointers.fopena = &fopena;
	pointers.dump_1d_slice = &dump_1d_slice;
	pointers.update_arrays = &update_arrays;
}

struct dll_interface *dll_get_interface()
{
	return &pointers;
}
