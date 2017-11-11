//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
#include <stdlib.h>
#include <dump.h>
#include <dos.h>
#include "sim.h"
#include "solver_interface.h"
#include "buffer.h"
#include "log.h"
#include <cal_path.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <lang.h>


gdouble min_pos_error=1e-4;

void pos_dump(struct simulation *sim,struct device *in)
{
if (get_dump_status(sim,dump_first_guess)==TRUE)
{
	struct stat st = {0};

	char out_dir[PATHLEN];
	join_path(2,out_dir,get_output_path(sim),"equilibrium");

	if (stat(out_dir, &st) == -1)
	{
		mkdir(out_dir, 0700);
	}

	struct buffer buf;
	buffer_init(&buf);
	char name[200];
	int band=0;
	int i=0;

	buffer_malloc(&buf);
	sprintf(name,"%s%s","first_guess_Fi",".dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,_("Intrinsic Fermi - position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,"Fi");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"$E_{LUMO}$");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buffer_add_info(sim,&buf);
	buffer_add_3d_device_data(sim,&buf,in, in->Fi);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s%s","first_guess_Ec",".dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,_("LUMO energy - position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,"LUMO");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"$E_{LUMO}$");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buffer_add_info(sim,&buf);
	buffer_add_3d_device_data(sim,&buf,in, in->Ec);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s%s","first_guess_Ev",".dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,_("HOMO energy - position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Position");
	strcpy(buf.y_label,"LUMO");
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"$E_{HOMO}$");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buffer_add_info(sim,&buf);
	buffer_add_3d_device_data(sim,&buf,in, in->Ev);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);


	buffer_malloc(&buf);
	sprintf(name,"%s%s","first_guess_n",".dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,_("Electron density - position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,_("Electron density"));
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"m^{-3}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buffer_add_info(sim,&buf);
	buffer_add_3d_device_data(sim,&buf,in, in->n);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);


	buffer_malloc(&buf);
	sprintf(name,"%s%s","first_guess_p",".dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,_("Hole density - position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,_("Hole density"));
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"m^{-3}");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buffer_add_info(sim,&buf);
	buffer_add_3d_device_data(sim,&buf,in, in->p);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);

	buffer_malloc(&buf);
	sprintf(name,"%s%s","first_guess_phi",".dat");
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,_("Potential - position"));
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,_("Position"));
	strcpy(buf.y_label,_("Potential"));
	strcpy(buf.x_units,"nm");
	strcpy(buf.y_units,"V");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buffer_add_info(sim,&buf);
	buffer_add_3d_device_data(sim,&buf,in, in->phi);
	buffer_dump_path(sim,out_dir,name,&buf);
	buffer_free(&buf);



	/*out=fopena(get_output_path(sim),"first_guess_np_trap.dat","w");
	for (i=0;i<in->ymeshpoints;i++)
	{
		fprintf(out,"%Le ",in->ymesh[i]);
		for (band=0;band<in->srh_bands;band++)
		{
			fprintf(out,"%Le %Le ",in->nt[i][band],in->pt[i][band]);
		}
		fprintf(out,"\n");
	}
	fclose(out);*/

}
}

long double get_p_error(struct device *in,long double *b)
{
gdouble tot=0.0;
int i;
for (i=0;i<in->ymeshpoints;i++)
{
	if ((in->interfaceleft==TRUE)&&(i==0))
	{
	}else
	if ((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1))
	{
	}else
	{
	tot+=gfabs(b[i]);
	}
}
return tot;
}


