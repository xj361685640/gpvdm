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
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include "util.h"
#include "log.h"
#include <const.h>
#include <lang.h>
#include <math.h>
#include <ctype.h>
#include <cal_path.h>

static char* unused_pchar __attribute__((unused));

/**Get length of a file in lines
@param file_name file name
*/
int get_file_len(struct simulation *sim,char *file_name)
{
FILE *file;
if (!(file=fopen(file_name,"r")))
{
	printf_log(sim,"Error opening file %s\n",file_name);
	exit(0);
}
char buffer[1000];

int i;
i=0;
char *p;
do
{
	buffer[0]=0;
	unused_pchar=fgets(buffer, 1000, file);
	p=buffer;
	if (buffer[0]=='#')
	{
		i--;
	}else
	{
		//Check for empty line
		while(*p==' ' || *p=='\t') p++;
		if ((*p=='\r')||(*p=='\n')||(*p==0)) i--;
	}
i++;
}while(!feof(file));
//i--;
fclose(file);
return i;
}

void split_dot(char *out, char *in)
{
	int i=0;	
	strcpy(out,in);
	for (i=0;i<strlen(out);i++)
	{
		if (out[i]=='.')
		{
			out[i]=0;
			break;
		}
	}
}


void fx_with_units(char *out,double number)
{
	if (number<1e3)
	{
		sprintf(out,"%.3lf Hz",number);
	}
	else
	if (number<1e6)
	{
		sprintf(out,"%.3lf KHz",number*1e-3);
	}
	else
	if (number<1e9)
	{
		sprintf(out,"%.3lf MHz",number*1e-6);
	}
	else
	if (number<1e12)
	{
		sprintf(out,"%.3lf GHz",number*1e-9);
	}

}

void time_with_units(char *out,double number)
{
	double val=fabs(number);
	if (val>=1.0)
	{
		sprintf(out,"%.3lf s",number);
	}else
	if (val>=1e-3)
	{
		sprintf(out,"%.3lf ms",number/1e-3);
	}else
	if (val>=1e-6)
	{
		sprintf(out,"%.3lf us",number/1e-6);
	}else
	if (val>=1e-9)
	{
		sprintf(out,"%.3lf ns",number/1e-9);
	}else
	if (val>=1e-12)
	{
		sprintf(out,"%.3lf ps",number/1e-12);
	}else
	if (val>=1e-15)
	{
		sprintf(out,"%.3lf fs",number/1e-15);
	}else
	if (val>=1e-18)
	{
		sprintf(out,"%.3lf as",number/1e-18);
	}else
	{
		sprintf(out,"%.3lf s",number);
	}


}


void print_hex(struct simulation *sim,unsigned char *data)
{
int i;
for (i=0;i<16;i++)
{
	printf_log(sim,"%02x",data[i]);
}
printf_log(sim,"\n");
}


void write_x_y_to_file(struct simulation *sim,char *name,double *x,double *y,int len)
{
int i;
FILE *out;

	out=fopen(name,"w");
	if (out==NULL)
	{
		ewe(sim,"%s %s\n",_("Error writing file"),name);
	}

	for (i=0;i<len;i++)
	{
		fprintf(out,"%le %le\n",x[i],y[i]);
	}
	fclose(out);
}

void write_x_y_z_to_file(struct simulation *sim,char *name,double *x,double *y,double *z,int len)
{
int i;
FILE *out;

	out=fopen(name,"w");
	if (out==NULL)
	{
		ewe(sim,"%s %s\n",_("Error writing file"),name);
	}

	for (i=0;i<len;i++)
	{
		fprintf(out,"%le %le %le\n",x[i],y[i],z[i]);
	}
	fclose(out);
}

void str_to_lower(char *out, char *in)
{
	int i=0;
	for (i=0;i<strlen(in);i++)
	{
		out[i]=tolower(in[i]);
	}
	out[i]=0;

}

int check_int(char *in)
{
int i=0;
int numeric=TRUE;
for (i=0;i<strlen(in);i++)
{
	if ((in[i]<48)||(in[i]>57))
	{
		numeric=FALSE;
		break;
	}
}
return numeric;
}
static int unused __attribute__((unused));

