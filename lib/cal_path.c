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

#include <limits.h>

void set_plugins_path(struct simulation *sim)
{
	char cwd[1000];
	char temp[1000];

	if (getcwd(cwd, 1000) == NULL) {
		ewe(sim, "IO error\n");
	}

	join_path(2, temp, cwd, "plugins");

	if (isdir(temp) == 0) {
		strcpy(sim->plugins_path, temp);
		return;
	}

	join_path(2, temp, sim->exe_path, "plugins");
	if (isdir(temp) == 0) {
		strcpy(sim->plugins_path, temp);
		return;
	}

	join_path(2, sim->plugins_path, sim->share_path, "plugins");

	if (isdir(sim->plugins_path) != 0) {
		ewe(sim, "I can't find the plugins\n");
	}
}

void set_lang_path(struct simulation *sim)
{
	char cwd[1000];
	char temp[1000];

	if (getcwd(cwd, 1000) == NULL) {
		ewe(sim, "IO error\n");
	}

	join_path(2, temp, cwd, "lang");

	if (isdir(temp) == 0) {
		strcpy(sim->lang_path, temp);
		return;
	}

	join_path(2, temp, sim->exe_path, "plugins");
	if (isdir(temp) == 0) {
		strcpy(sim->lang_path, temp);
		return;
	}

	join_path(2, sim->lang_path, sim->share_path, "plugins");

	if (isdir(sim->lang_path) != 0) {
		ewe(sim, "I can't find the plugins\n");
	}
}

void cal_path(struct simulation *sim)
{
	char cwd[1000];
	char temp[1000];
	strcpy(cwd, "");
	strcpy(sim->share_path, "nopath");

	strcpy(sim->plugins_path, "");
	strcpy(sim->lang_path, "");
	strcpy(sim->input_path, "");
	strcpy(sim->output_path, "");
	strcpy(sim->output_path, "");

	char buff[PATH_MAX];
	int len = readlink("/proc/self/exe", temp, 1000);
	if (len == -1) {
		ewe(sim, "IO error\n");
	}

	get_dir_name_from_path(sim->exe_path, temp);

	printf("I'm in %s\n", sim->exe_path);

	if (isfile("configure.ac") == 0) {
		strcpy(sim->share_path, cwd);
		printf("share path: %s\n", sim->share_path);
	} else if (isfile("ver.py") == 0) {
		path_up_level(temp, cwd);
		strcpy(sim->share_path, temp);
		printf("share path: %s\n", sim->share_path);
	} else {

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

	}

	if (getcwd(cwd, 1000) == NULL) {
		ewe(sim, "IO error\n");
	}

	strcpy(sim->output_path, cwd);
	strcpy(sim->input_path, cwd);

	set_plugins_path(sim);

	set_lang_path(sim);

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
