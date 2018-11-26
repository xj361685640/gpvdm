//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	https://www.gpvdm.com
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "inp.h"
#include "util.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include "inp.h"
#include "tx_packet.h"
#include <pthread.h>

int cmp_node_killjob(int sock,struct tx_struct *data)
{
int     status;

	if (cmpstr_min(data->id,"kill_job")==0)
	{
		int pid=(int)data->pid;
		kill_all(pid);
		wait(&status);
		return 0;
	}

return -1;
}

//This should be removed
int cmp_node_killall(int sock,struct tx_struct *data)
{
	if (cmpstr_min(data->id,"gpvdmnodekillall")==0)
	{
		printf("killall gpvdm_core\n");
		system("killall gpvdm_core");
		return 0;
	}

return -1;
}

int cmp_head_killall(int sock,struct tx_struct *data)
{
	int i=0;
	struct job *jobs=NULL;
	struct tx_struct packet;
	int njobs=0;
	if (cmpstr_min(data->id,"gpvdmkillall")==0)
	{
		jobs=get_jobs_array();
		njobs=get_njobs();
		for (i=0;i<njobs;i++)
		{
			if (jobs[i].pid!=-1)
			{
				tx_struct_init(&packet);
				tx_set_id(&packet,"kill_job");
				packet.pid=jobs[i].pid;
				send_packet_to_node(jobs[i].ip,&packet);
			}
		}
		

		return 0;
	}

return -1;
}

int cmp_head_stop_all_jobs(int sock,struct tx_struct *data)
{
	if (cmpstr_min(data->id,"gpvdm_stop_all_jobs")==0)
	{
		stop_all_jobs();
		return 0;
	}

return -1;
}

int cmp_delete_all_jobs(int sock,struct tx_struct *data)
{
	if (cmpstr_min(data->id,"gpvdm_delete_all_jobs")==0)
	{
		jobs_clear_all();
		return 0;
	}

return -1;
}

int cmp_head_stop_running_jobs(int sock,struct tx_struct *data)
{
	if (cmpstr_min(data->id,"gpvdm_stop_all_jobs")==0)
	{
		stop_all_jobs();
		return 0;
	}

return -1;
}
