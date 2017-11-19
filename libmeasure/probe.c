//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


#include "util.h"
#include "const.h"
#include "light.h"
#include "device.h"
#include "const.h"
#include <dump.h>
#include "config.h"
#include "inp.h"
#include "util.h"
#include "cal_path.h"
#include "lang.h"
#include "log.h"
#include <probe.h>
#include <buffer.h>
#include <sys/stat.h>
#include <i.h>

static int unused __attribute__((unused));

struct probe_config config;

static struct istruct spectrum_first;
//static struct istruct spectrum_reflect_first;
//static struct istruct reflect;
static int first=FALSE;
static int probe_enable=FALSE;

gdouble probe_cal(struct simulation *sim,struct device *in,	gdouble wavelength)
{
}

void dump_probe_spectrum(struct simulation *sim,struct device *in,char *out_dir,int dump_number)
{
}

void probe_record_step(struct simulation *sim,struct device *in)
{
}

void probe_dump(struct simulation *sim,struct device *in)
{
}
void probe_init(struct simulation *sim,struct device *in)
{
}

void probe_free(struct simulation *sim,struct device *in)
{
}

