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
#include <stdarg.h>
#include "log.h"

static int *log_level;

void log_init(struct device *in)
{
	log_level = &(in->log_level);
}

void set_logging_level(int value)
{
	(*log_level) = value;
}

void log_clear()
{
	FILE *out;
	out = fopen("log.dat", "w");
	fprintf(out, "gpvdm log file:\n");
	fclose(out);
}

void printf_log(const char *format, ...)
{
	FILE *out;
	char data[1000];
	va_list args;
	va_start(args, format);
	vsprintf(data, format, args);
	if ((*log_level == log_level_screen)
	    || (*log_level == log_level_screen_and_disk)) {
		printf("%s", data);
	}

	if ((*log_level == log_level_disk)
	    || (*log_level == log_level_screen_and_disk)) {
		out = fopen("log.dat", "a");
		fprintf(out, "%s", data);
		fclose(out);
	}

	va_end(args);
}
