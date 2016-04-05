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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dump.h>
#include "umfpack.h"
#include "sim.h"
#include "complex_solver.h"
#include <math.h>
#include "buffer.h"
#include "lang.h"
#include "log.h"

static int last_col = 0;
static int last_nz = 0;
static double *x = NULL;
static double *xz = NULL;
static int *Ap = NULL;
static int *Ai = NULL;
static double *Ax = NULL;
static double *Az = NULL;

void complex_error_report(int status, const char *file, const char *func,
			  int line)
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

void complex_solver_dump_matrix(int col, int nz, int *Ti, int *Tj, double *Tx,
				double *Txz, double *b, double *bz)
{
	char build[100];
	struct buffer buf;
	buffer_init(&buf);
	buffer_malloc(&buf);
	strcpy(buf.type, "map");
	buffer_add_info(&buf);
	int i;
	for (i = 0; i < nz; i++) {
		sprintf(build, "%d %d %le\n", Tj[i], Ti[i],
			sqrt(pow(Tx[i], 2.0) + pow(Txz[i], 2.0)));
		buffer_add_string(&buf, build);

	}

	for (i = 0; i < col; i++) {
		sprintf(build, "%d %d %le\n", col, i,
			sqrt(pow(b[i], 2.0) + pow(bz[i], 2.0)));
		buffer_add_string(&buf, build);
	}

	buffer_dump("matrix.dat", &buf);

	buffer_free(&buf);
	printf(_("Matrix dumped\n"));
}

void complex_solver_free()
{
	free(x);
	free(xz);
	free(Ap);
	free(Ai);
	free(Ax);
	free(Az);
	x = NULL;
	xz = NULL;
	Ap = NULL;
	Ai = NULL;
	Ax = NULL;
	Az = NULL;
	last_col = 0;
	last_nz = 0;
	printf_log(_("Complex solver free\n"));
}

void complex_solver_print(int col, int nz, int *Ti, int *Tj, double *Tx,
			  double *Txz, double *b, double *bz)
{
	int i;
	for (i = 0; i < nz; i++) {
		printf("%d %d %le+i%le\n", Ti[i], Tj[i], Tx[i], Txz[i]);
	}

	for (i = 0; i < col; i++) {
		printf("%le+i%le\n", b[i], bz[i]);
	}

}

int complex_solver(int col, int nz, int *Ti, int *Tj, double *Tx, double *Txz,
		   double *b, double *bz)
{

//getchar();
	int i;
	void *Symbolic, *Numeric;
	int status;
//printf("here1\n");
	double *dtemp = NULL;
	int *itemp = NULL;
	if ((last_col != col) || (last_nz != nz)) {

		dtemp = realloc(x, col * sizeof(double));
		if (x == NULL) {
			ewe(_("realloc memory error"));
		} else {
			x = dtemp;
		}

		dtemp = realloc(xz, col * sizeof(double));
		if (dtemp == NULL) {
			ewe(_("realloc memory error"));
		} else {
			xz = dtemp;
		}

		itemp = realloc(Ap, (col + 1) * sizeof(int));
		if (itemp == NULL) {
			ewe(_("realloc memory error"));
		} else {
			Ap = itemp;
		}

		itemp = realloc(Ai, (nz) * sizeof(int));
		if (itemp == NULL) {
			ewe(_("realloc memory error"));
		} else {
			Ai = itemp;
		}

		dtemp = realloc(Ax, (nz) * sizeof(double));
		if (dtemp == NULL) {
			ewe(_("realloc memory error"));
		} else {
			Ax = dtemp;
		}

		dtemp = realloc(Az, (nz) * sizeof(double));
		if (x == NULL) {
			ewe(_("realloc memory error"));
		} else {
			Az = dtemp;
		}

		last_col = col;
		last_nz = nz;
	}

	double Info[UMFPACK_INFO], Control[UMFPACK_CONTROL];

// get the default control parameters
	umfpack_zi_defaults(Control);

//change the default print level for this demo 
//(otherwise, nothing will print) 
	Control[UMFPACK_PRL] = 1;

//print the license agreement 
//umfpack_zi_report_status (Control, UMFPACK_OK) ;
	Control[UMFPACK_PRL] = 0;

// print the control parameters 
	umfpack_zi_report_control(Control);

	status =
	    umfpack_zi_triplet_to_col(col, col, nz, Ti, Tj, Tx, Txz, Ap, Ai, Ax,
				      Az, NULL);

	if (status != UMFPACK_OK) {
		complex_error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}
// symbolic analysis
//printf("here2 %d\n",col);
//status = umfpack_di_symbolic(col, col, Ap, Ai, Ax, &Symbolic, NULL, NULL);
	status =
	    umfpack_zi_symbolic(col, col, Ap, Ai, Ax, Az, &Symbolic, Control,
				Info);
	umfpack_zi_report_status(Control, status);
//printf("here3\n");

	if (status != UMFPACK_OK) {
		complex_error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}
// LU factorization 
//umfpack_di_numeric(Ap, Ai, Ax, Symbolic, &Numeric, NULL, NULL);
	umfpack_zi_numeric(Ap, Ai, Ax, Az, Symbolic, &Numeric, Control, Info);

	if (status != UMFPACK_OK) {
		complex_error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}
// solve system

//umfpack_di_free_symbolic(&Symbolic);

	// umfpack_di_solve(UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, NULL, NULL);
	status =
	    umfpack_zi_solve(UMFPACK_A, Ap, Ai, Ax, Az, x, xz, b, bz, Numeric,
			     Control, Info);
	if (status != UMFPACK_OK) {
		complex_error_report(status, __FILE__, __func__, __LINE__);
		return EXIT_FAILURE;
	}
//printf ("\nx (solution of Ax=b): ") ;
	(void)umfpack_zi_report_vector(col, x, xz, Control);

	umfpack_zi_free_symbolic(&Symbolic);
	umfpack_di_free_numeric(&Numeric);

	for (i = 0; i < col; i++) {
		b[i] = x[i];
		bz[i] = xz[i];
	}

	return 0;
}
