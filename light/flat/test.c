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
#include <dlfcn.h>
#include <i.h>
#include "light.h"

int main()
{
struct light two;
light_load_config(&two);
light_free_memory(&two);
return 0;
/*int ext=0;
struct fitvars config;
void *lib_handle;
double (*fn)(struct fitvars *,int,char *);
char *error;

char libname[100];

sprintf(libname,"./error_plugin.so");

   lib_handle = dlopen(libname, RTLD_LAZY);
   if (!lib_handle) 
   {
      fprintf(stderr, "%s\n", dlerror());
   }

   fn = dlsym(lib_handle, "lib_error");
   if ((error = dlerror()) != NULL)  
   {
      fprintf(stderr, "%s\n", error);
   }

   double ret=(*fn)(&config,ext,"../../");
printf ("%le\n",ret);
   dlclose(lib_handle);
return 0;*/

}
