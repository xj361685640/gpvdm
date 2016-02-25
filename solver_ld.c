//    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for 1st, 2nd and 3rd generation solar cells.
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "umfpack.h"
#include "sim.h"
#include "solver_ld.h"

#define NR_END 1
#define FREE_ARG char*
#define NRANSI
#define EPS 1.0e-14

unsigned long *lvector(long nl, long nh)
{
	unsigned long *v;

	v = (unsigned long *)
	    malloc((size_t) ((nh - nl + 1 + NR_END) * sizeof(long)));
	if (!v)
		ewe("allocation failure in lvector()");
	return v - nl + NR_END;
}

long double **matrix(long nrl, long nrh, long ncl, long nch)
{
	long i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
	long double **m;

	m = (long double **)
	    malloc((size_t) ((nrow + NR_END) * sizeof(long double *)));
	if (!m)
		ewe("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;

	m[nrl] =
	    (long double *)
	    malloc((size_t) ((nrow * ncol + NR_END) * sizeof(long double)));
	if (!m[nrl])
		ewe("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;

	for (i = nrl + 1; i <= nrh; i++)
		m[i] = m[i - 1] + ncol;

	return m;
}

void to_indexstored(int *Ti, int *Tj, long double *Tx, long double *sa,
		    unsigned long *ija, int n, int nmax)
{
	int i, j;
	int pos = 0;
	unsigned long k;
	for (pos = 0; pos < nmax; pos++) {
		if (Ti[pos] == Tj[pos]) {
			j = Tj[pos];
			sa[j] = Tx[pos];
		}
	}
	ija[0] = n + 2;
	k = n;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			for (pos = 0; pos < nmax; pos++) {
				if ((Ti[pos] == i) && (Tj[pos] == j)) {
					if (fabs(Tx[pos]) >= 0.0) {
						if (i != j) {
							if (++k > nmax)
								ewe("sprsin: nmax too small");
							sa[k] = Tx[pos];
							ija[k] = j + 1;
						}
					}
					break;
				}
			}
		}
		ija[i + 1] = k + 1 + 1;
	}

}

void sprsin(long double **a, int n, long double thresh, unsigned long nmax,
	    long double *sa, unsigned long *ija)
{
	int i, j;
	unsigned long k;
	for (j = 1; j <= n; j++) {
		sa[j] = a[j][j];
	}

	ija[1] = n + 2;
	k = n + 1;
	for (i = 1; i <= n; i++) {
		for (j = 1; j <= n; j++) {
			if (fabs(a[i][j]) >= thresh) {
				if (i != j) {
					if (++k > nmax)
						ewe("sprsin: nmax too small");
					sa[k] = a[i][j];
					ija[k] = j;
				}
			}
		}
		ija[i + 1] = k + 1;
	}
}

long double *dvector(long nl, long nh)
{
	long double *v;

	v = (long double *)
	    malloc((size_t) ((nh - nl + 1 + NR_END) * sizeof(long double)));
	if (!v)
		ewe("allocation failure in dvector()");
	return v - nl + NR_END;
}

void free_dvector(long double *v, long nl, long nh)
{
	free((FREE_ARG) (v + nl - NR_END));
}

long double snrm(unsigned long n, long double sx[], int itol)
{
	unsigned long i, isamax;
	long double ans;

	if (itol <= 3) {
		ans = 0.0;
		for (i = 1; i <= n; i++)
			ans += sx[i] * sx[i];
		return sqrt(ans);
	} else {
		isamax = 1;
		for (i = 1; i <= n; i++) {
			if (fabs(sx[i]) > fabs(sx[isamax]))
				isamax = i;
		}
		return fabs(sx[isamax]);
	}
}

void dsprstx(long double sa[], unsigned long ija[], long double x[],
	     long double b[], unsigned long n)
{
	unsigned long i, j, k;
	if (ija[1] != n + 2)
		ewe("mismatched vector and matrix in dsprstx");
	for (i = 1; i <= n; i++)
		b[i] = sa[i] * x[i];
	for (i = 1; i <= n; i++) {
		for (k = ija[i]; k <= ija[i + 1] - 1; k++) {
			j = ija[k];
			b[j] += sa[k] * x[i];
		}
	}
}

void dsprsax(long double sa[], unsigned long ija[], long double x[],
	     long double b[], unsigned long n)
{
	unsigned long i, k;

	if (ija[1] != n + 2)
		ewe("dsprsax: mismatched vector and matrix");
	for (i = 1; i <= n; i++) {
		b[i] = sa[i] * x[i];
		for (k = ija[i]; k <= ija[i + 1] - 1; k++)
			b[i] += sa[k] * x[ija[k]];
	}
}

void asolve(unsigned long n, long double b[], long double x[], int itrnsp,
	    long double *sa)
{
	unsigned long i;

	for (i = 1; i <= n; i++)
		x[i] = (sa[i] != 0.0 ? b[i] / sa[i] : b[i]);
}

