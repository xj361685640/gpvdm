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


#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "checksum.h"
#include <const.h>
#include "inp.h"
#include "util.h"
#include "cal_path.h"

static int unused __attribute__((unused));

void checksum_write(struct simulation *sim,char *file_name)
{

FILE *file;
char *buffer;
unsigned long len;
long l;
char chkfile[100];

sprintf(chkfile,"%s.chk",get_file_name_from_path(file_name));

inp_read_buffer(sim,&buffer, &l,file_name);
len=(unsigned int)l;

char temp[100];

checksum(temp,buffer, len);

free(buffer);

file=fopen(chkfile,"w");
if (file==NULL)
{
	ewe(sim,"File %s not found\n",chkfile);
}
fprintf(file,"%s\n",temp);
fclose(file);
}

int checksum_check(struct simulation *sim,char *file_name)
{

FILE *file;
char *buffer;
unsigned long len;
char chkfile[100];
char newcheck[100];
char fromfile[100];

strcpy(newcheck,"hello");

sprintf(chkfile,"%s.chk",file_name);
long l;
inp_read_buffer(sim,&buffer, &l,file_name);

len=(unsigned int)l;
checksum(newcheck,buffer, len);
free(buffer);

file=fopen(chkfile,"r");

if (!file)
{
	return FALSE;
}

unused=fscanf(file,"%s\n",fromfile);
fclose(file);

if (strcmp(newcheck,fromfile)==0)
{
	return TRUE;
}else
{
	return FALSE;
}

return 0;
}

uint32_t leftrotate (uint32_t x, uint32_t c)
{
    return ((x << c) | (x >> (32 -c)));
}

uint32_t s[64]={ 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22 ,
			     5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20 ,
			     4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23 ,
			     6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 };

uint32_t K[64]={	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
 					0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
					0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
					0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
					0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
					0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
					0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
					0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
					0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
					0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
					0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
					0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
					0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
					0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
					0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
					0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };



void checksum(char *out,char *data,int len)
{
uint32_t a0=0x67452301;
uint32_t b0=0xefcdab89;
uint32_t c0=0x98badcfe;
uint32_t d0=0x10325476;

int len_bits=len*8+8;
int add_bits=512-(len_bits % 512);
int new_len_bits=len_bits+add_bits;
int new_len_bytes=new_len_bits/8;
uint32_t* new_data=malloc(new_len_bytes*sizeof(char));
memset(new_data, 0, new_len_bytes);
memcpy(new_data, data, len);
((char*)new_data)[len]=0x80;
int i;
int ii;
int chunks=new_len_bits/512;
uint32_t dTemp=0;
uint32_t F;
int g;

uint32_t A = a0;
uint32_t B = b0;
uint32_t C = c0;
uint32_t D = d0;
for (i=0;i<chunks;i++)
{
    for (ii=0;ii<64;ii++)
	{
        if (ii<16)
		{
            F = (B & C) | ((~B) & D);
            g = ii;
		}else
		if (ii<32)
		{
            F = (D & B) | (( ~D) & C);
            g = (5*ii + 1) % 16;
		}else
		if (ii<48)
		{
            F = B ^ C ^ D;
            g = (3*ii + 5) % 16;
        }else
		if (ii<64)
		{
            F = C ^ (B | (~ D));
            g = (7*ii) % 16;
		}
        dTemp = D;
        D = C;
        C = B;
        B = B + leftrotate((A + F + K[ii] + new_data[g+i*16]), s[ii]);
        A = dTemp;
	}

    a0 = a0 + A;
    b0 = b0 + B;
    c0 = c0 + C;
    d0 = d0 + D;

}

sprintf(out,"%.8x%.8x%.8x%.8x",a0,b0,c0,d0);
free(new_data);
}


