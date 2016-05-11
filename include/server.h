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


#ifndef serverh
#define serverh

#include <sim_struct.h>
#include <server_struct.h>
struct my_msgbuf {
    long mtype;
    char mtext[200];
};


void server_stop_and_exit();
void server_shut_down(struct simulation *sim,struct server_struct *myserver);
void server_add_job(struct simulation *sim,char *command,char *output);
void print_jobs(struct simulation *sim);
void server_init(struct simulation *sim);
void server_exe_jobs(struct simulation *sim, struct server_struct *myserver);
void server_job_finished(struct server_struct *myserver,char *job);
int server_run_jobs(struct simulation *sim,struct server_struct *myserver);
double server_get_odes_per_s();
double server_get_jobs_per_s();
void change_cpus(struct simulation *sim,struct server_struct *myserver);
void server_check_wall_clock(struct simulation *sim,struct server_struct *myserver);
void server_update_last_job_time();
void server_set_dbus_finish_signal(struct server_struct *myserver, char *signal);
void server_send_finished_to_gui(struct simulation *sim);
#endif
