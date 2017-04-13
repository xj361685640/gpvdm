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



#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "util.h"
#include "const.h"
#include "light.h"
#include "device.h"
#include "const.h"
#include "dump.h"
#include "config.h"
#include "inp.h"
#include "util.h"
#include "hard_limit.h"
#include "epitaxy.h"
#include <lang.h>
#include "log.h"
#include <cal_path.h>
#include <buffer.h>

static int unused __attribute__((unused));

void light_load_materials(struct simulation *sim,struct light *in)
{
printf_log(sim,"%s\n",_("load: materials"));
struct buffer buf;
int i=0;
char temp[100];
char fit_file[1000];
char file_path[400];

DIR *theFolder;

struct inp_file inp;

/////////////////////////////////////////////////////
theFolder = opendir(get_spectra_path(sim));
if (theFolder==NULL)
{
	ewe(sim,_("Optical spectra directory not found\n"));
}
closedir (theFolder);
inp_init(sim,&inp);

join_path(3,file_path,get_spectra_path(sim),in->suns_spectrum_file,"mat.inp");

inp_load(sim,&inp,file_path);
inp_search_string(sim,&inp,temp,"#spectra_equation_or_data");

inp_free(sim,&inp);
	

	if (strcmp(temp,"equation")==0)
	{
		join_path(3, file_path,get_spectra_path(sim),in->suns_spectrum_file,"spectra_gen.inp");
	}else
	if (strcmp(temp,"data")==0)
	{
		join_path(3,file_path,get_spectra_path(sim),in->suns_spectrum_file,"spectra.inp");
	}else
	{
		ewe(sim,"%s: %s\n",_("spectrum file: I do not know what to do with %s"),temp);		
	}

	if (isfile(file_path)!=0)
	{
		ewe(sim,"%s: %s\n",_("File not found"),file_path);
	}
	
inter_load(sim,&(in->sun_read),file_path);
inter_sort(&(in->sun_read));



/////////////////////////////////////////////////////
theFolder = opendir(get_materials_path(sim));
if (theFolder==NULL)
{
	ewe(sim,_("No optical materials found\n"));
}
closedir (theFolder);


in->mat=(struct istruct *)malloc(in->layers*sizeof(struct istruct));
in->mat_n=(struct istruct *)malloc(in->layers*sizeof(struct istruct));

gdouble alpha_mul=1.0;
gdouble n_mul=1.0;
gdouble wavelength_shift_n=0.0;
gdouble wavelength_shift_alpha=0.0;
int patch=FALSE;
int inter=FALSE;

inp_init(sim,&inp);
char patch_file[400];
char out_file[400];
char token[400];
int ii=0;
gdouble b=0.0;
gdouble a=0.0;
gdouble c=0.0;
char type[40];
int spectrum=FALSE;
int spectrum_alpha=FALSE;

for (i=0;i<in->layers;i++)
{
	//fit.inp
	join_path(3, fit_file,get_materials_path(sim),in->material_dir_name[i],"fit.inp");

	inp_load(sim,&inp,fit_file);

	inp_search_gdouble(sim,&inp,&alpha_mul,"#alpha_mul");
	alpha_mul=fabs(alpha_mul);
	hard_limit(sim,"#alpha_mul",&alpha_mul);

	inp_search_gdouble(sim,&inp,&n_mul,"#n_mul");
	n_mul=fabs(n_mul);
	hard_limit(sim,"#n_mul",&n_mul);

	inp_search_gdouble(sim,&inp,&wavelength_shift_n,"#wavelength_shift_n");
	hard_limit(sim,"#wavelength_shift_n",&wavelength_shift_n);

	wavelength_shift_n-=40e-9;

	inp_search_gdouble(sim,&inp,&wavelength_shift_alpha,"#wavelength_shift_alpha");
	hard_limit(sim,"#wavelength_shift_alpha",&wavelength_shift_alpha);

	wavelength_shift_alpha-=40.0e-9;


	inp_search_int(sim,&inp,&patch,"#patch");

	inp_search_int(sim,&inp,&inter,"#inter");

	inp_search_int(sim,&inp,&spectrum,"#spectrum");

	inp_search_int(sim,&inp,&spectrum_alpha,"#spectrum_alpha");

	inp_free(sim,&inp);
	
	//mat.inp
	join_path(3, fit_file,get_materials_path(sim),in->material_dir_name[i],"mat.inp");

	inp_load(sim,&inp,fit_file);
	
	char default_file_alpha[100];
	char default_file_n[100];
	inp_search_string(sim,&inp,default_file_alpha,"#mat_default_file_alpha");
	inp_search_string(sim,&inp,default_file_n,"#mat_default_file_n");
	inp_free(sim,&inp);
	
	if (strcmp(default_file_alpha,"equation")==0)
	{
		join_path(3, file_path,get_materials_path(sim),in->material_dir_name[i],"alpha_gen.omat");
	}else
	{

		join_path(3, file_path,get_materials_path(sim),in->material_dir_name[i],"alpha.omat");
		if (isfile(file_path)!=0)
		{
			join_path(3, file_path,get_materials_path(sim),in->material_dir_name[i],"alpha_gen.omat");
		}
	}

	if (isfile(file_path)!=0)
	{
		ewe(sim,"%s: %s\n",_("File not found"),file_path);
	}

	inter_load(sim,&(in->mat[i]),file_path);
	inter_sort(&(in->mat[i]));

	if (strcmp(default_file_n,"equation")==0)
	{
		join_path(3, file_path,get_materials_path(sim),in->material_dir_name[i],"n_gen.omat");
	}else
	{
		join_path(3,file_path,get_materials_path(sim),in->material_dir_name[i],"n.omat");
		if (isfile(file_path)!=0)
		{
			join_path(3, file_path,get_materials_path(sim),in->material_dir_name[i],"n_gen.omat");
		}
	}

	if (isfile(file_path)!=0)
	{
		ewe(sim,"%s: %s\n",_("File not found"),file_path);
	}

	inter_load(sim,&(in->mat_n[i]),file_path);
	//printf_log(sim,"%s\n",file_path);
	//inter_dump(&in->mat_n[i]);
	//getchar();

	inter_sort(&(in->mat_n[i]));

	//struct istruct den;
	//inter_init_mesh(&den,1000,2e-7,7e-7);
	//inter_to_new_mesh(&(in->mat[i]),&den);
	//join_path(3, out_file,get_materials_path(sim),in->material_dir_name[i],"inter_n.dat");
	//inter_save(&den,out_file);
	//inter_free(&den);

	inter_mul(&(in->mat[i]),alpha_mul);
	inter_add_x(&(in->mat[i]),wavelength_shift_alpha);

	inter_mul(&(in->mat_n[i]),n_mul);
	inter_add_x(&(in->mat_n[i]),wavelength_shift_n);

	if (patch==TRUE)
	{
		join_path(3, patch_file,get_materials_path(sim),in->material_dir_name[i],"patch.inp");

		FILE* patch_in=fopen(patch_file,"r");
		if (in==NULL)
		{
			ewe(sim,"%s: %s\n",_("file not found"),patch_file);
		}

		do
		{
			unused=fscanf(patch_in,"%s",token);
			if (strcmp(token,"#end")==0)
			{
				break;
			}
				unused=fscanf(patch_in,"%s",type);

				unused=fscanf(patch_in,"%s",token);
				unused=fscanf(patch_in,"%Le",&a);

				unused=fscanf(patch_in,"%s",token);
				unused=fscanf(patch_in,"%Le",&b);

				unused=fscanf(patch_in,"%s",token);
				unused=fscanf(patch_in,"%Le",&c);

				if (strcmp(type,"bar_n")==0)
				{
					hard_limit(sim,token,&c);
					c=fabs(c);
					for (ii=0;ii<in->mat_n[i].len;ii++)
					{
						if ((in->mat_n[i].x[ii]>=a)&&(in->mat_n[i].x[ii]<=b))
						{
							in->mat_n[i].data[ii]=c;
						}
					}
				}else
				if (strcmp(type,"bar_alpha")==0)
				{
					hard_limit(sim,token,&c);
					c=fabs(c);
					for (ii=0;ii<in->mat[i].len;ii++)
					{
						if ((in->mat[i].x[ii]>=a)&&(in->mat[i].x[ii]<=b))
						{
							in->mat[i].data[ii]=c;
						}
					}
				}else
				if (strcmp(type,"gaus")==0)
				{
					hard_limit(sim,token,&c);
					c=fabs(c);
					gdouble add=0.0;
					int max_pos=inter_search_pos(&(in->mat_n[i]),a);
					gdouble subtract=in->mat_n[i].data[max_pos];
					b=fabs(b);
					for (ii=0;ii<in->mat_n[i].len;ii++)
					{
							add=(c-subtract)*exp(-gpow(((in->mat_n[i].x[ii]-a)/(sqrt(2.0)*b)),2.0));
							in->mat_n[i].data[ii]+=add;
							//printf_log(sim,"add=%le\n",add);
					}
				}else
				if (strcmp(type,"gaus_math")==0)
				{
					printf_log(sim,"gaus math\n");
					gdouble add=0.0;
					b=fabs(b);
					for (ii=0;ii<in->mat_n[i].len;ii++)
					{
							add=c*exp(-gpow(((in->mat_n[i].x[ii]-a)/(sqrt(2.0)*b)),2.0));
							in->mat_n[i].data[ii]+=add;
							//printf_log(sim,"add=%Le %Le\n",add,c);
					}
				}
		}while(!feof(patch_in));

		if (strcmp(token,"#end")!=0)
		{
			printf_log(sim,_("Error at end of patch file\n"));
			exit(0);
		}

		fclose(patch_in);

		if (inter==TRUE)
		{

			join_path(3, patch_file,get_materials_path(sim),in->material_dir_name[i],"inter.inp");

			patch_in=fopen(patch_file,"r");
			if (in==NULL)
			{
				ewe(sim,"%s: %s\n",_("file not found"),patch_file);
			}

			gdouble from=0.0;
			gdouble to=0.0;

			do
			{
				unused=fscanf(patch_in,"%s",token);
				if (strcmp(token,"#end")==0)
				{
					break;
				}

				unused=fscanf(patch_in,"%Le %Le",&from,&to);

				//for n
				int x0=inter_search_pos(&(in->mat_n[i]),from);
				int x1=inter_search_pos(&(in->mat_n[i]),to);
				gdouble y0=in->mat_n[i].data[x0];
				gdouble y1=in->mat_n[i].data[x1];
				gdouble step=(y1-y0)/((gdouble)(x1-x0));
				gdouble pos=y0;
				for (ii=x0;ii<x1;ii++)
				{
					in->mat_n[i].data[ii]=pos;
					pos+=step;
				}
				//for alpha
				x0=inter_search_pos(&(in->mat[i]),from);
				x1=inter_search_pos(&(in->mat[i]),to);
				y0=in->mat[i].data[x0];
				y1=in->mat[i].data[x1];
				step=(y1-y0)/((gdouble)(x1-x0));
				pos=y0;
				for (ii=x0;ii<x1;ii++)
				{
					in->mat[i].data[ii]=pos;
					pos+=step;
				}


			}while(!feof(patch_in));

			if (strcmp(token,"#end")!=0)
			{
				printf_log(sim,"%s\n",_("Error at end of inter file"));
				exit(0);
			}

			fclose(patch_in);
		}


	}

	if (spectrum==TRUE)
	{
		inter_free(&(in->mat_n[i]));

		join_path(3, patch_file,get_materials_path(sim),in->material_dir_name[i],"n_spectrum.inp");

		FILE *f_in=fopen(patch_file,"r");

		if (f_in==NULL)
		{
			ewe(sim,"%s: %s \n",_("file not found"),patch_file);
		}

		int n=0;
		gdouble value=0.0;
		gdouble start=0.0;
		gdouble stop=0.0;
		unused=fscanf(f_in,"%s",token);
		unused=fscanf(f_in,"%Le",&start);
		unused=fscanf(f_in,"%s",token);
		unused=fscanf(f_in,"%Le",&stop);
		unused=fscanf(f_in,"%s",token);
		unused=fscanf(f_in,"%d",&n);
		unused=fscanf(f_in,"%s",token);
		inter_init_mesh(&(in->mat_n[i]),n,start,stop);
		for (ii=0;ii<n;ii++)
		{
			unused=fscanf(f_in,"%Le",&value);
			in->mat_n[i].data[ii]=value;
		}
		fclose(f_in);

		join_path(2, out_file,get_materials_path(sim),in->material_dir_name[i]);

		buffer_malloc(&buf);
		buf.y_mul=1.0;
		buf.x_mul=1e9;
		strcpy(buf.title,_("Wavelength - Reflected light"));
		strcpy(buf.type,"xy");
		strcpy(buf.x_label,_("Wavelength"));
		strcpy(buf.data_label,"n");
		strcpy(buf.x_units,"nm");
		strcpy(buf.data_units,"a.u.");
		buf.logscale_x=0;
		buf.logscale_y=0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf,in->mat_n[i].x, in->mat_n[i].data, in->mat_n[i].len);
		buffer_dump_path(sim,out_file,"n_out.dat",&buf);
		buffer_free(&buf);

	}

	if (spectrum_alpha==TRUE)
	{
		inter_free(&(in->mat[i]));

		join_path(3, patch_file,get_materials_path(sim),in->material_dir_name[i],"alpha_spectrum.inp");

		FILE *f_in=fopen(patch_file,"r");

		if (f_in==NULL)
		{
			ewe(sim,"file %s not found\n",patch_file);
		}

		int n=0;
		gdouble value=0.0;
		gdouble start=0.0;
		gdouble stop=0.0;
		unused=fscanf(f_in,"%s",token);
		unused=fscanf(f_in,"%Le",&start);
		unused=fscanf(f_in,"%s",token);
		unused=fscanf(f_in,"%Le",&stop);
		unused=fscanf(f_in,"%s",token);
		unused=fscanf(f_in,"%d",&n);
		unused=fscanf(f_in,"%s",token);
		inter_init_mesh(&(in->mat[i]),n,start,stop);
		for (ii=0;ii<n;ii++)
		{
			unused=fscanf(f_in,"%Le",&value);
			in->mat[i].data[ii]=value;
		}
		fclose(f_in);

		join_path(2, out_file,get_materials_path(sim),in->material_dir_name[i]);

		buffer_malloc(&buf);
		buf.y_mul=1.0;
		buf.x_mul=1e9;
		strcpy(buf.title,_("Wavelength - Reflected light"));
		strcpy(buf.type,"xy");
		strcpy(buf.x_label,_("Wavelength"));
		strcpy(buf.y_label,"alpha");
		strcpy(buf.x_units,"nm");
		strcpy(buf.y_units,"a.u.");
		buf.logscale_x=0;
		buf.logscale_y=0;
		buffer_add_info(&buf);
		buffer_add_xy_data(&buf,in->mat[i].x, in->mat[i].data, in->mat[i].len);
		buffer_dump_path(sim,out_file,"alpha_out.dat",&buf);
		buffer_free(&buf);

	}
}

}


void light_free_materials(struct light *in)
{
int i;
	for (i=0;i<in->layers;i++)
	{
		inter_free(&(in->mat[i]));
		inter_free(&(in->mat_n[i]));
	}
	free(in->mat);
	free(in->mat_n);
}