int english_to_bin(struct simulation *sim, char * in)
{
char temp[100];
int ret=0;

str_to_lower(temp, in);

if (check_int(temp)==TRUE)
{
sscanf(temp,"%d",&ret);
return ret;
}

if (strcmp(temp,"true")==0)
{
	return TRUE;
}else
if (strcmp(temp,"false")==0)
{
	return FALSE;
}else
if (strcmp(temp,"1")==0)
{
	return TRUE;
}else
if (strcmp(temp,"0")==0)
{
	return FALSE;
}else
if (strcmp(temp,"yes")==0)
{
	return TRUE;
}else
if (strcmp(temp,"no")==0)
{
	return FALSE;
}else
if (strcmp(temp,"left")==0)
{
	return LEFT;
}else
if (strcmp(temp,"links")==0)
{
	return LEFT;
}else
if (strcmp(temp,"ja")==0)
{
	return TRUE;
}else
if (strcmp(temp,"nein")==0)
{
	return FALSE;
}else
if (strcmp(temp,"right")==0)
{
	return RIGHT;
}else
if (strcmp(temp,"rechts")==0)
{
	return RIGHT;

}else
if (strcmp(temp,"gaus")==0)
{
	return 0;
}else
if (strcmp(temp,"exp")==0)
{
	return 1;
}else
if (strcmp(temp,"exponential")==0)
{
	return dos_exp;
}else
if (strcmp(temp,"complex")==0)
{
	return dos_an;
}
else
if (strcmp(temp,"open_circuit")==0)
{
	return pulse_open_circuit;
}else
if (strcmp(temp,"load")==0)
{
	return pulse_load;
}else
if (strcmp(temp,"ideal_diode_ideal_load")==0)
{
	return pulse_ideal_diode_ideal_load;
}else
if (strcmp(temp,"none")==0)
{
	return log_level_none;
}else
if (strcmp(temp,"screen")==0)
{
	return log_level_screen;
}else
if (strcmp(temp,"disk")==0)
{
	return log_level_disk;
}else
if (strcmp(temp,"screen_and_disk")==0)
{
	return log_level_screen_and_disk;
}else
if (strcmp(temp,"newton")==0)
{
	return FIT_NEWTON;
}else
if (strcmp(temp,"simplex")==0)
{
	return FIT_SIMPLEX;
}else
if (strcmp(temp,"bfgs")==0)
{
	return FIT_BFGS;
}else
if (strcmp(temp,"top")==0)
{
	return TOP;
}else
if (strcmp(temp,"bottom")==0)
{
	return BOTTOM;
}



ewe(sim,"%s %s\n",_("I don't understand the command"),in);
return 0;
}

double read_value(struct simulation *sim,char *file,int skip,int line)
{
FILE *in;
char buf[1000];
double value;
in=fopen(file,"r");
if (in==NULL)
{
ewe(sim,"%s %s\n",_("Can not read file"),file);
}
int l=0;

do
{
l++;
	unused=fscanf(in,"%s",buf);

	if (l==line)
	{
		sscanf((buf+skip),"%le\n",&value);
		break;
	}


}while (!feof(in));

fclose(in);


return value;
}

void safe_file(struct simulation *sim,char *name)
{
FILE *file;
file = fopen(name, "rb");

if (!file)
{
	ewe(sim,"%s: %s\n",_("File not found"),name);
}

fclose(file);
}




void randomprint(struct simulation *sim,char *in)
{
	int i;
	for (i=0;i<strlen(in);i++)
	{
	int rnd=(float)5.0*rand()/(float)RAND_MAX;
		if (rnd==0) textcolor(sim,fg_wight);
		if (rnd==1) textcolor(sim,fg_red);
		if (rnd==2) textcolor(sim,fg_green);
		if (rnd==3) textcolor(sim,fg_yellow);
		if (rnd==4) textcolor(sim,fg_blue);
		if (rnd==5) textcolor(sim,fg_purple);
		
		if ((in[i]!='\n')||(sim->html==FALSE))
		{
			printf_log(sim,"%c",in[i]);
		}else
		{
			printf_log(sim,"<br>");
		}

		textcolor(sim,fg_reset);
					
		}

fflush(stdout);
}

FILE *fopena(char *path,char *name,const char *mode)
{
char wholename[200];
join_path(2, wholename,path,name);

FILE *pointer;
pointer=fopen(wholename,mode);

return pointer;
}


int scanarg( char* in[],int count,char * find)
{
int i;
for (i=0;i<count;i++)
{
if (strcmp(in[i],find)==0) return TRUE;
}
return FALSE;
}

