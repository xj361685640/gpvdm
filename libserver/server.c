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
#include <stdlib.h>
#include <time.h>
#include <dump.h>
#include <unistd.h>
#include <dos.h>
#include "util.h"

#include <sys/inotify.h>
#include <sys/time.h>
#include <signal.h>
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
#include "sim.h"
#include "server.h"
#include "inp.h"
#include "timer.h"
#include "gui_hooks.h"
#include "lang.h"
#include "log.h"

static int unused __attribute__ ((unused));

static double server_jobs_per_s = 0.0;
static double server_odes_per_s = 0.0;

static time_t last_job_ended_at;

void server_update_last_job_time()
{
	last_job_ended_at = time(NULL);
}

void change_cpus(struct server *myserver)
{
}

void alarm_wakeup(int i)
{
}

int cmp_lock(char *in)
{
	return -1;
}

void server_set_dbus_finish_signal(struct server *myserver, char *signal)
{
	strcpy(myserver->dbus_finish_signal, signal);
}

void server_shut_down(struct server *myserver)
{
	server_send_finished_to_gui(&globalserver);
}

void server_send_finished_to_gui(struct server *myserver)
{
	if (strcmp(myserver->dbus_finish_signal, "") != 0) {
		gui_send_data(myserver->dbus_finish_signal);
	}
}

void print_jobs(struct server *myserver)
{
}

double server_get_odes_per_s()
{
	return server_odes_per_s;
}

double server_get_jobs_per_s()
{
	return server_jobs_per_s;
}

void server_init(struct server *myserver)
{
	strcpy(myserver->dbus_finish_signal, "");

}

int server_decode(struct simulation *sim, char *command, char *output)
{
	int odes = 0;
	return odes;
}

void server_add_job(struct simulation *sim, struct server *myserver,
		    char *command, char *output)
{
	int odes = 0;

	if (cmpstr_min(command, "gendosn") == 0) {
		gen_dos_fd_gaus_n(extract_str_number(command, "gendosn_"));
	} else if (cmpstr_min(command, "gendosp") == 0) {
		gen_dos_fd_gaus_p(extract_str_number(command, "gendosp_"));
	} else {
		odes = run_simulation(command, output);
	}
	printf_log("Solved %d ODEs\n", odes);
}

void server_exe_jobs(struct simulation *sim, struct server *myserver)
{
	if (myserver->jobs == 0)
		return;
}

void server_job_finished(struct server *myserver, char *job)
{
}

int server_run_jobs(struct simulation *sim, struct server *myserver)
{
	return 0;
}

void server_check_wall_clock(struct server *myserver)
{
	time_t now = time(NULL);
	if (now > myserver->end_time) {
		struct tm tm = *localtime(&now);

		printf_log
		    ("Server quit due to wall clock at: %d-%d-%d %d:%d:%d\n",
		     tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
		     tm.tm_min, tm.tm_sec);
		now -= myserver->start_time;
		printf_log("I have run for: %lf hours\n", now / 60.0 / 60.0);
		exit(0);
	}
}

void server_stop_and_exit()
{
	exit(0);
}
