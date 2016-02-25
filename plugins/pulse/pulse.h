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

#ifndef pulse_h
#define pulse_h
#include "../../sim.h"

struct pulse {
	gdouble pulse_shift;
	gdouble pulse_L;
	gdouble pulse_Rload;
	int pulse_sim_mode;
};

void sim_pulse(struct device *in);
extern struct pulse pulse_config;
void pulse_load_config(struct pulse *in, struct device *dev,
		       char *config_file_name);
void newton_aux_pulse(struct device *in, gdouble V, gdouble * i, gdouble * didv,
		      gdouble * didphi, gdouble * didxil, gdouble * didxipl,
		      gdouble * didphir, gdouble * didxir, gdouble * didxipr);
gdouble newton_pulse(struct device *in, gdouble Vtot, int usecap);

void newton_aux_pulse_voc(struct device *in, gdouble V, gdouble * i,
			  gdouble * didv, gdouble * didphi, gdouble * didxil,
			  gdouble * didxipl, gdouble * didphir,
			  gdouble * didxir, gdouble * didxipr);
gdouble pulse_newton_sim_voc_fast(struct device *in, int do_LC);
gdouble pulse_newton_sim_voc(struct device *in);
void pulse_set_light_for_voc(struct device *in, gdouble Voc);

#endif
