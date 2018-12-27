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

/** @file probe.h
@brief Stark probe code
*/


#ifndef _probe
#define _probe
#include <sim.h>
#include <device.h>

struct probe_config
{
struct istruct *time_stark;
gdouble *probe_wavelength;
int n_probe_wavelength;
gdouble stark_mul;
int use_ss_spectra;
};

gdouble probe_cal(struct simulation *sim,struct device *in,	gdouble wavelength);
void dump_probe_spectrum(struct simulation *sim,struct device *in,char *out_dir, int dump_number);
void probe_init(struct simulation *sim,struct device *in);
void probe_free(struct simulation *sim,struct device *in);
void probe_record_step(struct simulation *sim,struct device *in);
void probe_dump(struct simulation *sim,struct device *in);
#endif