void atimes(unsigned long n, long double x[], long double r[], int itrnsp,
	    long double *sa, unsigned long *ija)
{
	if (itrnsp)
		dsprstx(sa, ija, x, r, n);
	else
		dsprsax(sa, ija, x, r, n);
}

void linbcg(unsigned long n, long double b[], long double x[], int itol,
	    long double tol, int itmax, int *iter, long double *err,
	    long double *sa, unsigned long *ija)
{
	unsigned long j;
	long double ak, akden, bk, bkden = 1.0;
	long double bknum, bnrm = 0.0;
	long double dxnrm, xnrm, zm1nrm, znrm;
	long double *p, *pp, *r, *rr, *z, *zz;

	p = dvector(1, n);
	pp = dvector(1, n);
	r = dvector(1, n);
	rr = dvector(1, n);
	z = dvector(1, n);
	zz = dvector(1, n);

	*iter = 0;
	atimes(n, x, r, 0, sa, ija);
	for (j = 1; j <= n; j++) {
		r[j] = b[j] - r[j];
		rr[j] = r[j];
	}
	znrm = 1.0;
	if (itol == 1)
		bnrm = snrm(n, b, itol);
	else if (itol == 2) {
		asolve(n, b, z, 0, sa);
		bnrm = snrm(n, z, itol);
	} else if (itol == 3 || itol == 4) {
		asolve(n, b, z, 0, sa);
		bnrm = snrm(n, z, itol);
		asolve(n, r, z, 0, sa);
		znrm = snrm(n, z, itol);
	} else
		ewe("illegal itol in linbcg");
	asolve(n, r, z, 0, sa);
	while (*iter <= itmax) {
		++(*iter);
		zm1nrm = znrm;
		asolve(n, rr, zz, 1, sa);
		for (bknum = 0.0, j = 1; j <= n; j++)
			bknum += z[j] * rr[j];
		if (*iter == 1) {
			for (j = 1; j <= n; j++) {
				p[j] = z[j];
				pp[j] = zz[j];
			}
		} else {
			bk = bknum / bkden;
			for (j = 1; j <= n; j++) {
				p[j] = bk * p[j] + z[j];
				pp[j] = bk * pp[j] + zz[j];
			}
		}
		bkden = bknum;
		atimes(n, p, z, 0, sa, ija);
		for (akden = 0.0, j = 1; j <= n; j++)
			akden += z[j] * pp[j];
		ak = bknum / akden;
		atimes(n, pp, zz, 1, sa, ija);
		for (j = 1; j <= n; j++) {
			x[j] += ak * p[j];
			r[j] -= ak * z[j];
			rr[j] -= ak * zz[j];
		}
		asolve(n, r, z, 0, sa);
		if (itol == 1 || itol == 2) {
			znrm = 1.0;
			*err = snrm(n, r, itol) / bnrm;
		} else if (itol == 3 || itol == 4) {
			znrm = snrm(n, z, itol);
			if (fabs(zm1nrm - znrm) > EPS * znrm) {
				dxnrm = fabs(ak) * snrm(n, p, itol);
				*err = znrm / fabs(zm1nrm - znrm) * dxnrm;
			} else {
				*err = znrm / bnrm;
				continue;
			}
			xnrm = snrm(n, x, itol);
			if (*err <= 0.5 * xnrm)
				*err /= xnrm;
			else {
				*err = znrm / bnrm;
				continue;
			}
		}
		if (*err <= tol)
			break;
	}
	printf("iter=%4d err=%Le\n", *iter, *err);

	free_dvector(p, 1, n);
	free_dvector(pp, 1, n);
	free_dvector(r, 1, n);
	free_dvector(rr, 1, n);
	free_dvector(z, 1, n);
	free_dvector(zz, 1, n);
}

#undef EPS

int nr_solver(int col, int nz, int *Ti, int *Tj, long double *Tx,
	      long double *b)
{
	int i;
	unsigned long *ija;
	long double *sa;
	long double *x;
	long double err;
	int iter;

	sa = malloc(sizeof(long double) * (nz + 1));
	ija = malloc(sizeof(unsigned long) * (nz + 1));

	x = malloc(sizeof(long double) * col);

	to_indexstored(Ti, Tj, Tx, sa, ija, col, nz);
	linbcg(col, (b - 1), (x - 1), 1, 1e-100, 1000, &iter, &err, (sa - 1),
	       (ija - 1));

	for (i = 0; i < col; i++) {
		b[i] = x[i];
	}
	free(sa);
	free(ija);
	free(x);
	return 0;
}

////////////////////////////
static int last_col = 0;
static int last_nz = 0;
static long double *x = NULL;
static int *Ap = NULL;
static int *Ai = NULL;
static long double *Ax = NULL;
static int solver_ld_dump_every_matrix = 0;

static double total_time = 0.0;

