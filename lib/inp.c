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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include "inp.h"
#include "util.h"
#include "code_ctrl.h"
#include "const.h"
#include <log.h>
#include <cal_path.h>

int search_for_token(struct simulation *sim,char *ret,char *dir_name,char* token,char *search_value)
{
int i=0;
int found=FALSE;
int is_sim_file=FALSE;
char found_value[256];
struct inp_file inp;
struct inp_list a;
inp_listdir(sim,dir_name,&a);


	for (i=0;i<a.len;i++)
	{
		if ((strcmp(a.names[i],".")!=0)&&(strcmp(a.names[i],"..")!=0))
		{
			if (strcmp_end(a.names[i],".inp")==0)
			{
				inp_init(sim,&inp);
				inp_load_from_path(sim,&inp,dir_name,a.names[i]);
				is_sim_file=inp_search(sim,found_value,&inp,token);
				inp_free(sim,&inp);

				if (is_sim_file==0)
				{
						if (strcmp(found_value,search_value)==0)
						{
							strcpy(ret,a.names[i]);
							found=TRUE;
							break;
						}
				}


			}
		}
	}

inp_list_free(&a);

if (found==TRUE)
{
	return 0;
}else
{
	strcpy(ret,"");
	return -1;
}

return -1;
}

int guess_whole_sim_name(struct simulation *sim,char *ret,char *dir_name,char* search_name)
{
int i=0;
int found=FALSE;
int is_sim_file=FALSE;
char sim_name[256];
char name[200];
struct inp_file inp;
struct inp_list a;
inp_listdir(sim,dir_name,&a);


	for (i=0;i<a.len;i++)
	{
		if ((strcmp(a.names[i],".")!=0)&&(strcmp(a.names[i],"..")!=0))
		{
			if (strcmp_end(a.names[i],".inp")==0)
			{
				inp_init(sim,&inp);
				inp_load_from_path(sim,&inp,dir_name,a.names[i]);
				is_sim_file=inp_search(sim,sim_name,&inp,"#sim_menu_name");
				inp_free(sim,&inp);

				if (is_sim_file==0)
				{
					if (strextract_name(name,sim_name)!= -1)
					{
						if (strcmp(name,search_name)==0)
						{
							strcpy(ret,sim_name);
							found=TRUE;
							break;
						}
					}
				}
				//if (strcmp(sim_name,search_name)==0)
				//{
				//	strcpy(ret,a.names[i]);
				//	found=TRUE;
				//	break;
				//}

			}
		}
	}

inp_list_free(&a);

if (found==TRUE)
{
	return 0;
}else
{
	strcpy(ret,"");
	return -1;
}

return -1;
}

int find_config_file(struct simulation *sim,char *ret,char *dir_name,char* search_name,char *start_of_name)
{
int i=0;
int found=FALSE;
char sim_name[256];
struct inp_file inp;
struct inp_list a;
inp_listdir(sim,dir_name,&a);


	for (i=0;i<a.len;i++)
	{
		if ((strcmp(a.names[i],".")!=0)&&(strcmp(a.names[i],"..")!=0))
		{
			if ((cmpstr_min(a.names[i],start_of_name)==0)&&(strcmp_end(a.names[i],".inp")==0))
			{
				inp_init(sim,&inp);
				inp_load_from_path(sim,&inp,dir_name,a.names[i]);
				inp_search_string(sim,&inp,sim_name,"#sim_menu_name");
				inp_free(sim,&inp);
				if (strcmp(sim_name,search_name)==0)
				{
					strcpy(ret,a.names[i]);
					found=TRUE;
					break;
				}

			}
		}
	}

inp_list_free(&a);

if (found==TRUE)
{
	return 0;
}else
{
	strcpy(ret,"");
	return -1;
}

return -1;
}


