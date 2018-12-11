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

      if(my_id == root_process) {
         
         /* I must be the root process, so I will query the user
          * to determine how many numbers to sum. */

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
/*
int main()
{
int i;	
int j;
int zlen=300;
int ylen=300;
double x[zlen];
double y[ylen];
double z[zlen];
double far[zlen];
double xsize=1e-5;
double zsize=10e-9;
double ysize=10e-9;
double dt=1e-21;
double dt2=dt/2.0;

double c=3e8;

double dx=xsize/((double)zlen);
double dy=ysize/((double)ylen);
double dz=zsize/((double)zlen);
double min=1.0/(c*sqrt(pow(1.0/dy,2.0)+pow(1.0/dz,2.0)));
//dt=min/10.0;
printf ("dy=%lf nm, dz=%lf nm min_dt=%le dt=%le\n",dy*1e9,dz*1e9,min,dt);




double f=c/lambda;
double omega=2.0*3.14159*f;
double **Ex;
double **Ey;
double **Ez;
double **Hx;
double **Hy;
double **Hz;
double **Ex_last;
double **Ey_last;
double **Ez_last;
double **Hx_last;
double **Hy_last;
double **Hz_last;
double **epsilon_r;
double **z_ang;

Ex=(double **)malloc(sizeof(double*)*zlen);
Ey=(double **)malloc(sizeof(double*)*zlen);
Ez=(double **)malloc(sizeof(double*)*zlen);
Hx=(double **)malloc(sizeof(double*)*zlen);
Hy=(double **)malloc(sizeof(double*)*zlen);
Hz=(double **)malloc(sizeof(double*)*zlen);
Ex_last=(double **)malloc(sizeof(double*)*zlen);
Ey_last=(double **)malloc(sizeof(double*)*zlen);
Ez_last=(double **)malloc(sizeof(double*)*zlen);
Hx_last=(double **)malloc(sizeof(double*)*zlen);
Hy_last=(double **)malloc(sizeof(double*)*zlen);
Hz_last=(double **)malloc(sizeof(double*)*zlen);
epsilon_r=(double **)malloc(sizeof(double*)*zlen);
z_ang=(double **)malloc(sizeof(double*)*zlen);
for (i=0;i<zlen;i++)
{
Ex[i]=(double *)malloc(sizeof(double)*ylen);
Ey[i]=(double *)malloc(sizeof(double)*ylen);
Ez[i]=(double *)malloc(sizeof(double)*ylen);
Hx[i]=(double *)malloc(sizeof(double)*ylen);
Hy[i]=(double *)malloc(sizeof(double)*ylen);
Hz[i]=(double *)malloc(sizeof(double)*ylen);
Ex_last[i]=(double *)malloc(sizeof(double)*ylen);
Ey_last[i]=(double *)malloc(sizeof(double)*ylen);
Ez_last[i]=(double *)malloc(sizeof(double)*ylen);
Hx_last[i]=(double *)malloc(sizeof(double)*ylen);
Hy_last[i]=(double *)malloc(sizeof(double)*ylen);
Hz_last[i]=(double *)malloc(sizeof(double)*ylen);
epsilon_r[i]=(double *)malloc(sizeof(double)*ylen);
z_ang[i]=(double *)malloc(sizeof(double)*ylen);
}



double xpos=0.0;
double ypos=0.0;
double zpos=0.0;

double bEx=1.0;
double bEy=1.0;
double bEz=1.0;

double bHx=1.0;
double bHy=1.0;
double bHz=1.0;

double Exc=1.0;
double Eyc=1.0;
double Ezc=1.0;

double Hxc=1.0;
double Hyc=1.0;
double Hzc=1.0;

zpos=dz/2.0;
ypos=dy/2.0;

for (j=0;j<ylen;j++)
{
zpos=0.0;
	for (i=0;i<zlen;i++)
	{
		z[i]=zpos;

		zpos+=dz;

		Ex[i][j]=0.0;
		Ey[i][j]=0.0;
		Ez[i][j]=0.0;
		Hx[i][j]=0.0;
		Hy[i][j]=0.0;
		Hz[i][j]=0.0;
		Ex_last[i][j]=0.0;
		Ey_last[i][j]=0.0;
		Ez_last[i][j]=0.0;
		Hx_last[i][j]=0.0;
		Hy_last[i][j]=0.0;
		Hz_last[i][j]=0.0;
		epsilon_r[i][j]=1.0;
		z_ang[i][j]=1.0;


	}
y[j]=ypos;
ypos+=dy;
}
int pos=0;
int step=0;
double Exl=0.0;
double Hyl=0.0;

FILE *gnuplot = popen("gnuplot -persist","w");
fprintf(gnuplot, "set terminal x11 title 'Solarsim' \n");
fflush(gnuplot);


int llen=5;
int lcount=0;
int on=0;
for (i=0;i<zlen;i++)
{
	for (j=0;j<ylen;j++)
	{
		if (on==1) epsilon_r[i][j]=3.0;
	}
lcount++;

if (lcount>=llen)
{
	lcount=0;
	if (on==0)
	{
		on=1;
	}else
	{
		on=0;
	}
}

}

for (j=ylen*0.7;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		epsilon_r[i][j]=1.0;
	}

}

for (j=0;j<ylen*0.1;j++)
{
	for (i=0;i<zlen;i++)
	{
		epsilon_r[i][j]=14.0;
	}

}


#ifdef derfac
for (j=ylen*0.5;j<ylen*0.55;j++)
{
	for (i=0;i<zlen*0.35;i++)
	{
		epsilon_r[i][j]=40.0;
	}

}

for (j=ylen*0.5;j<ylen*0.55;j++)
{
	for (i=zlen*0.4;i<zlen*0.6;i++)
	{
		epsilon_r[i][j]=40.0;
	}

}

for (j=ylen*0.5;j<ylen*0.55;j++)
{
	for (i=zlen*0.65;i<zlen;i++)
	{
		epsilon_r[i][j]=40.0;
	}

}
#endif
double time=0.0;
do
{
pos=0;


double c=1.0/sqrt(epsilon0*mu0);
double Hy_last_r=0.0;

double Cx=(dt2/(epsilon0*dx));
double Cy=(dt2/(epsilon0*dy));
double Cz=(dt2/(epsilon0*dz));
for (j=1;j<(ylen-1);j++)
{
	for (i=1;i<(zlen-1);i++)
	{
		Ex[i][j]=Ex[i][j]+(Hz[i][j+1-1]-Hz[i][j-1])*epsilon_r[i][j]*Cy-(Hy[i+1-1][j]-Hy[i-1][j])*epsilon_r[i][j]*Cz;
		Ey[i][j]=Ey[i][j]+(Hx[i+1-1][j]-Hx[i-1][j])*epsilon_r[i][j]*Cz;
		Ez[i][j]=Ez[i][j]-(Hx[i][j+1-1]-Hx[i][j-1])*epsilon_r[i][j]*Cy;
	}
}

//for (j=24;j<25;j++)
//{
j=ylen-2;
double start=z[1];
double stop=z[zlen-2];
	for (i=1;i<zlen;i++)
//i=ylen/4;
//double start=z[10];
//double stop=z[zlen-10];
//	for (j=10;j<ylen-10;j++)
	{

		double a=0.0;
		double b=0.0;
		double c=0.0;
		double phi=0.0;
		double theta=0.0;


		double dot=0.0;
		phi=90.0;
		theta=90.0;
		double shift=20.0;
		a=sin(theta*(2.0*pi/360.0))*cos(phi*(2.0*pi/360.0));
		b=sin(theta*(2.0*pi/360.0))*sin(phi*(2.0*pi/360.0));
		c=cos(theta*(2.0*pi/360.0));
		dot=tan(2.0*pi*(shift)/360.0)*(z[i]-start)*2.0*pi/lambda;
		//dot=tan(2.0*pi*(shift)/360.0)*(y[i]-start)*2.0*pi/lambda;
		double mod=1.0;
		Ex[i][j]+=mod*a*sin(dot-time*omega);
		Ey[i][j]+=mod*b*sin(dot-time*omega);
		Ez[i][j]+=mod*c*sin(dot-time*omega);
		
		//printf("%lf %lf %lf %lf\n",sin(dot),a,b,c);
		//Hy[i][j]=cos(((double)step)/(2.0*3.14159*10.0));
	}
//}
for (j=0;j<(ylen-1);j++)
{
	Ex[0][j]=Ex_last[zlen-2][j];//Ex_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ex[1][j]-Ex_last[0][j]);
	Ey[0][j]=Ey_last[zlen-2][j];//Ey_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ey[1][j]-Ey_last[0][j]);
	Ez[0][j]=Ez_last[zlen-2][j];//Ez_last[1][j]+((c*dt2-dz)/(c*dt2+dz))*(Ez[1][j]-Ez_last[0][j]);

	Ex[zlen-1][j]=Ex[1][j];//Ex_last[zlen-2][j]+((c*dt2-dz)/(c*dt2+dz))*(Ex[zlen-2][j]-Ex_last[zlen-1][j]);
	Ey[zlen-1][j]=Ey[1][j];//Ey_last[zlen-2][j]+((c*dt2-dz)/(c*dt2+dz))*(Ey[zlen-2][j]-Ey_last[zlen-1][j]);
	Ez[zlen-1][j]=Ez[1][j];//Ez_last[zlen-2][j]+((c*dt2-dz)/(c*dt2+dz))*(Ez[zlen-2][j]-Ez_last[zlen-1][j]);
}

for (i=0;i<(zlen-1);i++)
{
	Ex[i][0]=Ex_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][1]-Ex_last[i][0]);
	Ey[i][0]=Ey_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ey[i][1]-Ey_last[i][0]);
	Ez[i][0]=Ez_last[i][1]+((c*dt2-dy)/(c*dt2+dy))*(Ez[i][1]-Ez_last[i][0]);

	Ex[i][ylen-1]=Ex_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ex[i][ylen-2]-Ex_last[i][zlen-1]);
	Ey[i][ylen-1]=Ey_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ey[i][ylen-2]-Ey_last[i][zlen-1]);
	Ez[i][ylen-1]=Ez_last[i][ylen-2]+((c*dt2-dy)/(c*dt2+dy))*(Ez[i][ylen-2]-Ez_last[i][zlen-1]);

}

Cy=(dt2/(mu0*dy));
Cx=(dt2/(mu0*dx));
Cz=(dt2/(mu0*dy));

for (j=0;j<(ylen-1);j++)
{
	for (i=0;i<(zlen-1);i++)
	{
		Hx[i][j]=Hx[i][j]-(Ez[i][j+1]-Ez[i][j])*Cy+(Ey[i+1][j]-Ey[i][j])*Cz;
		Hy[i][j]=Hy[i][j]-(Ex[i+1][j]-Ex[i][j])*Cz;
		Hz[i][j]=Hz[i][j]+(Ex[i][j+1]-Ex[i][j])*Cy;
	}
}
//for (i=0;i<zlen;i++)
//{
//	printf("Ex=%le Ey=%le Ez=%le Hx=%le Hy=%le Hz=%le\n",Ex[i],Ey[i],Ez[i],Hx[i],Hy[i],Hz[i]);
//}
FILE *out;

if ((step%100)==0)
{
printf("plot!\n");
out=fopen("./Ex.dat","w");
for (j=ylen*0.9;j<ylen;j++)
{
	for (i=zlen*0.9;i<zlen;i++)
	{
		fprintf(out,"%le %le %le\n",z[i],y[j],Ex[i][j]);
	}

fprintf(out,"\n");
}
fclose(out);

out=fopen("./Ey.dat","w");
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		fprintf(out,"%le %le %le\n",z[i],y[j],Ey[i][j]);
	}

fprintf(out,"\n");
}
fclose(out);

out=fopen("./Ez.dat","w");
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		fprintf(out,"%le %le %le\n",z[i],y[j],Ez[i][j]);
	}

fprintf(out,"\n");
}
fclose(out);

#ifdef Hfield
out=fopen("./Hx.dat","w");
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		fprintf(out,"%le %le %le\n",z[i],y[j],Hx[i][j]);
	}

fprintf(out,"\n");
}
fclose(out);

out=fopen("./Hy.dat","w");
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		fprintf(out,"%le %le %le\n",z[i],y[j],Hy[i][j]);
	}

fprintf(out,"\n");
}
fclose(out);


out=fopen("./Hz.dat","w");
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		fprintf(out,"%le %le %le\n",z[i],y[j],Hz[i][j]);
	}

fprintf(out,"\n");
}
fclose(out);

out=fopen("./epsilon_r.dat","w");
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		fprintf(out,"%le %le %le\n",z[i],y[j],epsilon_r[i][j]);
	}

fprintf(out,"\n");
}
fclose(out);
#endif

struct vec E;
struct vec H;
struct vec S;
struct vec n_z;
set_vec(&n_z, 0.0,0.0, 1.0);

for (i=0;i<zlen;i++)
{
	for (j=0;j<ylen;j++)
	{
		set_vec(&E, Ex[i][j],Ey[i][j], Ez[i][j]);
		//print_vec(&E);
		set_vec(&H, Hx[i][j],Hy[i][j], Hz[i][j]);
		//print_vec(&H);
		cros_vec(&S,&E,&H);
		//print_vec(&S);
		div_vec(&S,mu0);
		z_ang[i][j]=mod_vec(&S);//ang_vec(&n_z,&S);//
		if isnan ( z_ang[i][j] ) z_ang[i][j]=0.0;
	}
}

out=fopen("./z_ang.dat","w");
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		fprintf(out,"%le %le %le\n",z[i],y[j],z_ang[i][j]);
	}

fprintf(out,"\n");
}
fclose(out);
for (j=0;j<zlen;j++)
{
far[j]=Ex[j][(int)(ylen*0.7)];
}

//near_to_far(z, far,zlen);

fprintf(gnuplot, "load 'Ex.plot'\n");
fflush(gnuplot);
}

step++;
for (j=0;j<ylen;j++)
{
	for (i=0;i<zlen;i++)
	{
		Ex_last[i][j]=Ex[i][j];
		Ey_last[i][j]=Ey[i][j];
		Ez_last[i][j]=Ez[i][j];
		Hx_last[i][j]=Hx[i][j];
		Hy_last[i][j]=Hy[i][j];
		Hz_last[i][j]=Hz[i][j];
		//printf("%le %le\n",Ex_last[i],Hy_last[i]);
	}
}

//usleep(10000);
//getchar();
time+=dt;
//printf("%ld\n",step);
}while(step<200000);

fprintf(gnuplot, "exit\n");
fflush(gnuplot);
pclose(gnuplot);

Ex[i];
Ey[i];
Ez[i];
Hx[i];
Hy[i];
Hz[i];
Ex_last[i];
Ey_last[i];
Ez_last[i];
Hx_last[i];
Hy_last[i];
Hz_last[i];
epsilon_r[i];

return 0;
}
*/
