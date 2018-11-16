//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
#include "tx_packet.h"
#include <pthread.h>

struct state *local_sim;
struct node_struct nodes[100];


int send_command(int sock,char *command,char *dir_name,int cpus)
{
	int ret=0;
	struct tx_struct packet;
	tx_struct_init(&packet);
	tx_set_id(&packet,"gpvdmcommand");
	strcpy(packet.exe_name,command);
	strcpy(packet.dir_name,dir_name);
	packet.cpus=cpus;

	ret=tx_packet(sock,&packet,NULL);


return ret;
}

void* exec_command(void *in)
{
	char sim_dir[200];
	struct tx_struct data;
	copy_packet(&data,(struct tx_struct *)in);
	
	join_path(2,sim_dir,calpath_get_store_path(), data.dir_name);
	printf("change dir to %s\n",sim_dir);

	
	chdir(sim_dir);

	char full_exe_path[400];
	char lib_path[400];
	char command[400];
	printf("command>>>>>>=%s\n",data.exe_name);
	join_path(3,full_exe_path,calpath_get_store_path(), "src",data.exe_name);
	join_path(2,lib_path,calpath_get_store_path(), "src");
	sprintf(command,"export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:%s;stdbuf -i0 -o0 -e0 %s",lib_path,full_exe_path);
	printf("full command =%s\n",command);
	system(command);
	


	//send_dir(sock, sim_dir, 0, sim_dir, data.dir_name);

	printf("FINISHED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %s\n",data.dir_name);

	struct tx_struct packet;
	tx_struct_init(&packet);
	tx_set_id(&packet,"gpvdmsimfinished");
	strcpy(packet.dir_name,data.dir_name);
	strcpy(packet.ip,get_my_ip());
	packet.cpus=data.cpus;

	tx_packet(local_sim->head_sock,&packet,NULL);

	return NULL;
}

int cmp_node_runjob(struct state *sim,struct tx_struct *data)
{
	local_sim=sim;
	if (cmpstr_min(data->id,"gpvdmcommand")==0)
	{

		printf("I will run %s in a new process\n",data->exe_name);

		pthread_t thread1;
		printf("command>>>>>>>=%s\n",data->exe_name);
		pthread_create( &thread1, NULL,exec_command,(void*)data);
		sleep(1);		//This is super bad, but I want to give it enough time to copy the data in.
		return 0;
	}

return -1;
}


void* command_thread(void *in)
{
	struct tx_struct data;
	copy_packet(&data,(struct tx_struct *)in);
	FILE *fp;
	//int status;
	int max_len=300;
	char path[max_len];
	char sim_dir[200];
	join_path(2,sim_dir,calpath_get_store_path(), data.dir_name);
	printf("change dir to %s\n",sim_dir);
	chdir(sim_dir);


	fp = popen(data.command, "r");

	while (fgets(path, max_len, fp) != NULL)
	{
		printf("output>>%s", path);
		send_message(path);
	}

	//status = pclose(fp);

	//system(data.command);

	if (local_sim->state==HEAD)
	{
		char temp[500];
		sprintf(temp,"I have run: %s", data.command);
		send_message(temp);
	}
	return NULL;
}




int cmp_head_exe(struct state *sim,int sock,struct tx_struct *data)
{
local_sim=sim;

	if (cmpstr_min(data->id,"gpvdmheadexe")==0)
	{


		printf("I will run %s\n",data->command);
		if (sim->state==HEAD)
		{
			char temp[500];
			sprintf(temp,"running: %s", data->command);
			send_message(temp);
		}

		pthread_create( &sim->thread_command, NULL,command_thread,(void*)data);
		sleep(1);		//This is super bad, but I want to give it enough time to copy the data in.
		return 0;
	}

return -1;
}

