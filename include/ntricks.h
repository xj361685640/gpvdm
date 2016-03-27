//    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for 1st, 2nd and 3rd generation solar cells.
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//	roderick.mackenzie@nottingham.ac.uk
//	www.roderickmackenzie.eu
//	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
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

#ifndef ntricks_h
#define ntricks_h

struct newton_math_state
{
int max_electrical_itt;
gdouble min_cur_error;
int newton_min_itt;
gdouble electrical_clamp;
int newton_clever_exit;
};

void newton_push_state(struct device *in);
void newton_pop_state(struct device *in);
gdouble sim_externalv(struct device *in,gdouble wantedv);
gdouble sim_i(struct device *in,gdouble wantedi);
void save_state(struct device *in,gdouble to);
int load_state(struct device *in,gdouble voltage);
void ramp(struct device *in,gdouble from,gdouble to,gdouble steps);
void ramp_externalv(struct device *in,gdouble from,gdouble to);
void set_ntricks_fast(int val);
gdouble sim_voc(struct device *in);

void ntricks_externv_set_load(gdouble R);
void ntricks_externv_newton_aux(struct device *in,gdouble V,gdouble* i,gdouble* didv,gdouble* didphi,gdouble* didxil,gdouble* didxipl,gdouble* didphir,gdouble* didxir,gdouble* didxipr);
gdouble ntricks_externv_newton(struct device *in,gdouble Vtot,int usecap);

#endif
