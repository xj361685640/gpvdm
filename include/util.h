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



#ifndef h_util
#define h_util
#include <stdio.h>
#include <sim_struct.h>

#ifdef windows
#include <windows.h>
#endif

#include <colors.h>

int strcmp_begin(char * str,char *begin);
void set_ewe_lock_file(char *lockname,char *data);
void print_hex(struct simulation *sim,unsigned char *data);
void remove_dir(struct simulation *sim,char* dir_name);
int ewe(struct simulation *sim, const char *format, ...);
double read_value(struct simulation *sim,char *file,int skip,int line);
int strcmp_end(char * str,char *end);
int extract_str_number(char * in,char *cut);
void randomprint(struct simulation *sim,char *in);
int scanarg( char* in[],int count,char * find);
int get_arg_plusone_pos( char* in[],int count,char * find);
char * get_arg_plusone( char* in[],int count,char * find);
FILE *fopena(char *path,char *name,const char *mode);

void edit_file_int(struct simulation *sim,char *in_name,char *front,int line,int value);
void edit_file(struct simulation *sim,char *in_name,char *front,int line,double value);
void edit_file_by_var(struct simulation *sim,char *in_name,char *token,char *newtext);
void copy_file(struct simulation *sim,char *output,char *input);
int get_file_len(struct simulation *sim,char *file_name);
int cmpstr_min(char * in1,char *in2);
int english_to_bin(struct simulation *sim,char * in);
void write_x_y_to_file(struct simulation *sim,char *name,double *x,double *y,int len);
void write_x_y_z_to_file(struct simulation *sim,char *name,double *x,double *y,double *z,int len);
int get_dir_name_from_path(char *out, char *in);
char *get_file_name_from_path(char *in);
void string_to_hex(char* out,char* in);
int strextract_name(char *out,char * in);
int strextract_int(char * in);
char* strextract_domain(char * in);
int find_config_file(struct simulation *sim,char *ret,char *dir_name,char* search_name,char *start_of_name);
void fx_with_units(char *out,double number);
void time_with_units(char *out,double number);
int is_domain(char * in);
int isdir(const char *path);
int path_up_level(char *out, char *in);
int fnmatch2(char *pat,char *in);
void split_dot(char *out, char *in);
int isfile(char *in);
int replace_number_in_string(char *buf, char* in, double replace, int n);
int get_number_in_string(double *out, char* in, int n);
#ifdef windows
void timersub(struct timeval *a,struct timeval *b,struct timeval *r);
#endif

#endif