void inp_listdir(struct simulation *sim, char *dir_name,struct inp_list *out)
{
char sim_file[PATH_MAX];
int mylen=0;
int i=0;
int err = 0;
char temp[200];

join_path(2,sim_file,dir_name,"sim.gpvdm");

out->names=(char**)malloc(sizeof(char*)*2000);
out->len=0;

struct zip *z = zip_open(sim_file, 0, &err);

if (z!=NULL)
{
	int files=zip_get_num_files(z);
	for (i=0;i<files;i++)
	{
		strcpy(temp,zip_get_name(z, i, ZIP_FL_UNCHANGED));
		if (inp_listcmp(out,temp)!=0)
		{
			mylen=strlen(temp);
			out->names[out->len]=(char*)malloc(sizeof(char)*(mylen+1));
			strcpy(out->names[out->len],temp);
			out->len++;
		}

	}

	zip_close(z);

}
struct dirent *next_file;
DIR *theFolder;

theFolder = opendir(dir_name);
if (theFolder!=NULL)
{
	while((next_file=readdir(theFolder))!=NULL)
	{
		mylen=strlen(next_file->d_name);
		out->names[out->len]=(char*)malloc(sizeof(char)*(mylen+1));
		strcpy(out->names[out->len],next_file->d_name);
		out->len++;
	}

closedir (theFolder);

}

}

void inp_list_free(struct inp_list *in)
{
	int i=0;

	for (i=0;i<in->len;i++)
	{
		free(in->names[i]);
	}

	free(in->names);
}

int inp_listcmp(struct inp_list *in,char *name)
{
	int i=0;

	for (i=0;i<in->len;i++)
	{
		if (strcmp(name,in->names[i])==0)
		{
			return 0;
		}
	}

return -1;
}

int zip_is_in_archive(char *full_file_name)
{
	char zip_path[1000];
	char file_path[1000];
	get_dir_name_from_path(file_path,full_file_name);
	char *file_name=get_file_name_from_path(full_file_name);

	join_path(2,zip_path,file_path,"sim.gpvdm");

	int err = 0;
	struct zip *z = zip_open(zip_path, 0, &err);

	if (z!=NULL)
	{
		//Search for the file of given name
		struct zip_stat st;
		zip_stat_init(&st);
		int ret=zip_stat(z, file_name, 0, &st);
		zip_close(z);

		if (ret!=0)
		{
		 	return -1;
		}
		
		return 0;
	}else
	{
		return -1;
	}
}

int inp_isfile(struct simulation *sim,char *full_file_name)
{
FILE *f = fopen(full_file_name, "rb");
if (f!=NULL)
{
	sim->files_read++;
	fclose(f);
	return 0;
}else
{
	return zip_is_in_archive(full_file_name);
}
//#endif

}

int inp_search_pos(struct simulation *sim,struct inp_file *in,char *token)
{
	int pos=0;
	inp_reset_read(sim,in);
	char * line=NULL;
	do
	{
		line  = inp_get_string(sim,in);
		if (line==NULL)
		{
			break;
		}

		if (strcmp(line,token)==0)
		{
			return pos;
		}

		pos++;

	}while(1);

return -1;
}

void inp_reset_read(struct simulation *sim,struct inp_file *in)
{
in->pos=0;
}

char* inp_get_string(struct simulation *sim,struct inp_file *in)
{
int i;
static char ret[100];
memset(ret, 0, 100);
int ii=0;
if (in->pos>=in->fsize)
{
	return NULL;
}

for (i=in->pos;i<in->fsize;i++)
{
	if ((in->data[i]=='\n')||(in->data[i]==0))
	{
		ret[ii]=0;
		in->pos++;
		break;
	}

	ret[ii]=in->data[i];
	ii++;
	in->pos++;

}

return ret;
}