int solve_pos(struct simulation *sim,struct device *in, int z, int x)
{
if (get_dump_status(sim,dump_iodump)==TRUE)
{
	printf_log(sim,"%s\n",_("Solving Poisson's equation"));
}

int i;
int y;

int N=in->ymeshpoints*3-2;


int M=in->ymeshpoints;
int *Ti =    malloc(N*sizeof(int));
int *Tj =    malloc(N*sizeof(int));
gdouble *Tx = malloc(N*sizeof(gdouble));
gdouble *b = malloc(M*sizeof(gdouble));


gdouble phil;
gdouble phic;
gdouble phir;
gdouble yl;
gdouble yc;
gdouble yr;
gdouble dyl;
gdouble dyr;
gdouble dyc=0.0;
int ittr=0;
int pos=0;
gdouble error=1000;
gdouble el=0.0;
gdouble ec=0.0;
gdouble er=0.0;
gdouble e0=0.0;
gdouble e1=0.0;
int pos_max_ittr=250;

int quit=FALSE;
int adv_step=0;
int adv=FALSE;
int band;


gdouble kTq=(in->Te[z][x][0]*kb/Q);

	do
	{

		if (in->interfaceleft==TRUE)
		{
			in->phi[z][x][0]=in->Vl;
		}

		if (in->interfaceright==TRUE)
		{
			in->phi[z][x][in->ymeshpoints-1]=in->Vr;
		}

		pos=0;

		for (i=0;i<in->ymeshpoints;i++)
		{


			if (i==0)
			{
				phil=in->Vl;
				el=in->epsilonr[z][x][0]*epsilon0;
				yl=in->ymesh[0]-(in->ymesh[1]-in->ymesh[0]);


			}else
			{
				el=in->epsilonr[z][x][i-1]*epsilon0;
				phil=in->phi[z][x][i-1];
				yl=in->ymesh[i-1];
			}

			if (i==(in->ymeshpoints-1))
			{
				phir=in->Vr;
				er=in->epsilonr[z][x][i]*epsilon0;
				yr=in->ymesh[i]+(in->ymesh[i]-in->ymesh[i-1]);
			}else
			{
				er=in->epsilonr[z][x][i+1]*epsilon0;
				phir=in->phi[z][x][i+1];
				yr=in->ymesh[i+1];

			}


			yc=in->ymesh[i];
			dyl=yc-yl;
			dyr=yr-yc;
			dyc=(dyl+dyr)/2.0;

			ec=in->epsilonr[z][x][i]*epsilon0;
			e0=(el+ec)/2.0;
			e1=(ec+er)/2.0;
			phic=in->phi[z][x][i];



			gdouble dphidn=0.0;
			if (adv==FALSE)
			{
				dphidn=(Q/(kb*in->Tl[z][x][i]))*in->Nc[z][x][i]*exp(((in->Fi[z][x][i]-(-in->phi[z][x][i]-in->Xi[z][x][i])))/(kTq));

			}else
			{
				dphidn=get_dn_den(in,in->Fi[z][x][i]-in->Ec[z][x][i],in->Tl[z][x][i],in->imat[z][x][i]);

			}


			gdouble dphidp=0.0;
			if (adv==FALSE)
			{
				dphidp= -(Q/(kb*in->Tl[z][x][i]))*in->Nv[z][x][i]*exp((((-in->phi[z][x][i]-in->Xi[z][x][i]-in->Eg[z][x][i])-in->Fi[z][x][i]))/(kTq));
			}else
			{
				dphidp= -get_dp_den(in,in->xp[z][x][i]-in->tp[z][x][i],in->Tl[z][x][i],in->imat[z][x][i]);
			}
			gdouble dphil=e0/dyl/dyc;
			gdouble dphic= -(e0/dyl/dyc+e1/dyr/dyc);
			gdouble dphir=e1/dyr/dyc;

			gdouble dphil_d=dphil;
			gdouble dphic_d=dphic;
			gdouble dphir_d=dphir;

			if (in->interfaceleft==TRUE)
			{

				if (i==1)
				{
					dphil_d=0.0;
					phil=in->Vl;

				}

				if (i==0)
				{
					dphil_d=0.0;
					dphic_d=1e-6;
					dphir_d=0.0;

				}
			}

			if (in->interfaceright==TRUE)
			{

				if (i==in->ymeshpoints-2)
				{
					dphir_d=0.0;
					phir=in->Vr;

				}

				if (i==in->ymeshpoints-1)
				{
					dphil_d=0.0;
					dphic_d=1e-6;
					dphir_d=0.0;

				}
			}

			gdouble dphi=dphil*phil+dphic*phic+dphir*phir;


			dphic_d+= -Q*(dphidn-dphidp); // just put in the _d to get it working again.

			//if (adv==TRUE)
			//{
			//	for (band=0;band<in->srh_bands;band++)
			//	{
			//		dphic_d+=(-q*(in->dnt[i][band]-in->dpt[i][band]));
			//	}
			//}

			if (i!=0)
			{
				Ti[pos]=i;
				Tj[pos]=i-1;
				Tx[pos]=dphil_d;
				pos++;
			}

			Ti[pos]=i;
			Tj[pos]=i;
			Tx[pos]=dphic_d;



			pos++;



			if (i!=(in->ymeshpoints-1))
			{
				Ti[pos]=i;
				Tj[pos]=i+1;
				Tx[pos]=dphir_d;
				pos++;

			}

			if ((in->interfaceleft==TRUE)&&(i==0))
			{
				b[i]= -0.0;
			}else
			if ((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1))
			{
				b[i]= -0.0;
			}else
			{
				b[i]= -(dphi-Q*(in->n[z][x][i]-in->p[z][x][i]-in->Nad[z][x][i])); //
				if (adv==TRUE)
				{
					for (band=0;band<in->srh_bands;band++)
					{
						b[i]+= -(-Q*(in->nt[z][x][i][band]-in->pt[z][x][i][band]));
					}
				}
			}
			//in->n[i]=in->Nc[z][x][i]*exp(((in->Fi[z][x][i]-in->Ec[z][x][i])*q)/(kb*in->Tl[z][x][i]));

		}

		error=get_p_error(in,b);

		solver(sim,M,N,Ti,Tj, Tx,b);


		for (i=0;i<in->ymeshpoints;i++)
		{
			if ((in->interfaceleft==TRUE)&&(i==0))
			{
			}else
			if ((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1))
			{
			}else
			{
			gdouble update;

			gdouble clamp_temp=300.0;
			update=b[i]/(1.0+fabs(b[i]/in->posclamp/(clamp_temp*kb/Q)));
			in->phi[z][x][i]+=update;

			}
		}

		//getchar();

		for (i=0;i<in->ymeshpoints;i++)
		{
			in->Ec[z][x][i]= -in->phi[z][x][i]-in->Xi[z][x][i];
			in->Ev[z][x][i]= -in->phi[z][x][i]-in->Xi[z][x][i]-in->Eg[z][x][i];

				if (adv==FALSE)
				{
					in->n[z][x][i]=in->Nc[z][x][i]*exp(((in->Fi[z][x][i]-in->Ec[z][x][i])*Q)/(kb*in->Tl[z][x][i]));
					in->dn[z][x][i]=(Q/(kb*in->Tl[z][x][i]))*in->Nc[z][x][i]*exp((Q*(in->Fi[z][x][i]-in->Ec[z][x][i]))/(kb*in->Tl[z][x][i]));
				}else
				{
					in->n[z][x][i]=get_n_den(in,in->Fi[z][x][i]-in->Ec[z][x][i],in->Tl[z][x][i],in->imat[z][x][i]);
					in->dn[z][x][i]=get_dn_den(in,in->Fi[z][x][i]-in->Ec[z][x][i],in->Tl[z][x][i],in->imat[z][x][i]);
				}

				in->Fn[z][x][i]=in->Fi[z][x][i];
				in->Fp[z][x][i]=in->Fi[z][x][i];

				in->x[z][x][i]=in->phi[z][x][i]+in->Fn[z][x][i];
				in->xp[z][x][i]= -(in->phi[z][x][i]+in->Fp[z][x][i]);


				if (adv==FALSE)
				{
					in->p[z][x][i]=in->Nv[z][x][i]*exp(((in->xp[z][x][i]-in->tp[z][x][i])*Q)/(kb*in->Tl[z][x][i]));
					in->dp[z][x][i]=(Q/(kb*in->Tl[z][x][i]))*in->Nv[z][x][i]*exp(((in->xp[z][x][i]-in->tp[z][x][i])*Q)/(kb*in->Tl[z][x][i]));
				}else
				{
					in->p[z][x][i]=get_p_den(in,in->xp[z][x][i]-in->tp[z][x][i],in->Tl[z][x][i],in->imat[z][x][i]);
					in->dp[z][x][i]=get_dp_den(in,in->xp[z][x][i]-in->tp[z][x][i],in->Tl[z][x][i],in->imat[z][x][i]);
				}


				for (band=0;band<in->srh_bands;band++)
				{

					in->Fnt[z][x][i][band]= -in->phi[z][x][i]-in->Xi[z][x][i]+dos_srh_get_fermi_n(in,in->n[z][x][i], in->p[z][x][i],band,in->imat[z][x][i],in->Te[z][x][i]);
					in->Fpt[z][x][i][band]= -in->phi[z][x][i]-in->Xi[z][x][i]-in->Eg[z][x][i]-dos_srh_get_fermi_p(in,in->n[z][x][i], in->p[z][x][i],band,in->imat[z][x][i],in->Th[z][x][i]);

					in->xt[z][x][i][band]=in->phi[z][x][i]+in->Fnt[z][x][i][band];
					in->nt[z][x][i][band]=get_n_pop_srh(sim,in,in->xt[z][x][i][band]+in->tt[z][x][i],in->Te[z][x][i],band,in->imat[z][x][i]);
					in->dnt[z][x][i][band]=get_dn_pop_srh(sim,in,in->xt[z][x][i][band]+in->tt[z][x][i],in->Te[z][x][i],band,in->imat[z][x][i]);


					in->xpt[z][x][i][band]= -(in->phi[z][x][i]+in->Fpt[z][x][i][band]);
					in->pt[z][x][i][band]=get_p_pop_srh(sim,in,in->xpt[z][x][i][band]-in->tpt[z][x][i],in->Th[z][x][i],band,in->imat[z][x][i]);
					in->dpt[z][x][i][band]=get_dp_pop_srh(sim,in,in->xpt[z][x][i][band]-in->tpt[z][x][i],in->Th[z][x][i],band,in->imat[z][x][i]);
				}

		}

		update_y_array(sim,in,z,x);

		in->xnl_left=in->x[z][x][0];
		in->xpl_left=in->xp[z][x][0];

		if (error<1)
		{
			adv=TRUE;
		}
		//#ifdef print_newtonerror

		//if (get_dump_status(sim,dump_print_pos_error)==TRUE)
		{
			printf_log(sim,"%d %s = %Le %d\n",ittr,_("Poisson solver error"),error,adv);
		}
		//#endif

		#ifdef dump_converge



		/*in->converge=fopena(get_output_path(sim),"converge.dat","a");
		fprintf(in->converge,"%e\n",error);
		fclose(in->converge);*/
		#endif



	ittr++;

	if (adv==TRUE)
	{
		adv_step++;
	}

	if (ittr>pos_max_ittr)
	{
		quit=TRUE;
	}

	if ((error<min_pos_error)&&(adv_step>3))
	{
		quit=TRUE;
	}

	}while(quit==FALSE);
		//getchar();

pos_dump(sim,in);

	update_y_array(sim,in,z,x);

	if (in->srh_sim==TRUE)
	{
		time_init(sim,in);

	}




in->odes+=in->ymeshpoints;



for (y=0;y<in->ymeshpoints;y++)
{

	in->nf_save[z][x][y]=in->n[z][x][y];
	in->pf_save[z][x][y]=in->p[z][x][y];
	in->nt_save[z][x][y]=0.0;
	in->pt_save[z][x][y]=0.0;
}



free(Ti);
free(Tj);
free(Tx);
free(b);




printf_log(sim,"%s\n",_("Solved Poisson's equation"));
printf_log(sim,"Vl=%Le Vr=%Le phi_mid=%Le\n",in->Vl,in->Vr, in->phi[z][x][in->ymeshpoints/2]);

return 0;
}

