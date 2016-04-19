//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//	roderick.mackenzie@nottingham.ac.uk
//	www.roderickmackenzie.eu
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
#include "lang.h"
#include "log.h"

static int unused __attribute__((unused));

void light_load_materials(struct simulation *sim,struct light *in)
{
int i=0;
char fit_file[1000];
char materialsdir[200];
char file_path[400];

DIR *theFolder;
char pwd[1000];
char temp[1000];
if (getcwd(pwd,1000)==NULL)
{
	ewe(sim,"Error getting directory path\n");
}
struct inp_file inp;

join_path(2,temp,pwd,"materials");
int found=FALSE;
theFolder = opendir(temp);
if (theFolder!=NULL)
{
	closedir (theFolder);
	strcpy(materialsdir,temp);
	found=TRUE;
}


if (found==FALSE)
{
	inp_init(sim,&inp);
	if (inp_load(sim,&inp,"materialsdir.inp")!=0)
	{
		ewe(sim,"File materialsdir.inp not found in %s",pwd);
	}

	inp_check(sim,&inp,1.0);
	inp_search_string(sim,&inp,temp,"#materialsdir");
	inp_free(sim,&inp);

	theFolder = opendir(temp);
	if (theFolder!=NULL)
	{
		closedir (theFolder);
		strcpy(materialsdir,temp);
		found=TRUE;
	}
}

if (found==FALSE)
{
	strcpy(temp,"/usr/share/gpvdm/materials");

	theFolder = opendir(temp);
	if (theFolder!=NULL)
	{
		closedir (theFolder);
		strcpy(materialsdir,temp);
		found=TRUE;
	}
}

if (found==FALSE)
{
	ewe(sim,_("No optical materials found\n"));
}

join_path(2,file_path,materialsdir,in->suns_spectrum_file);

inter_load(&(in->sun_read),file_path);
inter_sort(&(in->sun_read));

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
for (i=0;i<in->layers;i++)
{
	join_path(3, fit_file,materialsdir,in->material_dir_name[i],"fit.inp");

	inp_load(sim,&inp,fit_file);

	inp_search_gdouble(sim,&inp,&alpha_mul,"#alpha_mul");
	alpha_mul=fabs(alpha_mul);
	hard_limit(sim,"#alpha_mul",&alpha_mul);

	inp_search_gdouble(sim,&inp,&n_mul,"#n_mul");
	n_mul=fabs(n_mul);
	hard_limit(sim,"#n_mul",&n_mul);

	inp_search_gdouble(sim,&inp,&wavelength_shift_n,"#wavelength_shift_n");
	wavelength_shift_n-=40e-9;
	hard_limit(sim,"#wavelength_shift_n",&wavelength_shift_n);

	inp_search_gdouble(sim,&inp,&wavelength_shift_alpha,"#wavelength_shift_alpha");
	wavelength_shift_alpha-=40.0e-9;

	hard_limit(sim,"#wavelength_shift_alpha",&wavelength_shift_alpha);

	inp_search_int(sim,&inp,&patch,"#patch");

	inp_search_int(sim,&inp,&inter,"#inter");

	inp_search_int(sim,&inp,&spectrum,"#spectrum");

	inp_free(sim,&inp);

	join_path(3, file_path,materialsdir,in->material_dir_name[i],"alpha.omat");
	inter_load(&(in->mat[i]),file_path);
	inter_sort(&(in->mat[i]));

	join_path(3,file_path,materialsdir,in->material_dir_name[i],"n.omat");
	inter_load(&(in->mat_n[i]),file_path);
	//printf("%s\n",file_path);
	//inter_dump(&in->mat_n[i]);
	//getchar();

	inter_sort(&(in->mat_n[i]));

	//struct istruct den;
	//inter_init_mesh(&den,1000,2e-7,7e-7);
	//inter_to_new_mesh(&(in->mat[i]),&den);
	//join_path(3, out_file,materialsdir,in->material_dir_name[i],"inter_n.dat");
	//inter_save(&den,out_file);
	//inter_free(&den);

	inter_mul(&(in->mat[i]),alpha_mul);
	inter_add_x(&(in->mat[i]),wavelength_shift_alpha);

	inter_mul(&(in->mat_n[i]),n_mul);
	inter_add_x(&(in->mat_n[i]),wavelength_shift_n);

	if (patch==TRUE)
	{
		join_path(3, patch_file,materialsdir,in->material_dir_name[i],"patch.inp");
		
		FILE* patch_in=fopen(patch_file,"r");
		if (in==NULL)
		{
			ewe(sim,"file %s not found\n",patch_file);
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
							//printf("add=%le\n",add);
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
							//printf("add=%Le %Le\n",add,c);
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

			join_path(3, patch_file,materialsdir,in->material_dir_name[i],"inter.inp");

			patch_in=fopen(patch_file,"r");
			if (in==NULL)
			{
				ewe(sim,"file %s not found\n",patch_file);
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
				printf_log(sim,"Error at end of inter file\n");
				exit(0);
			}

			fclose(patch_in);
		}

		join_path(3, out_file,materialsdir,in->material_dir_name[i],"n_out.dat");
		inter_save(&(in->mat_n[i]),out_file);

		join_path(3, out_file,materialsdir,in->material_dir_name[i],"alpha_out.dat");
		inter_save(&(in->mat[i]),out_file);
	}

	if (spectrum==TRUE)
	{
		inter_free(&(in->mat_n[i]));

		join_path(3, patch_file,materialsdir,in->material_dir_name[i],"n_spectrum.inp");

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
		inter_init_mesh(&(in->mat_n[i]),n,start,stop);
		for (ii=0;ii<n;ii++)
		{
			unused=fscanf(f_in,"%Le",&value);
			in->mat_n[i].data[ii]=value;
		}
		fclose(f_in);

		join_path(3, out_file,materialsdir,in->material_dir_name[i],"n_out.dat");
		inter_save(&(in->mat_n[i]),out_file);

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

