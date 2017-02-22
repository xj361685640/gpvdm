//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.rodmack.com
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


#ifndef server_struct_h
#define server_struct_h

#define server_max 100
#define server_no_job 0
#define server_job_ready 1
#define server_job_running 2

#include <time.h>

struct server_struct
{

char command[server_max][200];
char output[server_max][200];
int state[server_max];
char dbus_finish_signal[200];
int jobs;
int jobs_running;
int cpus;
int fd;
int wd;
int on;
int readconfig;
int min_cpus;
int steel;
int max_run_time;
time_t end_time;
time_t start_time;
};

#endif