void solver_ld_test()
{
	int nz = 4l;
	int col = 2;
	long double Tx[4] = { 5.0, -4.0, 1.0, 2.0 };
	int Ti[4] = { 0, 1, 0, 1 };
	int Tj[4] = { 0, 0, 1, 1 };
	long double b[2] = { 8.0, 6.0 };
	solver_ld(col, nz, Ti, Tj, Tx, b);
	printf("%Lf %Lf", b[0], b[1]);
}

void set_solver_ld_dump_every_matrix(int dump)
{
	solver_ld_dump_every_matrix = dump;
}

void error_ld_report(int status, const char *file, const char *func, int line)
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

void solver_ld_precon(int col, int nz, int *Ti, int *Tj, long double *Tx,
		      long double *b)
{
	int i;
	int ii;

	for (i = 0; i < nz; i++) {
		if (Ti[i] == Tj[i]) {
			if (Tx[i] != 0.0) {
				b[Ti[i]] /= Tx[Ti[i]];
				for (ii = 0; ii < nz; ii++) {
					if (Ti[ii] == Ti[i]) {
						Tx[ii] /= Tx[Ti[i]];
						//printf("%e % ");
					}
				}
				//getchar();
			}
		}
	}

}

void solver_ld_dump_matrix(int col, int nz, int *Ti, int *Tj, long double *Tx,
			   long double *b, char *index)
{
	FILE *matrix;
	char name[100];
	sprintf(name, "matrix%s.dat", index);
	matrix = fopen(name, "w");
//fprintf(matrix,"%d\n",nz);
//fprintf(matrix,"%d\n",col);

	int i;
	for (i = 0; i < nz; i++) {
		fprintf(matrix, "%d %d %le\n", Tj[i], Ti[i], fabs(Tx[i]));
	}

	for (i = 0; i < col; i++) {
		fprintf(matrix, "%d %d %le\n", 0, i, fabs(b[i]));
	}

	printf("Matrix dumped\n");

	fclose(matrix);
}

void solver_ld_free()
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

int solver_ld(int col, int nz, int *Ti, int *Tj, long double *Tx,
	      long double *b)
{
	if (solver_ld_dump_every_matrix > 0) {
		char name[100];
		sprintf(name, "%d", solver_ld_dump_every_matrix);
		printf("Dumping matrix %d\n", solver_ld_dump_every_matrix);
		solver_ld_dump_matrix(col, nz, Ti, Tj, Tx, b, name);
		solver_ld_dump_every_matrix++;
	}
//getchar();
//int i;
	double stats[2];
	umfpack_tic(stats);
//void *Symbolic, *Numeric;
//int status;
//printf("here1\n");
	if ((last_col != col) || (last_nz != nz)) {
		x = realloc(x, col * sizeof(long double));
		Ap = realloc(Ap, (col + 1) * sizeof(int));
		Ai = realloc(Ai, (nz) * sizeof(int));
		Ax = realloc(Ax, (nz) * sizeof(long double));
		last_col = col;
		last_nz = nz;
	}
	nr_solver(col, nz, Ti, Tj, Tx, b);
//getchar();
/*
double Control [UMFPACK_CONTROL];
long double Info [UMFPACK_INFO];

umfpack_dild_defaults (Control) ;
//printf("rod\n");
//getchar();
Control[UMFPACK_BLOCK_SIZE]=20;
//Control [UMFPACK_STRATEGY]=UMFPACK_STRATEGY_SYMMETRIC;//UMFPACK_STRATEGY_UNSYMMETRIC;
//Control [UMFPACK_ORDERING]=UMFPACK_ORDERING_BEST;//UMFPACK_ORDERING_AMD;//UMFPACK_ORDERING_BEST;//
//printf("%lf\n",Control[UMFPACK_BLOCK_SIZE]);
//Control [UMFPACK_PIVOT_TOLERANCE]=0.0001;
//Control[UMFPACK_SINGLETONS]=1;
//Control[UMFPACK_SCALE]=3;
status = umfpack_dild_triplet_to_col(col, col, nz, Ti, Tj, Tx, Ap, Ai, Ax, NULL);
//printf("rod1\n");
//getchar();

if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

// symbolic analysis
//printf("here2 %d\n",col);
status = umfpack_dild_symbolic(col, col, Ap, Ai, Ax, &Symbolic, Control, Info);
//printf("rod2\n");
//getchar();

//printf("here3\n");

if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

// LU factorization
umfpack_dild_numeric(Ap, Ai, Ax, Symbolic, &Numeric, Control, Info);
//printf("rod3\n");
//getchar();

if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}
// solve system

umfpack_dild_free_symbolic(&Symbolic);
//printf("rod4\n");
//getchar();

umfpack_dild_solve(UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, Control, Info);
//printf("rod5\n");
//getchar();

//printf("%lf\n",Info [UMFPACK_ORDERING_USED]);

if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

umfpack_dild_free_numeric(&Numeric);
//printf("rod6\n");
//getchar();

memcpy(b, x, col*sizeof(long double));
*/
	umfpack_toc(stats);
	total_time += stats[0];
	return 0;
}

void solver_ld_print_time()
{
	printf("Time in umfpack %lf\n", total_time);
}
