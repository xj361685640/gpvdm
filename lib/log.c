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




#include <stdio.h>
#include <stdarg.h>
#include "log.h"
#include <colors.h>
#include <time.h>
#include <stdlib.h>
#include <util.h>
#include <cal_path.h>
#include <string.h>
#include <const.h>

void log_time_stamp(struct simulation *sim)
{
	time_t t=0;
	t=time(NULL);

	struct tm tm = *localtime(&t);

	printf_log(sim,"time: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void log_clear(struct simulation *sim)
{
	FILE* out;
	char temp[500];
	join_path(2,temp,get_output_path(sim),"log.dat");
	out=fopen(temp,"w");
	fprintf(out,"gpvdm log file:\n");
	fclose(out);
}

void set_logging_level(struct simulation *sim,int value)
{
	sim->log_level=value;
}

void text_to_html(struct simulation *sim,char *out, char *in)
{
	if (sim->html==TRUE)
	{

		int i=0;
		int out_pos=0;
		int len=0;
		len=strlen(in);
		for (i=0;i<len;i++)
		{
			if (in[i]=='\n')
			{
				out[out_pos]=0;
				strcat(out,"<br>");
				out_pos=strlen(out);
			}else
			{
				out[out_pos]=in[i];
				out_pos++;
			}
			
		}
		
		out[out_pos]=0;
	}else
	{
		out[0]=0;
		strcpy(out,in);
	}
	
}

void printf_log(struct simulation *sim, const char *format, ...)
{
	FILE* out;
	char data[1000];
	char data_html[1000];
	char temp[500];
	va_list args;
	va_start(args, format);
	vsprintf(data,format, args);
	
	if ((sim->log_level==log_level_screen)||(sim->log_level==log_level_screen_and_disk))
	{
		text_to_html(sim,data_html, data);
		printf("%s",data_html);
		fflush(stdout);
	}

	if ((sim->log_level==log_level_disk)||(sim->log_level==log_level_screen_and_disk))
	{
		join_path(2,temp,get_output_path(sim),"log.dat");
		out=fopen(temp,"a");
		if (out==NULL)
		{
			printf("error: opening file %s\n",temp);
		}
		fprintf(out,"%s",data);
		fclose(out);
	}

	va_end(args);
}

void waveprint(struct simulation *sim,char *in,double wavelength)
{
	//Remove tralining \n in html mode as div introduces one anyway
	/*if (sim->html==TRUE)
	{
		int len=strlen(in)-1;
		if (len>0)
		{
			if (in[len]=='\n')
			{
				in[len]=0;
			}
		}
	}*/

	if ((sim->log_level==log_level_screen)||(sim->log_level==log_level_screen_and_disk))
	{
		if (wavelength<400.0)
		{
			textcolor(sim,fg_purple);
		}else
		if (wavelength<500.0)
		{
			textcolor(sim,fg_blue);
		}else
		if (wavelength<575.0)
		{
			textcolor(sim,fg_green);
		}else
		if (wavelength<600.0)
		{
			textcolor(sim,fg_yellow);
		}else
		{
			textcolor(sim,fg_red);

		}
	}

	printf_log(sim,"%s",in);

	if ((sim->log_level==log_level_screen)||(sim->log_level==log_level_screen_and_disk))
	{
			textcolor(sim,fg_reset);
	}
}

void textcolor(struct simulation *sim, int color)
{
if (sim->html==TRUE)
{
	if (color==fg_purple)
	{
		printf("<span style=\"color:purple\">");
	}else
	if (color==fg_blue)
	{
		printf("<span style=\"color:blue\">");
	}else
	if (color==fg_green)
	{
		printf("<span style=\"color:green\">");
	}else
	if (color==fg_yellow)
	{
		printf("<span style=\"color:yellow\">");
	}else
	if (color==fg_red)
	{
		printf("<span style=\"color:red\">");
	}else
	if (color==fg_reset)
	{
		printf("</span>");
	}

}else
{
	char command[13];
	sprintf(command, "\e[%dm", color);
	printf("%s", command);
}
}

int log_search_error(char *path)
{
	int ret=-1;
    FILE * fp;
    char * line = NULL;
    ssize_t read;
	size_t len = 0;
    fp = fopen(path, "r");

    if (fp == NULL)
	{
       return ret;
	}

    while ((read = getline(&line, &len, fp)) != -1)
	{
		if (strcmp_begin(line,"error:")==0)
		{
			ret=0;
			break;
		}

    }

    fclose(fp);

    if (line)
	{
        free(line);
	}
    return ret;
}