int get_arg_plusone_pos( char* in[],int count,char * find)
{
int i;
for (i=0;i<count;i++)
{
if (strcmp(in[i],find)==0)
{
       if ((i+1)<count)
       {
               return i+1;
       }else
       {
               return FALSE;
       }
}
}
return FALSE;
}

char * get_arg_plusone( char* in[],int count,char * find)
{
int i;
static char no[] = "";
for (i=0;i<count;i++)
{

if (strcmp(in[i],find)==0)
{
       if ((i+1)<count)
       {
               return in[i+1];
       }else
       {
               return no;
       }
}
}

return no;
}


void edit_file_int(struct simulation *sim,char *in_name,char *front,int line_to_edit,int value)
{

FILE *in;
FILE *out;
char *line;
int file_size =0;
in=fopen(in_name,"r");
int pos=0;
char temp[200];
if (in==NULL)
{
	ewe(sim,"edit_file_by_var: %s %s\n",_("File not found"),in_name);
}
fseek(in, 0, SEEK_END);
file_size = ftell(in);
fseek(in, 0, SEEK_SET);

char *in_buf = malloc(file_size + 1);
memset(in_buf, 0, (file_size + 1)*sizeof(char));

fread(in_buf, file_size, 1, in);
in_buf[file_size] = 0;
fclose(in);

char *out_buf= out_buf=malloc((file_size+100)*sizeof(char));
memset(out_buf, 0, (file_size+100)*sizeof(char));


line = strtok(in_buf, "\n");
while(line)
{
pos++;
	if (pos!=line_to_edit)
	{
		strcat(out_buf,line);
		strcat(out_buf,"\n");
	}else
	{
		sprintf(temp,"%s%d\n",front,value);
		strcat(out_buf,temp);
	}
	line  = strtok(NULL, "\n");
}

free(in_buf);

out=fopen(in_name,"w");
if (in==NULL)
{
	ewe(sim,"edit_file_by_var: %s %s \n",_("Can not write file"),in_name);
}
fwrite(out_buf, strlen(out_buf), 1, out);
free(out_buf);
fclose(out);

}



void edit_file(struct simulation *sim,char *in_name,char *front,int line_to_edit,double value)
{

FILE *in;
FILE *out;
char *line;
int file_size =0;
in=fopen(in_name,"r");
int pos=0;
char temp[200];
if (in==NULL)
{
	ewe(sim,"edit_file_by_var: %s %s \n",_("File not found"),in_name);
}
fseek(in, 0, SEEK_END);
file_size = ftell(in);
fseek(in, 0, SEEK_SET);

char *in_buf = malloc(file_size + 1);
memset(in_buf, 0, (file_size + 1)*sizeof(char));

fread(in_buf, file_size, 1, in);
in_buf[file_size] = 0;
fclose(in);

char *out_buf= out_buf=malloc((file_size+100)*sizeof(char));
memset(out_buf, 0, (file_size+100)*sizeof(char));


line = strtok(in_buf, "\n");
while(line)
{
pos++;
	if (pos!=line_to_edit)
	{
		strcat(out_buf,line);
		strcat(out_buf,"\n");
	}else
	{
		sprintf(temp,"%s%le\n",front,value);
		strcat(out_buf,temp);
	}
	line  = strtok(NULL, "\n");
}

free(in_buf);

out=fopen(in_name,"w");
if (in==NULL)
{
	ewe(sim,"edit_file_by_var: %s %s \n",_("Can not write file"),in_name);
}
fwrite(out_buf, strlen(out_buf), 1, out);
free(out_buf);
fclose(out);

}

void copy_file(struct simulation *sim,char *output,char *input)
{
char buf[8192];
struct stat results;
int in_fd = open(input, O_RDONLY);
if (in_fd== -1)
{
	ewe(sim,"%s: %s\n",_("Can not open file"),input);
}

stat(input, &results);

int out_fd = open(output, O_WRONLY | O_CREAT| O_TRUNC,results.st_mode);
if (in_fd== -1)
{
	ewe(sim,"%s: %s\n",_("Can not open file"),output);
}


while (1)
{
    ssize_t result = read(in_fd, buf, 8192*sizeof(char));

    if (result==0)
	{
		break;
	}
    write(out_fd, buf, result*sizeof(char));
}

close(in_fd);
close(out_fd);
}

