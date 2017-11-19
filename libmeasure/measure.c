//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


#include "util.h"
#include "const.h"
#include "light.h"
#include "device.h"
#include "const.h"
#include <dump.h>
#include "config.h"
#include "inp.h"
#include "util.h"
#include "cal_path.h"
#include "lang.h"
#include "log.h"
#include "measure.h"
#include <probe.h>
#include <buffer.h>
#include <sys/stat.h>
#include <i.h>
#include <rpn.h>

static int unused __attribute__((unused));

struct probe_config config;

static struct istruct spectrum_first;
static int first=FALSE;

void measure_file(struct simulation *sim,char *file_name)
{
	int i;
	int vector;
	int enable;
	char vec_str[400];
	char file[400];
	char data_path[400];
	long double position;
	char output_token[400];
	char temp[400];
	char sim_name[400];

	char math[400];
	FILE *out;
	struct inp_file inp;
	struct inp_file inp_data;
	char input_position[400];
	char output_file[400];

	struct rpn rpn_cal;
	long double value;
	long double y;
	int write=FALSE;
	int ret=0;
	strcpy(vec_str,"");

	join_path(2, file,get_input_path(sim),file_name);

	inp_init(sim,&inp);
	if (inp_load(sim, &inp , file)!=0)
	{
		ewe(sim,"I can't find file %s\n",file);
	}

	inp_check(sim,&inp,1.0);
	inp_reset_read(sim,&inp);
	struct istruct data;
	inter_init(sim,&data);

	inp_get_string(sim,&inp);	//name
	strcpy(sim_name,inp_get_string(sim,&inp));	//

	inp_get_string(sim,&inp);	//enable
	strcpy(temp,inp_get_string(sim,&inp));
	enable=english_to_bin(sim,temp);

	if (enable==FALSE)
	{
		inp_free(sim,&inp);
		return;
	}

	inp_get_string(sim,&inp);	//compile to vector
	strcpy(temp,inp_get_string(sim,&inp));
	vector=english_to_bin(sim,temp);

	sprintf(output_file,"measure_%s.dat",sim_name);

	out=fopena(get_output_path(sim),output_file,"w");


	while(1)
	{
		write=FALSE;

		strcpy(temp,inp_get_string(sim,&inp));	//file token
		if (strcmp(temp,"#ver")==0)
		{
			break;
		}

		strcpy(file_name,inp_get_string(sim,&inp));		//file
		
		inp_get_string(sim,&inp);	//position token
		strcpy(input_position,inp_get_string(sim,&inp));
		
		inp_get_string(sim,&inp);	//output
		
		strcpy(output_token,inp_get_string(sim,&inp));	//output token
		//printf("%s %Lf %s\n",file_name,position,output_token);

		join_path(2,data_path,get_output_path(sim),file_name);

		inp_get_string(sim,&inp);
		strcpy(math,inp_get_string(sim,&inp));	//output token
		
		if (strcmp_begin(input_position,"#")==0)
		{
				inp_init(sim,&inp_data);
				if (inp_load(sim, &inp_data , data_path)==0)
				{
					ret=inp_search_gdouble(sim,&inp_data,&value,input_position);

					if (ret==0)
					{
						rpn_init(sim,&rpn_cal);
						rpn_add_var(sim,&rpn_cal,"x",(double)value);

						y=(long double)rpn_evaluate(sim,&rpn_cal,math);
						//printf("%Le %s %Le %s\n",y,math,value,input_position);
						//getchar();
						inp_free(sim,&inp_data);
						write=TRUE;
					}else
					{
						printf_log(sim,_("Token not found: %s"),input_position);
					}

				}else
				{
					printf_log(sim,_("File not found: %s"),data_path);
				}
		}else
		{
			if (inter_load(sim,&data,data_path)==0)
			{
				inter_sort(&data);
				//inter_dump(sim,&data);
				sscanf(input_position,"%Le",&(position));
				y=inter_get_hard(&data,position);

				rpn_init(sim,&rpn_cal);
				rpn_add_var(sim,&rpn_cal,"x",(double)y);

				y=(long double)rpn_evaluate(sim,&rpn_cal,math);
				
				inter_free(&data);
				write=TRUE;
			}
		}
		
		if (write==TRUE)
		{
			if (vector==FALSE)
			{
				fprintf(out,"#%s\n",output_token);
				fprintf(out,"%Le\n",y);
			}else
			{
				sprintf(temp,"%Le ",y);
				strcat(vec_str,temp);
			}
		}

	}

	inp_free(sim,&inp);

	if (vector==FALSE)
	{
		fprintf(out,"#ver\n");
		fprintf(out,"#1.0\n");
		fprintf(out,"#end\n");
	}else
	{
		fprintf(out,"%s",vec_str);	
	}

	fclose(out);
}


void measure(struct simulation *sim)
{
int i=0;
struct inp_list a;
inp_listdir(sim,get_input_path(sim),&a);


	for (i=0;i<a.len;i++)
	{
		if ((strcmp(a.names[i],".")!=0)&&(strcmp(a.names[i],"..")!=0))
		{
			if ((cmpstr_min(a.names[i],"measure")==0)&&(strcmp_end(a.names[i],".inp")==0))
			{
				measure_file(sim,a.names[i]);
			}
		}
	}

inp_list_free(&a);
}
