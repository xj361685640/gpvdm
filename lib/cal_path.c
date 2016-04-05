//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cal_path.h"
#include "util.h"
#include "inp.h"

static char *share_path;
static char *light_path;
static char *solver_path;
static char *lang_path;
static char *input_path;
static char *output_path;

void cal_path_init(struct device *in)
{
	share_path = in->share_path;
	light_path = in->light_path;
	solver_path = in->solver_path;
	lang_path = in->lang_path;
	input_path = in->inputpath;
	output_path = in->outputpath;
}

void cal_path(struct device *in)
{
	cal_path_init(in);

	if (isfile("configure.ac") == 0) {
		if (getcwd(share_path, 1000) == NULL) {
			ewe("IO error\n");
		}
	} else {
		if (isdir("/usr/lib64/gpvdm/") == 0) {
			strcpy(share_path, "/usr/lib64/gpvdm/");
		} else if (isdir("/usr/lib/gpvdm/") == 0) {
			strcpy(share_path, "/usr/lib/gpvdm/");
		} else {
			ewe("I don't know where the shared files are\n");
		}
	}
	join_path(2, light_path, share_path, "plugins");
	join_path(2, solver_path, share_path, "plugins");
	join_path(2, lang_path, share_path, "lang");

}

char *get_light_path()
{
	return light_path;
}

char *get_solver_path()
{
	return solver_path;
}

char *get_lang_path()
{
	return lang_path;
}

char *get_share_path()
{
	return share_path;
}

char *get_input_path()
{
	return input_path;
}

char *get_output_path()
{
	return output_path;
}

void set_output_path(char *in)
{
	strcpy(output_path, in);
}

void set_input_path(char *in)
{
	strcpy(input_path, in);
}