int inp_read_buffer(struct simulation *sim,char **buf, long *len,char *full_file_name)
{
FILE *f = fopen(full_file_name, "rb");
if (f!=NULL)
{
	sim->files_read++;
	int err=0;
	fseek(f, 0, SEEK_END);
	*len = ftell(f);
	fseek(f, 0, SEEK_SET);

	sim->bytes_read+=*len;

	log_write_file_access(sim,full_file_name,'r');

	*buf = malloc(((*len) + 1)*sizeof(char));
	//memset(*buf, 0, ((*len) + 1)*sizeof(char));
	if (fread(*buf, *len, 1, f)==0)
	{
		err= -1;
	}
	(*buf)[*len]=0;
	fclose(f);
	return err;

}else
{
	char zip_path[1000];
	char file_path[1000];
	get_dir_name_from_path(file_path,full_file_name);

	char *file_name=get_file_name_from_path(full_file_name);

	join_path(2,zip_path,file_path,"sim.gpvdm");

	int err = 0;
	struct zip *z = zip_open(zip_path, 0, &err);

	if (z!=NULL)
	{
		//Search for the file of given name
		struct zip_stat st;
		zip_stat_init(&st);
		int ret=zip_stat(z, file_name, 0, &st);

		if (ret==0)
		{
			//Alloc memory for its uncompressed contents
			*len=st.size*sizeof(char);
			*buf = (char *)malloc((*len+1)*sizeof(char));
			sim->bytes_read+=*len;

			//Read the compressed file
			struct zip_file *f = zip_fopen(z, file_name, 0);
			sim->files_read++;
			if (f==NULL)
			{
				free(buf);
				zip_close(z);
				return -1;
			}

			ret=zip_fread(f, *buf, st.size);
			if (ret==-1)
			{
				free(buf);
				zip_fclose(f);
				zip_close(z);
				return -1;
			}

			zip_fclose(f);
			(*buf)[*len]=0;
		}else
		{

			zip_close(z);
		 	return -1;
		}

		zip_close(z);

		return 0;
	}else
	{
		return -1;
	}

}

}

void inp_init(struct simulation *sim,struct inp_file *in)
{
strcpy(in->full_name,"");
in->data=NULL;
in->fsize=0;
in->pos=0;
in->edited=FALSE;
}

int inp_load_from_path(struct simulation *sim,struct inp_file *in,char *path,char *file)
{
int ret=0;
char full_path[1000];
join_path(2,full_path,path,file);
ret=inp_load(sim,in,full_path);
return ret;
}

int inp_load(struct simulation *sim,struct inp_file *in,char *file)
{
int ret=0;
in->pos=0;
if (strcmp(in->full_name,file)!=0)
{

	if (in->data!=NULL)
	{
		inp_free(sim,in);
	}

	strcpy(in->full_name,file);
	if (inp_read_buffer(sim,&(in->data),&(in->fsize),file)!=0)
	{
		ret= -1;
	}

	in->edited=FALSE;
}

return ret;
}

void inp_replace_double_offset_col(struct simulation *sim,struct inp_file *in,char *token, double value,int offset,int col)
{
	char temp[200];
	char new[200];
	inp_search_offset(sim,temp,in,token,offset);
	replace_number_in_string(new, temp, value, col);
	inp_replace_offset(sim,in,token,new,offset);
}

void inp_replace_double_offset(struct simulation *sim,struct inp_file *in,char *token, double value,int offset)
{
	char temp[100];
	sprintf(temp,"%le",value);
	inp_replace_offset(sim,in,token,temp,offset);
}

void inp_replace_double(struct simulation *sim,struct inp_file *in,char *token, double value)
{
	char temp[100];
	sprintf(temp,"%le",value);
	inp_replace(sim,in,token,temp);
}

void inp_replace(struct simulation *sim,struct inp_file *in,char *token, char *text)
{
inp_replace_offset(sim,in,token, text,0);
}

