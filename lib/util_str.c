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

/** @file util_str.c
	@brief Utility functions for string handeling.
*/




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

	#include <fnmatch.h>


int is_number(char a)
{
switch(a)
{
	case '1':
		return TRUE;
	case '2':
		return TRUE;
	case '3':
		return TRUE;
	case '4':
		return TRUE;
	case '5':
		return TRUE;
	case '6':
		return TRUE;
	case '7':
		return TRUE;
	case '8':
		return TRUE;
	case '9':
		return TRUE;
	case '0':
		return TRUE;
	case 'e':
		return TRUE;
	case 'E':
		return TRUE;
	case '+':
		return TRUE;
	case '-':
		return TRUE;
	case '.':
		return TRUE;
	default:
		return FALSE;
}
	
}

int get_number_in_string(double *out, char* in, int n)
{
	int i=0;
	int len=strlen(in);
	int m=0;
	int c=0;
	int number=-1;
	for (i=0;i<len;i++)
	{
		if (i>0)
		{
			m=is_number(in[i-1]);
		}else
		{
			m=FALSE;
		}

		c=is_number(in[i]);

		if ((m==FALSE) && (c==TRUE))
		{
			number++;
			if (number==n)
			{
				sscanf(&(in[i]),"%le",out);
				return 0;
			}
		}


	}

return -1;		
}

int replace_number_in_string(char *buf, char* in, double replace, int n)
{
	int i=0;
	int len=strlen(in);
	int m=0;
	int c=0;
	int number=-1;
	int pos=0;
	strcpy(buf,"");

	for (i=0;i<len;i++)
	{
		if (i>0)
		{
		m=is_number(in[i-1]);
		}else
		{
			m=FALSE;
		}

		c=is_number(in[i]);

		if ((m==FALSE) && (c==TRUE))
		{
			number++;
			if (number==n)
			{
				char temp[200];
				sprintf(temp,"%le ",replace);
				strcat(buf,temp);
				pos=strlen(buf);
			}
		}
		
		if (number!=n)
		{
			buf[pos]=in[i];
			pos++;
			buf[pos]=0;
			
		}
		
	}
		
}

int fnmatch2(char *pat,char *in)
{

	return fnmatch(pat,in, FNM_PATHNAME);

}

void string_to_hex(char* out,char* in)
{
int i;
char temp[8];
strcpy(out,"");

for (i=0;i<strlen(in);i++)
{
	sprintf(temp,"%02x",in[i]);
	strcat(out,temp);
}

}


int cmpstr_min(char * in1,char *in2)
{
int i;
int max=strlen(in1);
if (strlen(in2)<max) max=strlen(in2);
for (i=0;i<max;i++)
{
	if (in1[i]!=in2[i]) return 1;
}
return 0;
}

int strextract_name(char *out,char * in)
{
int i;
for (i=0;i<strlen(in);i++)
{
	if (in[i]=='@')
	{
		out[i]=0;
		return strlen(out);
	}
	out[i]=in[i];

}
strcpy(out,"");
return -1;
}

int strcmp_end(char * str,char *end)
{
if (strlen(str)<strlen(end)) return 1;
int pos=strlen(str)-strlen(end);
return strcmp((char *)(str+pos),end);
}

int strcmp_begin(char * str,char *begin)
{
int i;
if (strlen(str)<strlen(begin)) return 1;
int lb=strlen(begin);
for (i=0;i<lb;i++)
{
	if (str[i]!=begin[i]) return 1;
}
return 0;
}

char* strextract_domain(char * in)
{
int i=0;
for (i=0;i<strlen(in)-1;i++)
{
	if (in[i]=='@')
	{
		return (char *)(&in[i+1]);
	}
}
return (char *)-1;
}

int is_domain(char * in)
{
int i=0;
for (i=0;i<strlen(in)-1;i++)
{
	if (in[i]=='@')
	{
		return 0;
	}
}


return -1;
}

int extract_str_number(char * in,char *cut)
{
int out;
int len=strlen(cut);
sscanf((in+len),"%d",&out);
return out;
}

int strextract_int(char * in)
{
char temp[200];
int i=0;
int ret=0.0;
int count=0;
for (i=0;i<strlen(in);i++)
{
	if ((in[i]>47)&&(in[i]<58))
	{
		temp[count]=in[i];
		count++;
	}

}
temp[count]=0;
sscanf(temp,"%d",&ret);
return ret;
}
