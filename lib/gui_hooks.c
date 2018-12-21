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

/** @file gui_hooks.c
	@brief Handle GUI communcation dbus for Linux and pipes for windows.
*/



#define _DEFAULT_SOURCE
#include <stdio.h>
#include <sys/time.h>
#include <gui_hooks.h>
#include <util.h>
#include <const.h>
#include <log.h>
#include <string.h>

#ifdef dbus
	#include <dbus/dbus.h>
#endif


struct timeval my_last_time;

void gui_send_finished_to_gui(struct simulation *sim)
{
printf_log(sim,"finished signal=%s\n",sim->server.dbus_finish_signal);
if (strcmp(sim->server.dbus_finish_signal,"")!=0)
{
	gui_send_data(sim,sim->server.dbus_finish_signal);
}

if (strcmp(sim->server.lock_file,"")!=0)
{
	char lockname[500];
	FILE *out=fopen(sim->server.lock_file,"w");
	if (out == NULL)
	{
		printf("Problem writing file!\n");
		getchar();
	}
	fclose(out);

}




#ifdef dbus
dbus_connection_unref (sim->connection);
#endif
}

int gui_send_data (struct simulation *sim,char *tx_data_in)
{

if (sim->gui==TRUE)
{
	//printf("thinking about sending data\n");
	if ((strcmp_begin(tx_data_in,"pulse")==0)||(strcmp_begin(tx_data_in,"percent")==0))
	{
		struct timeval mytime;
		struct timeval result;

		gettimeofday (&mytime, NULL);

		timersub(&mytime,&my_last_time,&result);
		double diff=(double)result.tv_sec + ((double)result.tv_usec)/1000000.0;

		//printf("no %ld %ld %lf\n",(long)my_last_time.tv_sec,(long)my_last_time.tv_usec,diff);
		if (diff<0.15)
		{
			//printf("return\n");
			return 0;
		}

		gettimeofday (&my_last_time, NULL);
		//printf("reset\n");

	}


		//printf("sending data!!!!!!!!!!!!!!!!!!!!!!\n");
		char tx_data[1024];
		char temp[1024];
		string_to_hex(temp,tx_data_in);
		sprintf(tx_data,"hex%s",temp);

		#ifdef dbus

		DBusMessage *message;
		message = dbus_message_new_signal ("/org/my/test","org.my.gpvdm",tx_data);
		/* Send the signal */
		dbus_connection_send (sim->connection, message, NULL);
		dbus_connection_flush(sim->connection);
		dbus_message_unref (message);

		#endif


	
}
return 0;
}

int dbus_init()
{
my_last_time.tv_sec=0;
my_last_time.tv_usec=0;
return 0;
}

void gui_start(struct simulation *sim)
{
	gettimeofday (&my_last_time, NULL);
	
	#ifdef dbus
	DBusError error;
	dbus_error_init (&error);
	sim->connection = dbus_bus_get (DBUS_BUS_SESSION, &error);

	if (!sim->connection)
	{
		printf_log(sim,"Failed to connect to the D-BUS daemon: %s", error.message);
		dbus_error_free (&error);
		return;
	}
	#endif


	gui_send_data(sim,"start");
}
