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
//    the Free Software Foundation; version 2 of the License
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
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include "util.h"
#include "const.h"
#include "light.h"
#include "device.h"
#include "const.h"
#include "dump.h"
#include "config.h"
#include "inp.h"
#include "util.h"
#include "hard_limit.h"
#include "epitaxy.h"
#include "lang.h"
#include "log.h"

static int unused __attribute__ ((unused));

int light_load_laser(struct light *in, char *name)
{
	char pwd[1000];
	char file_name[255];
	struct inp_file inp;
	int ret = 0;

	if (getcwd(pwd, 1000) == NULL) {
		ewe("IO error\n");
	}

	ret = search_for_token(file_name, pwd, "#laser_name", name);

	if (ret == 0) {
		inp_init(&inp);
		inp_load_from_path(&inp, in->input_path, file_name);
		inp_check(&inp, 1.0);

		inp_search_gdouble(&inp, &in->laser_wavelength,
				   "#laserwavelength");
		in->laser_pos =
		    (int)((in->laser_wavelength - in->lstart) / in->dl);

		inp_search_gdouble(&inp, &in->spotx, "#spotx");

		inp_search_gdouble(&inp, &in->spoty, "#spoty");

		inp_search_gdouble(&inp, &in->pulseJ, "#pulseJ");

		inp_search_gdouble(&inp, &in->pulse_width,
				   "#laser_pulse_width");

		inp_free(&inp);
		printf("Loaded laser\n");
	} else {
		ewe("laser name not found\n");
	}
	return 0;
}