void inp_replace_offset(struct simulation *sim,struct inp_file *in,char *token, char *text,int offset)
{
int i=0;
char *temp = malloc(in->fsize + 100);
memset(temp, 0, in->fsize + 100);
char *line;
int len=0;

line = strtok(in->data, "\n");
int found=FALSE;

while(line)
{
	if (strcmp(line,token)!=0)
	{
		strcat(temp,line);
		strcat(temp,"\n");
	}else
	{
		strcat(temp,line);
		strcat(temp,"\n");

		for (i=0;i<offset;i++)
		{
			line  = strtok(NULL, "\n");
			strcat(temp,line);
			strcat(temp,"\n");
		}

		strcat(temp,text);
		strcat(temp,"\n");
		line  = strtok(NULL, "\n");
		found=TRUE;
	}
	line  = strtok(NULL, "\n");
}


len=strlen(temp);
in->fsize=len;

if (found==TRUE)
{
	in->edited=TRUE;
}

in->data=realloc(in->data,(len+1)*sizeof(char));
memcpy(in->data, temp, (len+1)*sizeof(char));

if (in->data[len]!=0)
{
	ewe(sim,"String not ended\n");
}
free(temp);
}
int zip_write_buffer(struct simulation *sim,char *full_file_name,char *buffer, int len)
{
int	in_zip_file= -1;
int	outside_zip_file= -1;

in_zip_file=zip_is_in_archive(full_file_name);
outside_zip_file=isfile(full_file_name);


if ((in_zip_file!=0)||(outside_zip_file==0))
{
	int out_fd = open(full_file_name, O_WRONLY | O_CREAT | O_TRUNC,0644);
	sim->files_written++;
	if (out_fd== -1)
	{
		ewe(sim,"File %s can not be opened\n",full_file_name);
	}

	if (ftruncate(out_fd, 0)== -1)
	{
		ewe(sim,"Error with truncate command\n");
	}

	if (write(out_fd, buffer, len*sizeof(char))== -1)
	{
		ewe(sim,"Error writing data to file.\n");
	}
	close(out_fd);
}else
{
	char zip_path[1000];
	char file_path[1000];
	get_dir_name_from_path(file_path,full_file_name);
	char *file_name=get_file_name_from_path(full_file_name);

	join_path(2,zip_path,file_path,"sim.gpvdm");

	int err = 0;
	struct zip *z = zip_open(zip_path, 0, &err);

	if (z!=NULL)
	{
		struct zip_source *s;
		s=zip_source_buffer(z, buffer, len,0);
		int index=zip_name_locate(z,  file_name,  0);

		if (index== -1)
		{
			zip_add(z, file_name, s);
			sim->files_written++;

		}else
		{
			zip_replace(z, index, s);
			sim->files_written++;
		}



		zip_close(z);
	}else
	{
		return -1;
	}

}
return 0;
}

int inp_save(struct simulation *sim,struct inp_file *in)
{

if (in->edited==TRUE)
{
	zip_write_buffer(sim,in->full_name,in->data, in->fsize);
	in->edited=FALSE;
}

return 0;
}

void inp_free(struct simulation *sim,struct inp_file *in)
{

	inp_save(sim,in);

	free(in->data);
	inp_init(sim,in);
}

void inp_search_gdouble(struct simulation *sim,struct inp_file *in,gdouble* out,char* token)
{
char temp[200];
if (inp_search(sim,temp,in,token)==0)
{
	sscanf(temp,"%Le",out);
	return;
}
ewe(sim,"token %s not found in file %s\n",token,in->full_name);
}

void inp_search_double(struct simulation *sim,struct inp_file *in,double* out,char* token)
{
char temp[200];
if (inp_search(sim,temp,in,token)==0)
{
	sscanf(temp,"%le",out);
	return;
}
ewe(sim,"token %s not found in file %s\n",token,in->full_name);
}

void inp_search_double_offset(struct simulation *sim,struct inp_file *in,double* out,char* token,int offset)
{
char temp[200];
if (inp_search_offset(sim,temp,in,token,offset)==0)
{
	sscanf(temp,"%le",out);
	return;
}
ewe(sim,"token %s not found in file %s\n",token,in->full_name);
}

int inp_search_double_offset_col(struct simulation *sim,struct inp_file *in,double* out,char* token,int offset,int col)
{
int ret=0; 
char temp[200];
if (inp_search_offset(sim,temp,in,token,offset)==0)
{
	ret=get_number_in_string(out, temp, col);
	return ret;
}
ewe(sim,"token %s not found in file %s\n",token,in->full_name);
return -1;	
}

void inp_search_int(struct simulation *sim,struct inp_file *in,int* out,char* token)
{
char temp[200];
if (inp_search(sim,temp,in,token)==0)
{
	sscanf(temp,"%d",out);
	return;
}
ewe(sim,"token %s not found in file %s\n",token,in->full_name);
}

void inp_search_string(struct simulation *sim,struct inp_file *in,char* out,char* token)
{
if (inp_search(sim,out,in,token)==0)
{
	return;
}
ewe(sim,"token %s not found in file %s\n",token,in->full_name);
}

void inp_check(struct simulation *sim,struct inp_file *in,double ver)
{
double read_ver=0.0;
	inp_reset_read(sim,in);

	char *line=inp_get_string(sim,in);
	while(line!=NULL)
	{
		if (strcmp(line,"#ver")==0)
		{
			line  = inp_get_string(sim,in);

			sscanf(line,"%lf",&(read_ver));

			if (ver!=read_ver)
			{
				ewe(sim,"File compatibility problem %s >%s< >%s< >%lf<\n",in->full_name,in->data,line,ver);
				
			}
			line  = inp_get_string(sim,in);

			if ((line==NULL)||(strcmp(line,"#end")!=0))
			{
				ewe(sim,"#end token missing %s\n",in->full_name);
			}

			inp_reset_read(sim,in);
			return;
		}

		line  = inp_get_string(sim,in);
	}

ewe(sim,"Token #ver not found in %s\n",in->full_name);
return;
}

