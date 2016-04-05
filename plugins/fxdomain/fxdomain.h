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

#ifndef fxdomain_h
#define fxdomain_h
#include <sim.h>

struct fxdomain {
	int fxdomain_sim_mode;
	gdouble fxdomain_Rload;
	int fxdomain_points;
	int fxdomain_n;
	gdouble fxdomain_Vexternal;
	gdouble fxdomain_voltage_modulation_max;
	gdouble fxdomain_light_modulation_max;
	int fxdomain_do_fit;
	int fxdomain_voltage_modulation;
	gdouble fxdomain_L;
	int periods_to_fit;

};

void sim_fxdomain(struct device *in);
extern struct fxdomain fxdomain_config;
void fxdomain_load_config(struct fxdomain *in, struct device *dev,
			  char *config_file_name);
void newton_aux_fxdomain(struct device *in, gdouble V, gdouble * i,
			 gdouble * didv, gdouble * didphi, gdouble * didxil,
			 gdouble * didxipl, gdouble * didphir, gdouble * didxir,
			 gdouble * didxipr);
gdouble newton_fxdomain(struct device *in, gdouble Vtot, int usecap);

void newton_aux_fxdomain_voc(struct device *in, gdouble V, gdouble * i,
			     gdouble * didv, gdouble * didphi, gdouble * didxil,
			     gdouble * didxipl, gdouble * didphir,
			     gdouble * didxir, gdouble * didxipr);
gdouble fxdomain_newton_sim_voc_fast(struct device *in, int do_LC);
gdouble fxdomain_newton_sim_voc(struct device *in);
void fxdomain_set_light_for_voc(struct device *in, gdouble Voc);

#endif
