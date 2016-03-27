//    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for 1st, 2nd and 3rd generation solar cells.
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.s

#define _DEFAULT_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <gui_hooks.h>
#include <util.h>

#ifdef dbus
#include <dbus/dbus.h>
#endif

struct timeval last_time;

int gui_send_data(char *tx_data_in)
{

	if ((strcmp_begin(tx_data_in, "pulse") == 0)
	    || (strcmp_begin(tx_data_in, "percent") == 0)) {
		struct timeval mytime;
		struct timeval result;

		gettimeofday(&mytime, NULL);

		timersub(&mytime, &last_time, &result);
		double diff = result.tv_sec + result.tv_usec / 1000000.0;

		if (diff < 0.08) {
			return 0;
		}

		last_time.tv_sec = mytime.tv_sec;
		last_time.tv_usec = mytime.tv_usec;

	}

	char tx_data[1024];
	char temp[1024];
	string_to_hex(temp, tx_data_in);
	sprintf(tx_data, "hex%s", temp);

#ifdef dbus
	DBusConnection *connection;
	DBusError error;
	dbus_error_init(&error);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &error);

	if (!connection) {
		printf("Failed to connect to the D-BUS daemon: %s",
		       error.message);
		dbus_error_free(&error);
		return 1;
	}

	DBusMessage *message;
	message =
	    dbus_message_new_signal("/org/my/test", "org.my.gpvdm", tx_data);
	/* Send the signal */
	dbus_connection_send(connection, message, NULL);
	dbus_connection_flush(connection);
	dbus_message_unref(message);
	//dbus_connection_close(connection);
#endif

	gettimeofday(&last_time, NULL);
	return 0;
}

int dbus_init()
{
	last_time.tv_sec = 0;
	last_time.tv_usec = 0;
	return 0;
}

void gui_start()
{
	gettimeofday(&last_time, NULL);
	gui_send_data("start");
}
