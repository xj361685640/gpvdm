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
#include "sim.h"
#include "solver_interface.h"
#include <cal_path.h>

long double min_thermal_error=2e-11;

void update_heat(struct device *in)
{
int i;
long double Ecl=0.0;
long double Ecr=0.0;
long double Evl=0.0;
long double Evr=0.0;
long double yl=0.0;
long double yr=0.0;
long double dh=0.0;
long double Jn=0.0;
long double Jp=0.0;

for (i=0;i<in->ymeshpoints;i++)
{
	if (i==0)
	{
		Ecl=-in->Xi[0]-in->Vapplied;
		Evl=-in->Xi[0]-in->Vapplied-in->Eg[0];
		yl=in->ymesh[0]-(in->ymesh[1]-in->ymesh[0]);
	}else
	{
		Ecl=in->Ec[i-1];
		Evl=in->Ev[i-1];
		yl=in->ymesh[i-1];
	}

	if (i==in->ymeshpoints-1)
	{
		Ecr=-in->Xi[i]-in->Vr;
		Evr=-in->Xi[i]-in->Vr-in->Eg[i];
		yr=in->ymesh[i]+(in->ymesh[i]-in->ymesh[i-1]);
		
	}else
	{
		Ecr=in->Ec[i];
		Evr=in->Ev[i];
		yr=in->ymesh[i];
	}

	dh=yr-yl;
	Jn=in->Jn[i];
	Jp=in->Jp[i];
	
	in->He[i]=(Ecr-Ecl)*Jn/dh;//+in->Hasorb[i]/2.0;
	in->Hh[i]=(Evr-Evl)*Jp/dh;//+in->Hasorb[i]/2.0;
	//printf("%Le\n",in->Hasorb[i]);
	//getchar();
	in->ke[i]=(5/2+1.5)*kb*(kb/Q)*in->Te[i]*in->mun[i]*in->n[i];
	in->kh[i]=(5/2+1.5)*kb*(kb/Q)*in->Th[i]*in->mup[i]*in->p[i];
	
	//printf("%Le\n",in->He[i]);
	//getchar();
}

}

void dump_thermal(struct device *in)
{
int i;
FILE *out;
out=fopen("./thermal.dat","w");
for (i=0;i<in->ymeshpoints;i++)
{
	printf("%Le Tl=%Lf Te=%Lf Th=%Lf %Le %Le %Le %Le \n",in->ymesh[i],in->Tl[i],in->Te[i],in->Th[i],in->Hl[i],in->He[i],in->Hh[i],in->kl[i]);
}

fclose(out);
}

long double get_thermal_error(struct device *in,long double *b)
{
long double tot=0.0;
int i;
for (i=0;i<in->ymeshpoints;i++)
{
	if (in->thermal_l==TRUE)
	{
		tot+=gfabs(b[i]);
	}

	if (in->thermal_e==TRUE)
	{
		tot+=gfabs(b[in->ymeshpoints+i]);
	}

	if (in->thermal_h==TRUE)
	{
		tot+=gfabs(b[in->ymeshpoints+in->ymeshpoints+i]);
	}
}
return tot;
}


