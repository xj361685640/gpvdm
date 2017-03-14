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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dump.h>
#include <suitesparse/umfpack.h>
#include "complex_solver.h"
#include <math.h>
#include "buffer.h"
#include "lang.h"
#include "log.h"
#include <util.h>


void complex_error_report(int status, const char *file, const char *func, int line)
{
	fprintf(stderr, "in %s: file %s, line %d: ", func, file, line);

	switch (status) {
		case UMFPACK_ERROR_out_of_memory:
			fprintf(stderr, _("out of memory!\n"));
			break;
		case UMFPACK_WARNING_singular_matrix:
			fprintf(stderr, _("matrix is singular!\n"));
			break;
		default:
			fprintf(stderr, "UMFPACK error code %d\n", status);
	}
}


void complex_solver_dump_matrix(struct simulation *sim,int col,int nz,int *Ti,int *Tj, double *Tx, double *Txz,double *b,double *bz)
{
char build[100];
struct buffer buf;
buffer_init(&buf);
buffer_malloc(&buf);
strcpy(buf.type,"map");
buffer_add_info(&buf);
int i;
for (i=0;i<nz;i++)
{
	sprintf(build,"%d %d %le\n",Tj[i],Ti[i],sqrt(pow(Tx[i],2.0)+pow(Txz[i],2.0)));
	buffer_add_string(&buf,build);

}

for (i=0;i<col;i++)
{
	sprintf(build,"%d %d %le\n",col,i,sqrt(pow(b[i],2.0)+pow(bz[i],2.0)));
	buffer_add_string(&buf,build);
}


buffer_dump(sim,"matrix.dat",&buf);

buffer_free(&buf);
printf_log(sim,_("Matrix dumped\n"));
}

void complex_solver_free(struct simulation *sim)
{
free(sim->complex_x);
free(sim->complex_xz);
free(sim->complex_Ap);
free(sim->complex_Ai);
free(sim->complex_Ax);
free(sim->complex_Az);
sim->complex_x=NULL;
sim->complex_xz=NULL;
sim->complex_Ap=NULL;
sim->complex_Ai=NULL;
sim->complex_Ax=NULL;
sim->complex_Az=NULL;
sim->complex_last_col=0;
sim->complex_last_nz=0;
printf_log(sim,_("Complex solver free\n"));
}

void complex_solver_print(struct simulation *sim,int col,int nz,int *Ti,int *Tj, double *Tx, double *Txz,double *b,double *bz)
{
int i;
for (i=0;i<nz;i++)
{
	printf_log(sim,"%d %d %le+i%le\n",Ti[i],Tj[i],Tx[i],Txz[i]);
}

for (i=0;i<col;i++)
{
	printf_log(sim,"%le+i%le\n",b[i],bz[i]);
}


}

int complex_solver(struct simulation *sim,int col,int nz,int *Ti,int *Tj, double *Tx, double *Txz,double *b,double *bz)
{
int i;
void *Symbolic, *Numeric;
int status;
double *dtemp=NULL;
int *itemp=NULL;
if ((sim->complex_last_col!=col)||(sim->complex_last_nz!=nz))
{

	dtemp = realloc(sim->complex_x,col*sizeof(double));
	if (dtemp==NULL)
	{
		ewe(sim,_("complex_solver realloc memory error"));
	}else
	{
		sim->complex_x=dtemp;
	}

	dtemp = realloc(sim->complex_xz,col*sizeof(double));
	if (dtemp==NULL)
	{
		ewe(sim,_("complex_solver realloc memory error"));
	}else
	{
		sim->complex_xz=dtemp;
	}

	itemp = realloc(sim->complex_Ap,(col+1)*sizeof(int));
	if (itemp==NULL)
	{
		ewe(sim,_("complex_solver realloc memory error"));
	}else
	{
		sim->complex_Ap=itemp;
	}

	itemp = realloc(sim->complex_Ai,(nz)*sizeof(int));
	if (itemp==NULL)
	{
		ewe(sim,_("complex_solver realloc memory error"));
	}else
	{
		sim->complex_Ai=itemp;
	}

	dtemp  = realloc(sim->complex_Ax,(nz)*sizeof(double));
	if (dtemp==NULL)
	{
		ewe(sim,_("complex_solver realloc memory error"));
	}else
	{
		sim->complex_Ax=dtemp;
	}

	dtemp = realloc (sim->complex_Az,(nz) * sizeof (double));
	if (dtemp==NULL)
	{
		ewe(sim,_("complex_solver realloc memory error"));
	}else
	{
		sim->complex_Az=dtemp;
	}

	sim->complex_last_col=col;
	sim->complex_last_nz=nz;
}

double Info [UMFPACK_INFO], Control [UMFPACK_CONTROL];

// get the default control parameters
umfpack_zi_defaults (Control) ;

//change the default print level for this demo
//(otherwise, nothing will print)
Control [UMFPACK_PRL] = 1 ;

//print the license agreement
//umfpack_zi_report_status (Control, UMFPACK_OK) ;
Control [UMFPACK_PRL] = 0 ;

// print the control parameters
umfpack_zi_report_control (Control) ;

status = umfpack_zi_triplet_to_col (col, col, nz, Ti, Tj, Tx, Txz, sim->complex_Ap, sim->complex_Ai, sim->complex_Ax, sim->complex_Az, NULL) ;


if (status != UMFPACK_OK) {
	complex_error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

// symbolic analysis
//status = umfpack_di_symbolic(col, col, sim->complex_Ap, sim->complex_Ai, Ax, &Symbolic, NULL, NULL);
status = umfpack_zi_symbolic(col, col, sim->complex_Ap, sim->complex_Ai, sim->complex_Ax, sim->complex_Az, &Symbolic, Control, Info) ;
umfpack_zi_report_status (Control, status) ;

if (status != UMFPACK_OK) {
	complex_error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

// LU factorization
//umfpack_di_numeric(sim->complex_Ap, sim->complex_Ai, sim->complex_Ax, Symbolic, &Numeric, NULL, NULL);
umfpack_zi_numeric (sim->complex_Ap, sim->complex_Ai, sim->complex_Ax, sim->complex_Az, Symbolic, &Numeric, Control, Info) ;

if (status != UMFPACK_OK) {
	complex_error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}
// solve system

//umfpack_di_free_symbolic(&Symbolic);

        // umfpack_di_solve(UMFPACK_A, sim->complex_Ap, sim->complex_Ai, sim->complex_Ax, x, b, Numeric, NULL, NULL);
status = umfpack_zi_solve(UMFPACK_A, sim->complex_Ap, sim->complex_Ai, sim->complex_Ax, sim->complex_Az, sim->complex_x, sim->complex_xz, b, bz, Numeric, Control, Info) ;
if (status != UMFPACK_OK) {
	complex_error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

    (void) umfpack_zi_report_vector (col, sim->complex_x, sim->complex_xz, Control) ;

umfpack_zi_free_symbolic (&Symbolic) ;
umfpack_di_free_numeric(&Numeric);

for (i = 0; i < col; i++)
{
	b[i]=sim->complex_x[i];
	bz[i]=sim->complex_xz[i];
}

return 0;
}
