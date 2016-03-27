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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "sim.h"

static int unused __attribute__ ((unused));

void stop_start(struct device *in)
{
	struct timespec delay;

	if (in->stop_start == TRUE) {
		getchar();
	}

	if (in->start_stop_time != 0.0) {
		double sec = (int)in->start_stop_time;
		double ns = (in->start_stop_time - (double)sec) * 1e9;
		delay.tv_sec = (long int)sec;
		delay.tv_nsec = (long int)ns;
		//printf("here1 %lf\n",);
		//delay.tv_nsec=(long)(*1e9);
		if (nanosleep(&delay, NULL) < 0) {
			ewe("Nano sleep failed \n");
		}
	}

}
