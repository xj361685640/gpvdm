//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.roderickmackenzie.eu
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



#include <sys/time.h>
#include <string.h>
#include "sim.h"
#include "dump.h"
#include "plot.h"
#include "util.h"

struct timeval last_time;

static char plot_script_dir[500];

void set_plot_script_dir(char * in)
{
strcpy(plot_script_dir,in);
strcat(plot_script_dir,"/plot/");
}

void plot_open(struct simulation *sim)
{
if (get_dump_status(sim,dump_plot)==TRUE)
{
gettimeofday (&last_time, NULL);
	sim->gnuplot = popen("gnuplot -persist","w");
	fprintf(sim->gnuplot, "set terminal x11 title 'General-purpose Photovoltaic Device Model - www.gpvdm.com' \n");
	fflush(sim->gnuplot);
}

}

void plot_now(struct simulation *sim,char *name)
{
struct timeval mytime;
struct timeval result;
gettimeofday (&mytime, NULL);

timersub(&mytime,&last_time,&result);
double diff=result.tv_sec + result.tv_usec/1000000.0;

if (diff<1e-1)
{
return;
}

last_time.tv_sec=mytime.tv_sec;
last_time.tv_usec=mytime.tv_usec;

if (get_dump_status(sim,dump_plot)==TRUE)
{
	fprintf(sim->gnuplot, "load '%s%s'\n",plot_script_dir,name);
	fflush(sim->gnuplot);
}
}

void plot_now_excite(struct simulation *sim)
{
if (get_dump_status(sim,dump_plot)==TRUE)
{

	fprintf(sim->gnuplot, "load 'plot_excite'\n");
	fflush(sim->gnuplot);

}
}

void plot_replot(struct simulation *sim)
{
if (get_dump_status(sim,dump_plot)==TRUE)
{

	fprintf(sim->gnuplot, "replot\n");
	fflush(sim->gnuplot);

}
}



void plot_close(struct simulation *sim)
{
if (get_dump_status(sim,dump_plot)==TRUE)
{

	fprintf(sim->gnuplot, "exit\n");
	fflush(sim->gnuplot_time);
	pclose(sim->gnuplot);

}
}
