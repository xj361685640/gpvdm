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
#include <log.h>

void cal_path(struct simulation *sim)
{
	char cwd[1000];
	strcpy(cwd, "");
	strcpy(sim->share_path, "nopath");

	strcpy(sim->plugins_path, "");
	strcpy(sim->lang_path, "");
	strcpy(sim->input_path, "");
	strcpy(sim->output_path, "");

	if (getcwd(cwd, 1000) == NULL) {
		ewe(sim, "IO error\n");
	}

	if (isdir("/usr/lib64/gpvdm/") == 0) {
		strcpy(sim->share_path, "/usr/lib64/gpvdm/");
	} else if (isdir("/usr/lib/gpvdm/") == 0) {
		strcpy(sim->share_path, "/usr/lib/gpvdm/");
	} else {
		strcpy(sim->share_path, "/usr/lib/gpvdm/");
		printf_log(sim,
			   "I don't know where the shared files are assuming %s\n",
			   sim->share_path);
	}

	if (isdir("plugins") == 0) {
		join_path(2, sim->plugins_path, cwd, "plugins");
	} else {
		join_path(2, sim->plugins_path, sim->share_path, "plugins");

		if (isdir(sim->plugins_path) != 0) {
			ewe(sim, "I can't find the plugins\n");
		}
	}

	if (isdir("lang") == 0) {
		join_path(2, sim->lang_path, cwd, "lang");
	} else {
		join_path(2, sim->lang_path, sim->share_path, "lang");

		if (isdir(sim->lang_path) != 0) {
			ewe(sim, "I can't find the language database.\n");
		}

	}

}

char *get_plugins_path(struct simulation *sim)
{
	return sim->plugins_path;
}

char *get_lang_path(struct simulation *sim)
{
	return sim->lang_path;
}

char *get_input_path(struct simulation *sim)
{
	return sim->input_path;
}

char *get_output_path(struct simulation *sim)
{
	return sim->output_path;
}

void set_output_path(struct simulation *sim, char *in)
{
	strcpy(sim->output_path, in);
}

void set_input_path(struct simulation *sim, char *in)
{
	strcpy(sim->input_path, in);
}
