//<clean=none></clean>
#include <stdio.h>
#include <stdlib.h>
#include "umfpack.h"
#include <math.h>

static int last_col=0;
static int last_nz=0;
static double *x=NULL;
static int *Ap=NULL;
static int *Ai=NULL;
static double *Ax=NULL;

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

void solver_precon(int col,int nz,int *Ti,int *Tj, double *Tx,double *b)
{
int i;
int ii;

for (i=0;i<nz;i++)
{
	if (Ti[i]==Tj[i])
	{
		if (Tx[i]!=0.0)
		{
			b[Ti[i]]/=Tx[Ti[i]];
			for (ii=0;ii<nz;ii++)
			{
				if (Ti[ii]==Ti[i])
				{
					Tx[ii]/=Tx[Ti[i]];
					//printf("%e % ");
				}
			}
			//getchar();
		}
	}
}

}


void solver_dump_matrix(int col,int nz,int *Ti,int *Tj, double *Tx,double *b)
{
FILE *matrix;
matrix=fopen("./matrix.dat","w");
//fprintf(matrix,"%d\n",nz);
//fprintf(matrix,"%d\n",col);


int i;
for (i=0;i<nz;i++)
{
	fprintf(matrix,"%d %d %le\n",Tj[i],Ti[i],fabs(Tx[i]));
}

for (i=0;i<col;i++)
{
	fprintf(matrix,"%d %d %le\n",0,i,fabs(b[i]));
}

printf("Matrix dumped\n");

fclose(matrix);
}

void solver_free()
{
free(x);
free(Ap);
free(Ai);
free(Ax);
x=NULL;
Ap=NULL;
Ai=NULL;
Ax=NULL;
last_col=0;
last_nz=0;
}


int solver(int col,int nz,int *Ti,int *Tj, double *Tx,double *b)
{

//getchar();
int i;
void *Symbolic, *Numeric;
int status;
//printf("here1\n");
if ((last_col!=col)||(last_nz!=nz))
{
x = realloc(x,col*sizeof(double));
Ap = realloc(Ap,(col+1)*sizeof(int));
Ai = realloc(Ai,(nz)*sizeof(int));
Ax  = realloc(Ax,(nz)*sizeof(double));
last_col=col;
last_nz=nz;
}




status = umfpack_di_triplet_to_col(col, col, nz, Ti, Tj, Tx, Ap, Ai, Ax, NULL);


if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

/* symbolic analysis */
//printf("here2 %d\n",col);
status = umfpack_di_symbolic(col, col, Ap, Ai, Ax, &Symbolic, NULL, NULL);
//printf("here3\n");

if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

/* LU factorization */
umfpack_di_numeric(Ap, Ai, Ax, Symbolic, &Numeric, NULL, NULL);


if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}
/* solve system */

umfpack_di_free_symbolic(&Symbolic);


umfpack_di_solve(UMFPACK_A, Ap, Ai, Ax, x, b, Numeric, NULL, NULL);

if (status != UMFPACK_OK) {
	error_report(status, __FILE__, __func__, __LINE__);
	return EXIT_FAILURE;
}

umfpack_di_free_numeric(&Numeric);

for (i = 0; i < col; i++)
{
	b[i]=x[i];
	//printf("x[%d] = %g\n", i, x[i]);
}

return 0;
}
