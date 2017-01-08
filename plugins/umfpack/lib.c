//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
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
#include <stdlib.h>
#include <suitesparse/umfpack.h>
#include <util.h>

void error_report(int status, const char *file, const char *func, int line)
{
	fprintf(stderr, "in %s: file %s, line %d: ", func, file, line);

	switch (status) {
		case UMFPACK_ERROR_out_of_memory:
			fprintf(stderr, "out of memory!\n");
			break;
		case UMFPACK_WARNING_singular_matrix:
			fprintf(stderr, "matrix is singular!\n");
			break;
		default:
			fprintf(stderr, "UMFPACK error code %d\n", status);
	}
}



int umfpack_solver(struct simulation *sim,int col,int nz,int *Ti,int *Tj, long double *lTx,long double *lb)
{
int i;
void *Symbolic, *Numeric;
int status;
double *dtemp;
int *itemp;


if ((sim->last_col!=col)||(sim->last_nz!=nz))
{
	dtemp = realloc(sim->x,col*sizeof(double));
	if (dtemp==NULL)
	{
		ewe(sim,"realloc failed\n");
	}else
	{
		sim->x=dtemp;
	}


	dtemp = realloc(sim->b,col*sizeof(double));
	if (dtemp==NULL)
	{
		ewe(sim,"realloc failed\n");
	}else
	{
		sim->b=dtemp;
	}

	itemp = realloc(sim->Ap,(col+1)*sizeof(int));
	if (itemp==NULL)
	{
		ewe(sim,"realloc failed\n");
	}else
	{
		sim->Ap=itemp;
	}

	itemp = realloc(sim->Ai,(nz)*sizeof(int));
	if (itemp==NULL)
	{
		ewe(sim,"realloc failed\n");
	}else
	{
		sim->Ai=itemp;
	}

	dtemp  = realloc(sim->Ax,(nz)*sizeof(double));
	if (dtemp==NULL)
	{
		ewe(sim,"realloc failed\n");
	}else
	{
		sim->Ax=dtemp;
	}

	dtemp  = realloc(sim->Tx,(nz)*sizeof(double));
	if (dtemp==NULL)
	{
		ewe(sim,"realloc failed\n");
	}else
	{
		sim->Tx=dtemp;
	}


	sim->last_col=col;
	sim->last_nz=nz;
}

for (i=0;i<col;i++)
{
	sim->b[i]=(double)lb[i];
}

for (i=0;i<nz;i++)
{
	sim->Tx[i]=(double)lTx[i];
}


double Control [UMFPACK_CONTROL],Info [UMFPACK_INFO];

umfpack_di_defaults (Control) ;
Control[UMFPACK_BLOCK_SIZE]=20;
//Control [UMFPACK_STRATEGY]=UMFPACK_STRATEGY_SYMMETRIC;//UMFPACK_STRATEGY_UNSYMMETRIC;
//Control [UMFPACK_ORDERING]=UMFPACK_ORDERING_BEST;//UMFPACK_ORDERING_AMD;//UMFPACK_ORDERING_BEST;//
//printf("%lf\n",Control[UMFPACK_BLOCK_SIZE]);
//Control [UMFPACK_PIVOT_TOLERANCE]=0.0001;
//Control[UMFPACK_SINGLETONS]=1;
//Control[UMFPACK_SCALE]=3;
status = umfpack_di_triplet_to_col(col, col, nz, Ti, Tj, sim->Tx, sim->Ap, sim->Ai, sim->Ax, NULL);
//printf("rod1\n");
//getchar();

if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

// symbolic analysis
//printf("here2 %d\n",col);
status = umfpack_di_symbolic(col, col, sim->Ap, sim->Ai, sim->Ax, &Symbolic, Control, Info);
//printf("rod2\n");
//getchar();

//printf("here3\n");

if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

// LU factorization
umfpack_di_numeric(sim->Ap, sim->Ai, sim->Ax, Symbolic, &Numeric, Control, Info);
//printf("rod5\n");
//getchar();


if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}
// solve system

umfpack_di_free_symbolic(&Symbolic);
//printf("rod a\n");
//getchar();


umfpack_di_solve(UMFPACK_A, sim->Ap, sim->Ai, sim->Ax, sim->x, sim->b, Numeric, Control, Info);

//printf("rod b\n");
//getchar();

//printf("%lf\n",Info [UMFPACK_ORDERING_USED]);

if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

umfpack_di_free_numeric(&Numeric);
//printf("rod\n");
//getchar();

for (i=0;i<col;i++)
{
lb[i]=(long double)sim->x[i];
}

//memcpy(b, x, col*sizeof(double));
//umfpack_toc(stats);


return 0;
}

void umfpack_solver_free(struct simulation *sim)
{
printf("%x %x %x\n",sim->x,sim->b,sim->Ap);

if (sim->x!=NULL)
{
	free(sim->x);
	sim->x=NULL;	
}
printf("%x %x %x\n",sim->x,sim->b,sim->Ap);

if (sim->b!=NULL)
{
	free(sim->b);
	sim->b=NULL;	
}
printf("%x %x %x\n",sim->x,sim->b,sim->Ap);

if (sim->Ap!=NULL)
{
	free(sim->Ap);
	sim->Ap=NULL;
}
printf("%x %x %x\n",sim->x,sim->b,sim->Ap);

if (sim->Ai!=NULL)
{
	free(sim->Ai);
	sim->Ai=NULL;
}

if (sim->Ax!=NULL)
{
	free(sim->Ax);
	sim->Ax=NULL;
}

if (sim->Tx!=NULL)
{
	free(sim->Tx);
	sim->Tx=NULL;
}

sim->last_col=0;
sim->last_nz=0;
}

