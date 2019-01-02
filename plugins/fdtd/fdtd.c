//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
//
//	https://www.gpvdm.com
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

/** @file fdtd.c
	@brief Call the fdtd solver
*/

#include <sim.h>
#include "dump.h"
#include <ntricks.h>
#include <gui_hooks.h>
#include <inp.h>
#include <gui_hooks.h>
#include <cal_path.h>
#include <contacts.h>
#include <log.h>
#include <fdtd.h>
#include "fdtd_plugin.h"

static int unused __attribute__((unused));

void sim_fdtd(struct simulation *sim,struct device *in)
{
	do_fdtd(sim,in);
}