void edit_file_by_var(struct simulation *sim,char *in_name,char *token,char *newtext)
{
FILE *in;
FILE *out;
char *line;
int found=FALSE;
int file_size =0;
in=fopen(in_name,"r");
if (in==NULL)
{
	ewe(sim,"edit_file_by_var: %s %s\n",_("File not found"),in_name);
}
fseek(in, 0, SEEK_END);
file_size = ftell(in);
fseek(in, 0, SEEK_SET);

char *in_buf = malloc(file_size + 1);
memset(in_buf, 0, (file_size + 1)*sizeof(char));

fread(in_buf, file_size, 1, in);
in_buf[file_size] = 0;
fclose(in);

char *out_buf= out_buf=malloc((file_size+strlen(newtext)+10)*sizeof(char));
memset(out_buf, 0, (file_size+strlen(newtext)+10)*sizeof(char));


line = strtok(in_buf, "\n");
while(line)
{
	if (strcmp(line,token)!=0)
	{
		strcat(out_buf,line);
		strcat(out_buf,"\n");
	}else
	{
		strcat(out_buf,line);
		strcat(out_buf,"\n");
		strcat(out_buf,newtext);
		strcat(out_buf,"\n");
		line  = strtok(NULL, "\n");
		found=TRUE;
	}
	line  = strtok(NULL, "\n");
}

if (found==FALSE)
{
	ewe(sim,"edit_file_by_var: %s %s\n",_("Token not found in file"),token);
}

free(in_buf);

out=fopen(in_name,"w");
if (in==NULL)
{
	ewe(sim,"edit_file_by_var: %s %s \n",_("Can not write file"),in_name);
}
fwrite(out_buf, strlen(out_buf), 1, out);
free(out_buf);
fclose(out);

}

int path_up_level(char *out, char *in)
{
int i=0;
strcpy(out,in);
int len=strlen(out);
if (len<1)
{
	return -1;
}

if (len!=3)
{
	if (out[len-1]=='\\')
	{
		out[len-1]=0;
		len=strlen(out);
	}
}

if (len!=1)
{
	if (out[len-1]=='/')
	{
		out[len-1]=0;
		len=strlen(out);
	}
}

for (i=len;i>=0;i--)
{

		if (out[i]=='\\')
		{
			out[i+1]=0;
			break;
		}



		if (out[i]=='/')
		{
			out[i+1]=0;
			break;
		}
}

return 0;
}

char *get_file_name_from_path(char *in)
{
int i=0;
for (i=strlen(in)-1;i>0;i--)
{
		if (in[i]=='/')
		{
			return (in+i+1);
		}
}
return in;
}

int get_dir_name_from_path(char *out, char *in)
{
strcpy(out,in);

int i=0;
int len=strlen(in);
for (i=len;i>0;i--)
{
		if (in[i]=='/')
		{
			out[i]=0;
			return 0;
		}
}

if (len>0)
{
	out[0]=0;
	return 0;
}

return -1;
}

int isdir(const char *path)
{
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
	{
		return 1;
	}else
	{
		if (S_ISDIR(statbuf.st_mode)==0)
		{
			return 1;
		}else
		{
			return 0;
		}
	}
}

int isfile(char *in)
{
	struct stat statbuf;
	if (stat(in, &statbuf) != 0)
	{
		return 1;
	}else
	{
		if (S_ISREG(statbuf.st_mode)==0)
		{
			return 1;
		}else
		{
			return 0;
		}
	}

}

void remove_dir(struct simulation *sim,char* dir_name)
{

struct dirent *next_file;
DIR *theFolder;
char filepath[256];


//if (get_dump_status(dump_newton)==TRUE)
//{

	theFolder = opendir(dir_name);
	if (theFolder!=NULL)
	{
		while((next_file=readdir(theFolder))!=NULL)
		{
			if ((strcmp(next_file->d_name,".")!=0)&&(strcmp(next_file->d_name,"..")!=0))
			{
				join_path(2, filepath,dir_name,next_file->d_name);
				if (isdir(filepath)==0)
				{
					remove_dir(sim,filepath);
					printf_log(sim,"%s =%s\n",_("Deleting directory"),filepath);
						remove(filepath);
				}else
				{
					remove(filepath);
				}
			}
		}

		closedir (theFolder);
	}
//}


}
