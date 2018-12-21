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

/** @file exit.c
	@brief Exit while sending sane data to the gui, also handle crashes while fitting.
*/


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "log.h"
#include "const.h"
//#include "dump_ctrl.h"
//#include "dos_types.h"
#include "gui_hooks.h"
#include "lang.h"
#include <signal.h>
#include <server.h>
#include <unistd.h>

static char lock_name[100];
static char lock_data[100];


void set_ewe_lock_file(char *lockname,char *data)
{
strcpy(lock_name,lockname);
strcpy(lock_data,data);
}

int ewe( struct simulation *sim, const char *format, ...)
{
	FILE* out;
	char temp[1000];
	char temp2[1000];
	va_list args;
	va_start(args, format);
	vsprintf(temp,format, args);

	sprintf(temp2,"error:%s",temp);

	printf_log(sim, "%s\n",temp2);

	//gui_send_data(temp2);

	va_end(args);





	gui_send_finished_to_gui(sim);


	if (strcmp(lock_name,"")!=0)
	{
		out=fopen(lock_name,"w");
		fprintf(out,"%s",lock_data);
		fclose(out);
	}
	//printf_log(sim,"Raising segmentation fault\n");
	//raise(SIGSEGV);
exit(1);
}


