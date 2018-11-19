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


void *rx_loop(void *s)
{
struct state *sim=get_sim();

int ret=0;
int processed=FALSE;
struct tx_struct data;
struct tx_struct last_data;
int sock=*((int*)s);
char ip[100];
get_ip_from_sock(ip,sock);
printf("rx loop for ip=%s\n",ip);

	while(1)
	{
		processed=FALSE;
		ret=rx_packet(sock,&data);

		if (ret==-1)
		{
			get_ip_from_sock(ip,sock);
			node_delete(ip);
			printf("ip=%s\n",ip);
			printf("Breaking due to -1\n");
			break;
		}

		if (ret!=-2)
		{
			
			if (cmp_rxfile(sock,&data,sim)==0)
			{
				debug_printf("a\n");
				processed=TRUE;
			}else
			if (cmp_addjob(sock,&data)==0)
			{
				debug_printf("b\n");
				processed=TRUE;
			}else
			if (cmp_addnode(sock,&data)==0)
			{
				debug_printf("c\n");
				processed=TRUE;
			}else
			if (cmp_deletenode(sock,&data)==0)
			{
				debug_printf("d\n");
				processed=TRUE;
			}else
			if (cmp_runjobs(sim,sock,&data)==0)
			{
				debug_printf("e\n");
				processed=TRUE;
			}else
			if (cmp_simfinished(sim,sock,&data)==0)
			{
				debug_printf("f\n");
				processed=TRUE;
			}else
			if (cmp_register_master(sock,&data)==0)
			{
				debug_printf("g\n");
				processed=TRUE;
			}else
			if (cmp_head_killall(sock,&data)==0)
			{
				debug_printf("h\n");
				processed=TRUE;
			}else
			if (cmp_head_sleep(sock,&data)==0)
			{
				debug_printf("i\n");
				processed=TRUE;
			}else
			if (cmp_head_poweroff(sock,&data)==0)
			{
				debug_printf("j\n");
				processed=TRUE;
			}else
			if (cmp_sendnodelist(sock,&data)==0)
			{
				debug_printf("k\n");
				processed=TRUE;
			}else
			if (cmp_get_data(sim,sock,&data)==0)
			{
				debug_printf("l\n");
				processed=TRUE;
			}else
			if (cmp_head_exe(sim,sock,&data)==0)
			{
				debug_printf("m\n");
				processed=TRUE;
			}else
			if (cmp_master_clean(sock,&data)==0)
			{
				debug_printf("n\n");
				processed=TRUE;
			}else
			if (cmp_rxloadstats(sock,&data)==0)
			{
				debug_printf("o\n");
				processed=TRUE;
			}else
			if (cmp_head_quit(sock,&data)==0)
			{
				debug_printf("p\n");
				processed=TRUE;
			}else
			if (cmp_rxsetmaxloads(sock,&data)==0)
			{
				debug_printf("1\n");
				processed=TRUE;
			}else
			if (cmp_sync_packet_one(sock,&data)==0)
			{
				debug_printf("2\n");
				processed=TRUE;
			}else
			if (cmp_sync_packet_two(sim,sock,&data)==0)
			{
				debug_printf("3\n");
				processed=TRUE;
			}else
			if (cmp_send_job_list(sock,&data)==0)
			{
				debug_printf("4\n");
				processed=TRUE;
			}else
			if (cmp_head_stop_all_jobs(sock,&data)==0)
			{
				debug_printf("5\n");
				processed=TRUE;
			}else
			if (cmp_delete_all_jobs(sock,&data)==0)
			{
				debug_printf("6\n");
				processed=TRUE;
			}
			
			if (processed==FALSE)
			{
				if (sim->state==HEAD)
				{
					char temp[500];
					sprintf(temp,"command not understood last:%s this:%s", last_data.id, data.id);
					send_message(temp);
				}
			}

			jobs_save();
			copy_packet(&last_data,&data);
		}

	}

	

close(sock);
printf("leavng main rx loop for %s\n",ip);
return NULL;
}
