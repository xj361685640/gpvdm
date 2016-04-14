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

static char *plugins_path;
static char *lang_path;
static char *input_path;
static char *output_path;

void cal_path_init(struct device *in)
{
	plugins_path = in->plugins_path;
	lang_path = in->lang_path;
	input_path = in->inputpath;
	output_path = in->outputpath;
}

void cal_path(struct simulation *sim, struct device *in)
{
	char share_path[1000];
	char cwd[1000];
	strcpy(cwd, "");
	strcpy(share_path, "");

	cal_path_init(in);

	if (getcwd(cwd, 1000) == NULL) {
		ewe(sim, "IO error\n");
	}

	if (isdir("/usr/lib64/gpvdm/") == 0) {
		strcpy(share_path, "/usr/lib64/gpvdm/");
	} else if (isdir("/usr/lib/gpvdm/") == 0) {
		strcpy(share_path, "/usr/lib/gpvdm/");
	} else {
		printf("I don't know where the shared files are\n");
	}

	if (isdir("plugins") == 0) {
		join_path(2, plugins_path, cwd, "plugins");
	} else {
		join_path(2, plugins_path, share_path, "plugins");

		if (isdir(plugins_path) != 0) {
			ewe(sim, "I can't find the plugins\n");
		}
	}

	if (isdir("lang") == 0) {
		join_path(2, lang_path, cwd, "lang");
	} else {
		join_path(2, lang_path, share_path, "lang");

		if (isdir(lang_path) != 0) {
			ewe(sim, "I can't find the language database.\n");
		}

	}

}

char *get_plugins_path()
{
	return plugins_path;
}

char *get_lang_path()
{
	return lang_path;
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