char* inp_search_part(struct simulation *sim,struct inp_file *in,char *token)
{
	inp_reset_read(sim,in);
	char * line = inp_get_string(sim,in);
	while(line!=NULL)
	{

		if (cmpstr_min(line,token)==0)
		{
			return line;
		}

		line  = inp_get_string(sim,in);
	}

return NULL;
}


int inp_search(struct simulation *sim,char* out,struct inp_file *in,char *token)
{
return inp_search_offset(sim,out,in,token,0);
}

int inp_search_offset(struct simulation *sim,char* out,struct inp_file *in,char *token,int offset)
{
	int i;
	inp_reset_read(sim,in);
	char * line = inp_get_string(sim,in);
	while(line!=NULL)
	{

		if (strcmp(line,token)==0)
		{
			for (i=0;i<offset;i++)
			{
				line  = inp_get_string(sim,in);
				if (line==NULL)
				{
					ewe(sim,"inp_search_offset error");
				}
			}

			line  = inp_get_string(sim,in);
			strcpy(out,line);
			return 0;
		}

		line  = inp_get_string(sim,in);
	}

return -1;
}

int inp_get_array_len(struct simulation *sim,struct inp_file *in,char *token)
{
	int ret=-1;
	inp_reset_read(sim,in);
	char * line = inp_get_string(sim,in);
	while(line!=NULL)
	{
		if (strcmp(line,token)==0)
		{
			ret=0;
			do
			{
				line  = inp_get_string(sim,in);
				if (line==NULL)
				{
					break;
				}

				if (line[0]=='#')
				{
					break;
				}
				ret++;

			}while(1);

			return ret;
		}

		line  = inp_get_string(sim,in);
	}

return ret;
}

int inp_get_array_gdouble(struct simulation *sim,long double * out,struct inp_file *in,char *token)
{
	int ret=-1;
	inp_reset_read(sim,in);
	char * line = inp_get_string(sim,in);
	gdouble value;
	while(line!=NULL)
	{
		if (strcmp(line,token)==0)
		{
			ret=0;
			do
			{
				line  = inp_get_string(sim,in);

				if (line==NULL)
				{
					break;
				}

				if (line[0]=='#')
				{
					break;
				}

				sscanf(line,"%Le",&value);
				out[ret]=value;

				ret++;

			}while(1);

			return ret;
		}

		line  = inp_get_string(sim,in);
	}

return ret;
}

int inp_count_hash_tags(struct simulation *sim,struct inp_file *in)
{
	int count=0;
	inp_reset_read(sim,in);
	char *line;
	do
	{
		line  = inp_get_string(sim,in);

		if (line==NULL)
		{
			break;
		}

		if (strlen(line)>0)
		{

			if (line[0]=='#')
			{
				count++;
			}

		}

	}while(1);



return count;
}

int inp_get_array(struct simulation *sim,char ** out,struct inp_file *in,char *token)
{
	int ret=-1;
	inp_reset_read(sim,in);
	char * line = inp_get_string(sim,in);
	while(line!=NULL)
	{
		if (strcmp(line,token)==0)
		{
			ret=0;
			do
			{
				line  = inp_get_string(sim,in);

				if (line==NULL)
				{
					break;
				}

				if (line[0]=='#')
				{
					break;
				}

				strcpy(out[ret],line);

				ret++;

			}while(1);

			return ret;
		}

		line  = inp_get_string(sim,in);
	}

return ret;
}

int inp_search_english(struct simulation *sim,struct inp_file *in,char *token)
{
	inp_reset_read(sim,in);
	char * line = inp_get_string(sim,in);
	while(line!=NULL)
	{

		if (strcmp(line,token)==0)
		{
			line  = inp_get_string(sim,in);
			return english_to_bin(sim,line);
		}

		line  = inp_get_string(sim,in);
	}
ewe(sim,"Token %s not found in file %s",token,in->full_name);
exit(0);
return -1;
}

