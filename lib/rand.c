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
#include <math.h>
#include "inp.h"

#include "util.h"
#include <const.h>
#include "rand.h"

int random_int_range(struct simulation *sim, int start_in, int stop_in)
{
	int start = start_in;
	int stop = stop_in;

	int temp = 0;

	if (start > stop) {
		temp = start;
		start = stop;
		stop = temp;
	}
	int ret = 0;
	int delta = (stop - start);

	ret = random_int(sim, delta);

	return start + ret;
}

int random_int(struct simulation *sim, int in)
{
	if (in == 0)
		return 0;
	int out = 0;
	int random;
	FILE *fp = fopen("/dev/urandom", "r");
	if (fread(&random, sizeof(int), 1, fp) == 0) {
		ewe(sim, "No data read from urandom\n");
	}
	random = fabs(random);
	out = random % (in + 1);
//printf("%d\n", out);
	fclose(fp);
	return out;
}

void randomize_input_files(struct simulation *sim)
{
	struct inp_file inp;
	inp_init(sim, &inp);

	struct inp_file ifile;
	inp_init(sim, &ifile);

	inp_load(sim, &inp, "random.inp");
	inp_check(sim, &inp, 1.0);
	inp_reset_read(sim, &inp);
	char *data;
	char file[100];
	char token[100];
	double man_min = 0.0;
	double man_max = 0.0;
	double exp_min = 0.0;
	double exp_max = 0.0;
	double a = 0.0;
	double b = 0.0;
	double value = 0.0;
	char value_string[100];
	do {
		data = inp_get_string(sim, &inp);

		if (strcmp(data, "#ver") == 0) {
			break;
		}

		sscanf(data, "%s %s %le %le %le %le", file, token, &man_min,
		       &man_max, &exp_min, &exp_max);
		//printf("%s '%s' %f %f %f %f\n",file,token,man_min,man_max,exp_min,exp_max);
		a = (double)random_int_range(sim, man_min, man_max);
		b = (double)random_int_range(sim, exp_min, exp_max);
		value = a * pow(10, b);
		sprintf(value_string, "%le", value);
		inp_load(sim, &ifile, file);
		inp_replace(sim, &ifile, token, value_string);
		inp_free(sim, &ifile);

		//printf("%f %f %le\n",a,b,value);

	} while (1);

	inp_free(sim, &inp);

}
