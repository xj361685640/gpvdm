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

static int unused __attribute__((unused));

struct simulation *local_sim;



static double server_jobs_per_s=0.0;
static double server_odes_per_s=0.0;

static time_t last_job_ended_at;


void server_update_last_job_time()
{
last_job_ended_at=time(NULL);
}

void change_cpus(struct simulation *sim,struct server_struct *myserver)
{
}

void alarm_wakeup (int i)
{
}


int cmp_lock(char *in)
{
return -1;
}

void server_set_dbus_finish_signal(struct server_struct *myserver, char *signal)
{
	strcpy(myserver->dbus_finish_signal,signal);
}

void server_shut_down(struct simulation *sim,struct server_struct *myserver)
{
	gui_send_finished_to_gui(sim);
}

void print_jobs(struct simulation *sim)
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

void server_init(struct simulation *sim)
{
local_sim=sim;

	strcpy(sim->server.dbus_finish_signal,"");

}


int server_decode(struct simulation *sim,char *command)
{
int odes=0;
return odes;
}

void server_add_job(struct simulation *sim,char *command,char *output)
{
	int odes=0;

	if (cmpstr_min(command,"gendosn")==0)
	{
		gen_dos_fd_gaus_n(sim,extract_str_number(command,"gendosn_"));
	}else
	if (cmpstr_min(command,"gendosp")==0)
	{
		gen_dos_fd_gaus_p(sim,extract_str_number(command,"gendosp_"));
	}else
	{
		odes=run_simulation(sim);
	}
	printf_log(sim,"Solved %d ODEs\n",odes);
}


void server_exe_jobs(struct simulation *sim, struct server_struct *myserver)
{
if (myserver->jobs==0) return;
}


void server_job_finished(struct server_struct *myserver,char *job)
{
}

int server_run_jobs(struct simulation *sim,struct server_struct *myserver)
{
	return 0;
}

void server_check_wall_clock(struct simulation *sim,struct server_struct *myserver)
{
time_t now = time(NULL);
if (now>myserver->end_time)
{
struct tm tm = *localtime(&now);

printf_log(sim,"Server quit due to wall clock at: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
now-=myserver->start_time;
printf_log(sim,"I have run for: %lf hours\n",now/60.0/60.0);
exit(0);
}
}

void server_stop_and_exit()
{
	exit(0);
}