int solve_thermal(struct simulation *sim,struct device *in)
{
printf("Solve thermal l=%d e=%d h=%d\n",in->thermal_l,in->thermal_e,in->thermal_h);

int i;

int N=0;
int M=0;

int *Ti;
int *Tj;
long double *Tx;
long double *b;

long double Tll;
long double Tlc;
long double Tlr;
long double Tel;
long double Tec;
long double Ter;
long double Thl;
long double Thc;
long double Thr;
long double yl;
long double yc;
long double yr;
long double dyl=0.0;
long double dyc=0.0;
long double dyr=0.0;
int ittr=0;
int pos=0;
long double error;
long double kll=0.0;
long double klc=0.0;
long double klr=0.0;
long double kl0=0.0;
long double kl1=0.0;
long double dTll=0.0;
long double dTlc=0.0;
long double dTlr=0.0;
long double dTl=0.0;
long double Hl=0.0;

long double kel=0.0;
long double kec=0.0;
long double ker=0.0;
long double ke0=0.0;
long double ke1=0.0;
long double dTel=0.0;
long double dTec=0.0;
long double dTer=0.0;
long double dTe=0.0;
long double He=0.0;

long double khl=0.0;
long double khc=0.0;
long double khr=0.0;
long double kh0=0.0;
long double kh1=0.0;
long double dThl=0.0;
long double dThc=0.0;
long double dThr=0.0;
long double dTh=0.0;
long double Hh=0.0;
long double n=0.0;
long double p=0.0;
long double dTedTl=0.0;
long double dThdTl=0.0;
long double dTldTe=0.0;
long double dTldTh=0.0;

long double Jnl=0.0;
long double Jnc=0.0;
long double Jnr=0.0;

long double Jpl=0.0;
long double Jpc=0.0;
long double Jpr=0.0;

long double Snl=0.0;
long double Snr=0.0;
long double dSndy=0.0;
long double dSnldTe=0.0;
long double dSnrdTe=0.0;

long double Spl=0.0;
long double Spr=0.0;
long double dSpdy=0.0;
long double dSpldTh=0.0;
long double dSprdTh=0.0;

if (in->thermal_l==TRUE)
{
	N+=in->ymeshpoints*3-2;			//dTldTl
	M+=in->ymeshpoints;
}

if (in->thermal_e==TRUE)
{
	N+=in->ymeshpoints*3-2;			//dTedTe
	N+=in->ymeshpoints;				//dTedTl
	N+=in->ymeshpoints;				//dTldTe
	M+=in->ymeshpoints;
}

if (in->thermal_h==TRUE)
{
	N+=in->ymeshpoints*3-2;			//dThdTh
	N+=in->ymeshpoints;				//dThdTl
	N+=in->ymeshpoints;				//dTldTh
	M+=in->ymeshpoints;
}



Ti=malloc(N*sizeof(int));
Tj=malloc(N*sizeof(int));
Tx=malloc(N*sizeof(long double));
b=malloc(M*sizeof(long double));


in->thermal_conv=FALSE;
update_heat(in);

	do
	{

		pos=0;
		for (i=0;i<in->ymeshpoints;i++)
		{


			if (i==0)
			{
				kll=in->kl[0];
				kel=in->ke[0];
				khl=in->kh[0];
				//printf("%d\n",in->Tliso);
				//getchar();
				if (in->Tliso==FALSE)
				{
					Tel=in->Tll;
					Thl=in->Tll;
					Tll=in->Tll;
				}else
				{
					Tll=in->Tl[0];
					Tel=in->Tl[0];
					Thl=in->Tl[0];
				}


				yl=in->ymesh[0]-(in->ymesh[1]-in->ymesh[0]);
				Jnl=in->Jn[0];
				Jpl=in->Jp[0];
			}else
			{
				kll=in->kl[i-1];
				kel=in->ke[i-1];
				khl=in->kh[i-1];
				Tll=in->Tl[i-1];
				Tel=in->Te[i-1];
				Thl=in->Th[i-1];
				yl=in->ymesh[i-1];
				Jnl=in->Jn[i-1];
				Jpl=in->Jp[i-1];
			}

			if (i==(in->ymeshpoints-1))
			{
				klr=in->kl[in->ymeshpoints-1];
				ker=in->ke[in->ymeshpoints-1];
				khr=in->kh[in->ymeshpoints-1];
				Jnr=in->Jn[in->ymeshpoints-1];
				Jpr=in->Jp[in->ymeshpoints-1];

				yr=in->ymesh[i]+(in->ymesh[i]-in->ymesh[i-1]);

				if (in->Triso==FALSE)
				{
					Ter=in->Tlr;
					Thr=in->Tlr;
					Tlr=in->Tlr;
				}else
				{
					Tlr=in->Tl[in->ymeshpoints-1];
					Ter=in->Tl[in->ymeshpoints-1];
					Thr=in->Tl[in->ymeshpoints-1];
				}
			}else
			{
				klr=in->kl[i+1];
				ker=in->ke[i+1];
				khr=in->kh[i+1];
				Tlr=in->Tl[i+1];
				Ter=in->Te[i+1];
				Thr=in->Th[i+1];
				Jnr=in->Jn[i+1];
				Jpr=in->Jp[i+1];
				yr=in->ymesh[i+1];
			}


			yc=in->ymesh[i];
			dyl=yc-yl;
			dyr=yr-yc;
			
			dyc=(dyl+dyr)/2.0;

			klc=in->kl[i];
			kec=in->ke[i];
			khc=in->kh[i];


			kl0=(kll+klc)/2.0;
			kl1=(klc+klr)/2.0;

			ke0=(kel+kec)/2.0;
			ke1=(kec+ker)/2.0;

			kh0=(khl+khc)/2.0;
			kh1=(khc+khr)/2.0;


			Tlc=in->Tl[i];
			Tec=in->Te[i];
			Thc=in->Th[i];
			Hl=in->Hl[i];
			He=in->He[i];
			Hh=in->Hh[i];

			n=in->n[i];
			p=in->p[i];

			Jnc=in->Jn[i];
			Jpc=in->Jp[i];


			if (in->thermal_l==TRUE)
			{
				dTll=kl0/dyl/dyc;
				dTlc=-(kl0/dyl/dyc+kl1/dyr/dyc);
				dTlr=kl1/dyr/dyc;
			
				dTl=dTll*Tll+dTlc*Tlc+dTlr*Tlr+Hl;

				if ((in->Tliso==TRUE)&&(i==0))
				{
					dTlc+=dTll;
					dTll=0.0;
				}

				if ((in->Triso==TRUE)&&(i==(in->ymeshpoints-1)))
				{
					dTlc+=dTlr;
					dTlr=0.0;
				}
			}

			if (in->thermal_e==TRUE)
			{
				dTel=ke0/dyl/dyc;
				dTec=-(ke0/dyl/dyc+ke1/dyr/dyc);
				dTer=ke1/dyr/dyc;

				dTe=dTel*Tel+dTec*Tec+dTer*Ter-(3.0/2.0)*kb*n*(Tec-Tlc)/in->thermal_tau_e;

				dTe+=He;

				dTl+=(3.0/2.0)*kb*n*(Tec-Tlc)/in->thermal_tau_e;

				dTlc+=-(3.0/2.0)*kb*n/in->thermal_tau_e;
				dTldTe=(3.0/2.0)*kb*n/in->thermal_tau_e;



				dTec+=-(3.0/2.0)*kb*n/in->thermal_tau_e;
				dTedTl=(3.0/2.0)*kb*n/in->thermal_tau_e;

				Snl=(5.0/2.0)*(kb/Q)*Tel*Jnl;
				Snr=(5.0/2.0)*(kb/Q)*Ter*Jnr;
				dSndy=-(Snr-Snl)/(dyl+dyr);
				dSnldTe=(5.0/2.0)*(kb/Q)*Jnl/(dyl+dyr);
				dSnrdTe=-(5.0/2.0)*(kb/Q)*Jnr/(dyl+dyr);

				dTel+=dSnldTe;
				dTer+=dSnrdTe;

			}else
			{
				dTl+=He;
			}

			if (in->thermal_h==TRUE)
			{
				dThl=kh0/dyl/dyc;
				dThc=-(kh0/dyl/dyc+kh1/dyr/dyc);
				dThr=kh1/dyr/dyc;

				dTh=dThl*Thl+dThc*Thc+dThr*Thr-(3.0/2.0)*kb*p*(Thc-Tlc)/in->thermal_tau_h;

				dTl+=(3.0/2.0)*kb*p*(Thc-Tlc)/in->thermal_tau_h;
				dTlc+=-(3.0/2.0)*kb*p/in->thermal_tau_h;
				dTldTh=(3.0/2.0)*kb*p/in->thermal_tau_h;

				dTh+=Hh;

				dThc+=-(3.0/2.0)*kb*p/in->thermal_tau_h;
				dThdTl=(3.0/2.0)*kb*p/in->thermal_tau_h;

				Spl=(5.0/2.0)*(kb/Q)*Thl*Jpl;
				Spr=(5.0/2.0)*(kb/Q)*Thr*Jpr;
				dSpdy=-(Spr-Spl)/(dyl+dyr);
				dSpldTh=(5.0/2.0)*(kb/Q)*Jpl/(dyl+dyr);
				dSprdTh=-(5.0/2.0)*(kb/Q)*Jpr/(dyl+dyr);

				dThl+=dSpldTh;
				dThr+=dSprdTh;
			}else
			{
				dTl+=Hh;
			}




			/*if ((in->Tliso==TRUE)&&(i==0))
			{
				dTlc+=dTel;
				dTel=0.0;
			}*/


			/*if ((in->Tliso==TRUE)&&(i==0))
			{
				dTlc+=dThl;
				dThl=0.0;
			}*/


			//Build matrix
			if (i!=0)
			{
				Ti[pos]=i;
				Tj[pos]=i-1;
				Tx[pos]=dTll;
				pos++;

				if (in->thermal_e==TRUE)
				{
					Ti[pos]=in->ymeshpoints+i;
					Tj[pos]=in->ymeshpoints+i-1;
					Tx[pos]=dTel;
					pos++;
				}

				if (in->thermal_h==TRUE)
				{
					Ti[pos]=2*in->ymeshpoints+i;
					Tj[pos]=2*in->ymeshpoints+i-1;
					Tx[pos]=dThl;
					pos++;
				}


			}

			//lattice
			Ti[pos]=i;
			Tj[pos]=i;
			Tx[pos]=dTlc;
			pos++;

			if (in->thermal_e==TRUE)
			{
				Ti[pos]=i;
				Tj[pos]=in->ymeshpoints+i;
				Tx[pos]=dTldTe;
				pos++;

				Ti[pos]=in->ymeshpoints+i;
				Tj[pos]=in->ymeshpoints+i;
				Tx[pos]=dTec;
				pos++;


				Ti[pos]=in->ymeshpoints+i;
				Tj[pos]=i;
				Tx[pos]=dTedTl;
				pos++;
			}

			if (in->thermal_h==TRUE)
			{
				Ti[pos]=i;
				Tj[pos]=2*in->ymeshpoints+i;
				Tx[pos]=dTldTh;
				pos++;

				Ti[pos]=2*in->ymeshpoints+i;
				Tj[pos]=2*in->ymeshpoints+i;
				Tx[pos]=dThc;
				pos++;

				Ti[pos]=2*in->ymeshpoints+i;
				Tj[pos]=i;
				Tx[pos]=dThdTl;
				pos++;
			}




			if (i!=(in->ymeshpoints-1))
			{
				Ti[pos]=i;
				Tj[pos]=i+1;
				Tx[pos]=dTlr;
				pos++;

				if (in->thermal_e==TRUE)
				{
					Ti[pos]=in->ymeshpoints+i;
					Tj[pos]=in->ymeshpoints+i+1;
					Tx[pos]=dTer;
					pos++;
				}

				if (in->thermal_h==TRUE)
				{
					Ti[pos]=2*in->ymeshpoints+i;
					Tj[pos]=2*in->ymeshpoints+i+1;
					Tx[pos]=dThr;
					pos++;
				}

			}

			b[i]=-(dTl);

			if (in->thermal_e==TRUE)
			{
				b[i+in->ymeshpoints]=-(dTe+dSndy);
			}

			if (in->thermal_h==TRUE)
			{
				b[i+2*in->ymeshpoints]=-(dTh+dSpdy);
			}

		}


		solver(sim,M,N,Ti,Tj, Tx,b);

		for (i=0;i<in->ymeshpoints;i++)
		{
			long double clamp=100.0;
			in->Tl[i]+=b[i]/(1.0+fabs(b[i]/clamp/(300.0*kb/Q)));

			if (in->thermal_e==TRUE)
			{
				in->Te[i]+=b[i+in->ymeshpoints]/(1.0+fabs(b[i+in->ymeshpoints]/clamp/(300.0*kb/Q)));
			}else
			{
				in->Te[i]=in->Tl[i];
			}

			if (in->thermal_h==TRUE)
			{
				in->Th[i]+=b[i+2*in->ymeshpoints]/(1.0+fabs(b[i+2*in->ymeshpoints]/clamp/(300.0*kb/Q)));
			}else
			{
				in->Th[i]=in->Tl[i];
			}



			/*in->Tl[i]+=b[i];
			in->Te[i]+=b[i+in->ymeshpoints];
			in->Th[i]+=b[i+2*in->ymeshpoints];*/
		}


		error=get_thermal_error(in,b);
		printf("%ld Thermal error = %Le %Le\n",ittr,error,min_thermal_error);
		if ((ittr<2)&&(error<min_thermal_error)) in->thermal_conv=TRUE;
		ittr++;
		
	}while((ittr<200)&&(error>min_thermal_error));

dump_thermal(in);
/*
for (i=0;i<in->ymeshpoints;i++)
{
	if (in->Tl[i]>2000) in->Tl[i]=2000.0;
	if (in->Te[i]>2000) in->Te[i]=2000.0;
	if (in->Th[i]>2000) in->Th[i]=2000.0;

}
*/

free(Ti);
free(Tj);
free(Tx);
free(b);
return 0;
}

