//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
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
#include <umfpack.h>
#include <util.h>

static int last_col = 0;
static int last_nz = 0;
static double *x = NULL;
static int *Ap = NULL;
static int *Ai = NULL;
static double *Ax = NULL;
static double *b = NULL;
static double *Tx = NULL;

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

int umfpack_solver(struct simulation *sim, int col, int nz, int *Ti, int *Tj,
		   long double *lTx, long double *lb)
{
	int i;
	void *Symbolic, *Numeric;
	int status;
	double *dtemp;
	int *itemp;

	if ((last_col != col) || (last_nz != nz)) {
		dtemp = realloc(x, col * sizeof(double));
		if (dtemp == NULL) {
			ewe(sim, "realloc failed\n");
		} else {
			x = dtemp;
		}

		dtemp = realloc(b, col * sizeof(double));
		if (dtemp == NULL) {
			ewe(sim, "realloc failed\n");
		} else {
			b = dtemp;
		}

		itemp = realloc(Ap, (col + 1) * sizeof(int));
		if (itemp == NULL) {
			ewe(sim, "realloc failed\n");
		} else {
			Ap = itemp;
		}

		itemp = realloc(Ai, (nz) * sizeof(int));
		if (itemp == NULL) {
			ewe(sim, "realloc failed\n");
		} else {
			Ai = itemp;
		}

		dtemp = realloc(Ax, (nz) * sizeof(double));
		if (dtemp == NULL) {
			ewe(sim, "realloc failed\n");
		} else {
			Ax = dtemp;
		}

		dtemp = realloc(Tx, (nz) * sizeof(double));
		if (dtemp == NULL) {
			ewe(sim, "realloc failed\n");
		} else {
			Tx = dtemp;
		}

		last_col = col;
		last_nz = nz;
	}

	for (i = 0; i < col; i++) {
		b[i] = (double)lb[i];
	}

	for (i = 0; i < nz; i++) {
		Tx[i] = (double)lTx[i];
	}

	double Control[UMFPACK_CONTROL], Info[UMFPACK_INFO];

	umfpack_di_defaults(Control);
	Control[UMFPACK_BLOCK_SIZE] = 20;
//Control [UMFPACK_STRATEGY]=UMFPACK_STRATEGY_SYMMETRIC;//UMFPACK_STRATEGY_UNSYMMETRIC;
//Control [UMFPACK_ORDERING]=UMFPACK_ORDERING_BEST;//UMFPACK_ORDERING_AMD;//UMFPACK_ORDERING_BEST;//
//printf("%lf\n",Control[UMFPACK_BLOCK_SIZE]);
//Control [UMFPACK_PIVOT_TOLERANCE]=0.0001;
//Control[UMFPACK_SINGLETONS]=1;
//Control[UMFPACK_SCALE]=3;
	status =
	    umfpack_di_triplet_to_col(col, col, nz, Ti, Tj, Tx, Ap, Ai, Ax,
				      NULL);
//printf("rod1\n");
//getchar();

	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}
// symbolic analysis
//printf("here2 %d\n",col);
	status =
	    umfpack_di_symbolic(col, col, Ap, Ai, Ax, &Symbolic, Control, Info);
//printf("rod2\n");
//getchar();

//printf("here3\n");

	if (status != UMFPACK_OK) {
		error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}
// LU factorization
	umfpack_di_numeric(Ap, Ai, Ax, Symbolic, &Numeric, Control, Info);
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

	umfpack_di_solve(UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, Control, Info);

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

	for (i = 0; i < col; i++) {
		lb[i] = (long double)x[i];
	}

//memcpy(b, x, col*sizeof(double));
//umfpack_toc(stats);

	return 0;
}

void umfpack_solver_free()
{
	free(x);
	free(Ap);
	free(Ai);
	free(Ax);
	x = NULL;
	Ap = NULL;
	Ai = NULL;
	Ax = NULL;
	last_col = 0;
	last_nz = 0;
}
