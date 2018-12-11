#include <math.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <mpi.h>

#include "vec.h"
   
#define max_rows 100000
#define send_data_tag 2001
#define return_data_tag 2002

double epsilon0=8.85418782e-12;
double mu0=1.25663706e-6;
double pi=3.141592653;
double lambda=1e-10;
double *far_avg=NULL;
double far_steps=0;

int array[max_rows];
int array2[max_rows];
   
   main(int argc, char **argv) 
   {
      long int sum, partial_sum;
      MPI_Status status;
      int my_id, root_process, ierr, i, num_rows, num_procs,
         an_id, num_rows_to_receive, avg_rows_per_process, 
         sender, num_rows_received, start_row, end_row, num_rows_to_send;

      /* Now replicte this process to create parallel processes.
       * From this point on, every process executes a seperate copy
       * of this program */

      ierr = MPI_Init(&argc, &argv);
      
      root_process = 0;
      
      /* find out MY process ID, and how many processes were started. */
      
      ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
      ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

if(my_id == root_process)
{

         /* I must be the root process, so I will query the user
          * to determine how many numbers to sum. */
printf("Hello\n");
         printf("please enter the number of numbers to sum: ");
         scanf("%i", &num_rows);
      
         if(num_rows > max_rows) {
            printf("Too many numbers.\n");
            exit(1);
         }

         avg_rows_per_process = num_rows / num_procs;

         /* initialize an array */

         for(i = 0; i < num_rows; i++) {
            array[i] = i + 1;
         }

         /* distribute a portion of the bector to each child process */
   
         for(an_id = 1; an_id < num_procs; an_id++) {
            start_row = an_id*avg_rows_per_process + 1;
            end_row   = (an_id + 1)*avg_rows_per_process;

            if((num_rows - end_row) < avg_rows_per_process)
               end_row = num_rows - 1;

            num_rows_to_send = end_row - start_row + 1;

            ierr = MPI_Send( &num_rows_to_send, 1 , MPI_INT,
                  an_id, send_data_tag, MPI_COMM_WORLD);

            ierr = MPI_Send( &array[start_row], num_rows_to_send, MPI_INT,
                  an_id, send_data_tag, MPI_COMM_WORLD);
         }

         /* and calculate the sum of the values in the segment assigned
          * to the root process */
        
         sum = 0;
         for(i = 0; i < avg_rows_per_process + 1; i++) {
            sum += array[i];   
         } 

         printf("sum %i calculated by root process\n", sum);

         /* and, finally, I collet the partial sums from the slave processes, 
          * print them, and add them to the grand sum, and print it */

         for(an_id = 1; an_id < num_procs; an_id++) {
            
            ierr = MPI_Recv( &partial_sum, 1, MPI_LONG, MPI_ANY_SOURCE,
                  return_data_tag, MPI_COMM_WORLD, &status);
  
            sender = status.MPI_SOURCE;

            printf("Partial sum %i returned from process %i\n", partial_sum, sender);
     
            sum += partial_sum;
         }

         printf("The grand total is: %i\n", sum);
}

      else {

         /* I must be a slave process, so I must receive my array segment,
          * storing it in a "local" array, array1. */

         ierr = MPI_Recv( &num_rows_to_receive, 1, MPI_INT, 
               root_process, send_data_tag, MPI_COMM_WORLD, &status);
          
         ierr = MPI_Recv( &array2, num_rows_to_receive, MPI_INT, 
               root_process, send_data_tag, MPI_COMM_WORLD, &status);

         num_rows_received = num_rows_to_receive;

         /* Calculate the sum of my portion of the array */

         partial_sum = 0;
         for(i = 0; i < num_rows_received; i++) {
            partial_sum += array2[i];
         }

         /* and finally, send my partial sum to hte root process */

         ierr = MPI_Send( &partial_sum, 1, MPI_LONG, root_process, 
               return_data_tag, MPI_COMM_WORLD);
      }
      ierr = MPI_Finalize();
   }




void near_to_far(double *x, double *E,int len)
{
int i;
double dx=x[1]-x[0];
double mid=x[len/2];
struct vec start;
struct vec stop;
double left=-0.4;
double right=0.4;
double dist=0.3;
double pos=left;
double *far=malloc(sizeof(double)*len);
double *xfar=malloc(sizeof(double)*len);
double dfar=(right-left)/((double)len);
int j;

if (far_avg==NULL)
{
far_avg=malloc(sizeof(double)*len);
	for (j=0;j<len;j++)
	{
		far_avg[j]=0.0;
	}
}

pos=left;
double k=(2.0*pi/lambda);
double modr;
double complex cresult;
double result;
int n;

FILE *out=fopen("./near.dat","w");
for (j=0;j<len;j++)
{
	fprintf(out,"%le %le\n",x[j], fabs(E[j]));
}
fclose(out);

struct vec dr;
for (j=0;j<len;j++)
{
result=0.0;
	for (n=0;n<1;n++)
	{
		for (i=0;i<len;i++)
		{
			set_vec(&start,0.0,0.0,x[i]-mid-((double)n)*x[len-1]);
			set_vec(&stop,0.0,pos,dist);
			cpy_vec(&dr,&stop);
			sub_vec(&dr,&start);
			modr=mod_vec(&dr);
			complex cresult=cexp(I*k*modr*-1.0)/modr;
			result+=dx*E[j]*cabs(cresult);
		}
	}
far[j]=fabs(result);
xfar[j]=pos;
pos+=dfar;

}

out=fopen("./far.dat","w");
for (j=0;j<len;j++)
{
	far_avg[j]+=far[j];
	fprintf(out,"%le %le\n",xfar[j], far_avg[j]/((double)far_steps));
}
fclose(out);

far_steps++;

free(far);
free(xfar);
}
