//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file newton.c
	@brief Newton solver which uses slotboom variables.
*/


#include <stdlib.h>
#include <string.h>
#include <const.h>
#include "newton.h"
#include <dll_export.h>
#include <log.h>
#include <exp.h>
#include <util.h>
#include <dump.h>
#include <cal_path.h>
#include <dos.h>
#include <sim.h>
#include <solver_interface.h>
#include <contacts.h>
#include <plot.h>

static gdouble Jnl=0.0;
static gdouble Jnr=0.0;
static gdouble Jpl=0.0;
static gdouble Jpr=0.0;

static gdouble Dnl=0.0;
static gdouble Dnc=0.0;
static gdouble Dnr=0.0;
static gdouble Dpl=0.0;
static gdouble Dpc=0.0;
static gdouble Dpr=0.0;

static gdouble nl=0.0;
static gdouble nc=0.0;
static gdouble nr=0.0;

static gdouble pl=0.0;
static gdouble pc=0.0;
static gdouble pr=0.0;

static gdouble xil=0.0;
static gdouble xir=0.0;
static gdouble xipl=0.0;
static gdouble xipr=0.0;

static gdouble dJpdxipl=0.0;
static gdouble dJpdxipc=0.0;
static gdouble dJpdxipr=0.0;

static gdouble dnl=0.0;
static gdouble dnc=0.0;
static gdouble dnr=0.0;

static gdouble dpl=0.0;
static gdouble dpc=0.0;
static gdouble dpr=0.0;

static gdouble munl=0.0;
static gdouble munc=0.0;
static gdouble munr=0.0;

static gdouble mupl=0.0;
static gdouble mupc=0.0;
static gdouble mupr=0.0;


static gdouble wnl=0.0;
static gdouble wnc=0.0;
static gdouble wnr=0.0;

static gdouble wpl=0.0;
static gdouble wpc=0.0;
static gdouble wpr=0.0;

static gdouble dJdxil=0.0;
static gdouble dJdxic=0.0;
static gdouble dJdxir=0.0;

static gdouble dJdphil=0.0;
static gdouble dJdphic=0.0;
static gdouble dJdphir=0.0;

static gdouble dJpdphil=0.0;
static gdouble dJpdphic=0.0;
static gdouble dJpdphir=0.0;


static gdouble dphidxic=0.0;
static gdouble dphidxipc=0.0;

gdouble Vapplied=0.0;

static gdouble T0=1e20;
static gdouble D0=1.0;
static gdouble n0=1.0;
static gdouble phi0=0.0;
static gdouble l0=0.0;
static gdouble r0=0.0;
static gdouble r_bi0=0.0;


void update_solver_vars(struct simulation *sim,struct device *in, int z, int x,int clamp)
{
int i;
int band=0;

gdouble clamp_temp=300.0;

gdouble update=0.0;
	for (i=0;i<in->ymeshpoints;i++)
	{

		update=(gdouble)in->b[i]*phi0;
		if ((in->interfaceleft==TRUE)&&(i==0))
		{
		}else
		if ((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1))
		{
		}else
		{
			if (clamp==TRUE)
			{
				in->phi[z][x][i]+=update/(1.0+gfabs(update/in->electrical_clamp/(clamp_temp*kb/Q)));
			}else
			{
				in->phi[z][x][i]+=update;

			}
		}


		update=(gdouble)(in->b[in->ymeshpoints*(1)+i])*phi0;
		if (clamp==TRUE)
		{
			in->x[z][x][i]+=update/(1.0+gfabs(update/in->electrical_clamp/(clamp_temp*kb/Q)));
		}else
		{
			in->x[z][x][i]+=update;
		}


		update=(gdouble)(in->b[in->ymeshpoints*(1+1)+i])*phi0;
		if (clamp==TRUE)
		{
			in->xp[z][x][i]+=update/(1.0+gfabs(update/in->electrical_clamp/(clamp_temp*kb/Q)));
		}else
		{
			in->xp[z][x][i]+=update;

		}


		if (in->ntrapnewton==TRUE)
		{
			for (band=0;band<in->srh_bands;band++)
			{
				update=(gdouble)(in->b[in->ymeshpoints*(1+1+1+band)+i]);
				if (clamp==TRUE)
				{
					in->xt[z][x][i][band]+=update/(1.0+gfabs(update/in->electrical_clamp/(clamp_temp*kb/Q)));

				}else
				{
					in->xt[z][x][i][band]+=update;
				}
			}
		}

		if (in->ptrapnewton==TRUE)
		{
			for (band=0;band<in->srh_bands;band++)
			{
				update=(gdouble)(in->b[in->ymeshpoints*(1+1+1+in->srh_bands+band)+i]);
				if (clamp==TRUE)
				{
					in->xpt[z][x][i][band]+=update/(1.0+gfabs(update/in->electrical_clamp/(clamp_temp*kb/Q)));
				}else
				{
					in->xpt[z][x][i][band]+=update;

				}
			}
		}


		}


update_y_array(sim,in,z,x);

}

void fill_matrix(struct simulation *sim,struct device *in, int z, int x)
{
//gdouble offset= -0.5;
int band=0;

update_y_array(sim,in,z,x);

//FILE *file_j =fopen("myj.dat","w");
//getchar();
gdouble phil;
gdouble phic;
gdouble phir;
gdouble yl;
gdouble yc;
gdouble yr;
gdouble dyl;
gdouble dyr;
gdouble ddh=0.0;
//gdouble dh;
int pos=0;

gdouble Ecl=0.0;
gdouble Ecr=0.0;
gdouble Ecc=0.0;
gdouble Evl=0.0;
gdouble Evr=0.0;
gdouble Evc=0.0;

gdouble Tel=0.0;
//gdouble Tec=0.0;
gdouble Ter=0.0;

gdouble Thl=0.0;
//gdouble Thc=0.0;
gdouble Thr=0.0;

gdouble xnr;
gdouble tnr;
gdouble xnl;
gdouble tnl;

gdouble xpr;
gdouble tpr;
gdouble xpl;
gdouble tpl;


//gdouble exl;
//gdouble exr;
//gdouble exc;
//gdouble Dexl;
//gdouble Dexc;
//gdouble Dexr;
//gdouble R;

gdouble epr;
gdouble epc;
gdouble epl;

//gdouble G;
gdouble Gn;
gdouble Gp;
int i;
gdouble dJdxipc=0.0;
gdouble dJpdxic=0.0;

gdouble e0=0.0;
gdouble e1=0.0;

gdouble dphil=0.0;
gdouble dphic=0.0;
gdouble dphir=0.0;
gdouble deriv=0.0;

gdouble nlast=0.0;
gdouble plast=0.0;
gdouble dt=0.0;
//gdouble kll=0.0;
//gdouble klc=0.0;
//gdouble klr=0.0;

//gdouble kl0=0.0;
//gdouble kl1=0.0;

gdouble one=0.0;
gdouble one0_l=0.0;
gdouble one0_r=0.0;



gdouble Rtrapn=0.0;
gdouble Rtrapp=0.0;

gdouble dJdphil_leftl=0.0;
gdouble dJdphil_leftc=0.0;
gdouble dJpdphil_leftl=0.0;
gdouble dJpdphil_leftc=0.0;
gdouble dphil_left=0.0;
gdouble dJdxil_leftc=0.0;
gdouble dJpdxipl_leftc=0.0;

gdouble dJdxic_rightc=0.0;
gdouble dJpdxipc_rightc=0.0;
gdouble dJpdphi_rightc=0.0;
gdouble dJdphi_rightc=0.0;

gdouble Bfree=0.0;
gdouble nceq=0.0;
gdouble pceq=0.0;
gdouble Rfree=0.0;

gdouble nc0_l=0.0;
//gdouble dnc0_l=0.0;
//gdouble pc0_l=0.0;
//gdouble dpc0_l=0.0;

gdouble nc0_r=0.0;
//gdouble dnc0_r=0.0;
//gdouble pc0_r=0.0;
//gdouble dpc0_r=0.0;

gdouble dJnldxil_l=0.0;
gdouble dJnldxil_c=0.0;
gdouble dJnrdxir_c=0.0;
gdouble dJnrdxir_r=0.0;
gdouble dJpldxipl_l=0.0;
gdouble dJpldxipl_c=0.0;
gdouble dJprdxipr_c=0.0;
gdouble dJprdxipr_r=0.0;

gdouble i0=0.0;
gdouble didv=0.0;  //not a function
gdouble diphic=0.0; //could be a function
gdouble didxic=0.0;
gdouble didxipc=0.0;
gdouble didphir=0.0;
gdouble didxir=0.0;
gdouble didxipr=0.0;
gdouble Nad=0.0;

//gdouble dylh_left=0.0;
//gdouble dyrh_left=0.0;
//gdouble dncdphic=0.0;
//gdouble dpcdphic=0.0;

if (in->kl_in_newton==FALSE)
{
	if (in->interfaceleft==TRUE)
	{
		in->phi[z][x][0]=in->Vapplied_l[z][x];
	}
}

if (in->interfaceright==TRUE)
{
		in->phi[z][x][in->ymeshpoints-1]=in->Vr-in->Vapplied_r[z][x];
}

		pos=0;
		for (i=0;i<in->ymeshpoints;i++)
		{
			if (i==0)
			{
//				exl=0.0;
//				Dexl=in->Dex[0];

				phil=(in->Vapplied_l[z][x]/phi0);


				yl=in->ymesh[0]/l0-(in->ymesh[1]-in->ymesh[0])/l0;
//				Tll=in->Tll;
				Tel=in->Tll;
				Thl=in->Tll;




				Ecl= -in->Xi[z][x][0]/phi0-phil;
				Evl= -in->Xi[z][x][0]/phi0-phil-in->Eg[z][x][0]/phi0;
				epl=in->epsilonr[z][x][0];//*epsilon0;


				xnl=in->Fi[z][x][0]/phi0;
				tnl=in->Xi[z][x][0]/phi0;
				one=xnl+tnl;

				nl=get_n_den(in,one*phi0,Tel,in->imat[z][x][i])/n0;
				dnl=get_dn_den(in,one*phi0,Tel,in->imat[z][x][i])*phi0/n0;
				wnl=get_n_w(in,one*phi0,Tel,in->imat[z][x][i]);

				munl=in->mun[z][x][0];


				xpl= -in->Fi[z][x][0]/phi0;
				tpl=(in->Xi[z][x][0]/phi0+in->Eg[z][x][0]/phi0);
				one=xpl-tpl;

				pl=get_p_den(in,one*phi0,Thl,in->imat[z][x][i])/n0;
				dpl=get_dp_den(in,one*phi0,Thl,in->imat[z][x][i])*phi0/n0;
				wpl=get_p_w(in,one*phi0,Thl,in->imat[z][x][i]);


				mupl=in->mup[z][x][0];


//				kll=in->kl[i];

			}else
			{
//				Dexl=in->Dex[i-1];
//				exl=in->ex[i-1];
				phil=in->phi[z][x][i-1]/phi0;
				yl=in->ymesh[i-1]/l0;
//				Tll=in->Tl[z][x][i-1];
				Tel=in->Te[z][x][i-1];
				Thl=in->Th[z][x][i-1];
				Ecl=in->Ec[z][x][i-1]/phi0;
				Evl=in->Ev[z][x][i-1]/phi0;


				nl=in->n[z][x][i-1]/n0;
				dnl=in->dn[z][x][i-1]*phi0/n0;


				wnl=in->wn[z][x][i-1];
				wpl=in->wp[z][x][i-1];

				pl=in->p[z][x][i-1]/n0;
				dpl=in->dp[z][x][i-1]*phi0/n0;
				munl=in->mun[z][x][i-1];
				mupl=in->mup[z][x][i-1];


				epl=in->epsilonr[z][x][i-1];//*epsilon0;


//				kll=in->kl[z][x][i-1];
			}

			Ecc=(-in->Xi[z][x][i]/phi0-in->phi[z][x][i]/phi0);
			Evc=(-in->Xi[z][x][i]/phi0-in->phi[z][x][i]/phi0-in->Eg[z][x][i]/phi0);

			if (i==(in->ymeshpoints-1))
			{

//				Dexr=in->Dex[z][x][i];
//				exr=0.0;
				//phir=in->Vr;

				if (in->invert_applied_bias==FALSE)
				{
					phir=(in->Vr/phi0+in->Vapplied_r[z][x]/phi0);
				}else
				{
					phir=(in->Vr/phi0-in->Vapplied_r[z][x]/phi0);
				}


				yr=in->ymesh[i]/l0+(in->ymesh[i]-in->ymesh[i-1])/l0;
//				Tlr=in->Tlr;
				Ter=in->Tlr;
				Thr=in->Tlr;


				Ecr= -in->Xi[z][x][i]/phi0-phir;
				Evr= -in->Xi[z][x][i]/phi0-phir-in->Eg[z][x][i]/phi0;


				xnr=(in->Vr/phi0+in->Fi[z][x][i]/phi0);
				tnr=(in->Xi[z][x][i]/phi0);

				one=xnr+tnr;

				nr=get_n_den(in,one*phi0,Ter,in->imat[z][x][i])/n0;
				dnr=get_dn_den(in,one*phi0,Ter,in->imat[z][x][i])*phi0/n0;
				wnr=get_n_w(in,one*phi0,Ter,in->imat[z][x][i]);



				xpr= -(in->Vr/phi0+in->Fi[z][x][i]/phi0);
				tpr=(in->Xi[z][x][i]/phi0+in->Eg[z][x][i]/phi0);

				one=xpr-tpr;

				pr=get_p_den(in,one*phi0,Thr,in->imat[z][x][i])/n0;
				dpr=get_dp_den(in,one*phi0,Thr,in->imat[z][x][i])*phi0/n0;
				wpr=get_p_w(in,one*phi0,Thr,in->imat[z][x][i]);

				munr=in->mun[z][x][i];
				mupr=in->mup[z][x][i];

				epr=in->epsilonr[z][x][i];//*epsilon0;
//				klr=in->kl[z][x][i];

			}else
			{

//				Dexr=in->Dex[z][x][i+1];
//				exr=in->ex[z][x][i+1];
				phir=in->phi[z][x][i+1]/phi0;
				yr=in->ymesh[i+1]/l0;
//				Tlr=in->Tl[z][x][i+1];
				Ter=in->Te[z][x][i+1];
				Thr=in->Th[z][x][i+1];

				Ecr=in->Ec[z][x][i+1]/phi0;
				Evr=in->Ev[z][x][i+1]/phi0;


				nr=in->n[z][x][i+1]/n0;
				dnr=in->dn[z][x][i+1]*phi0/n0;

				wnr=in->wn[z][x][i+1];
				wpr=in->wp[z][x][i+1];

				pr=in->p[z][x][i+1]/n0;
				dpr=in->dp[z][x][i+1]*phi0/n0;
				munr=in->mun[z][x][i+1];
				mupr=in->mup[z][x][i+1];

				epr=in->epsilonr[z][x][i+1];//*epsilon0;
//				klr=in->kl[z][x][i+1];

			}

			dJdxipc=0.0;
			dJpdxic=0.0;

			epc=in->epsilonr[z][x][i];//*epsilon0;


//			exc=in->ex[z][x][i];
//			Dexc=in->Dex[z][x][i];
			yc=in->ymesh[i]/l0;
			dyl=yc-yl;
			dyr=yr-yc;
			ddh=(dyl+dyr)/2.0;
			gdouble dylh=dyl/2.0;
			gdouble dyrh=dyr/2.0;

//			dh=(dyl+dyr);
			phic=in->phi[z][x][i]/phi0;
//			Tlc=in->Tl[z][x][i];
//			Tec=in->Te[z][x][i];
//			Thc=in->Th[z][x][i];

				munc=in->mun[z][x][i];
				mupc=in->mup[z][x][i];

				wnc=in->wn[z][x][i];
				wpc=in->wp[z][x][i];

				Dnl=munl*(2.0/3.0)*wnl/Q;
				Dpl=mupl*(2.0/3.0)*wpl/Q;

				Dnc=munc*(2.0/3.0)*wnc/Q;
				Dpc=mupc*(2.0/3.0)*wpc/Q;
				in->Dn[z][x][i]=Dnc;
				in->Dp[z][x][i]=Dnc;


				Dnr=munr*(2.0/3.0)*wnr/Q;
				Dpr=mupr*(2.0/3.0)*wpr/Q;


				Dnl=(Dnl+Dnc)/2.0;
				Dnr=(Dnr+Dnc)/2.0;

				Dpl=(Dpl+Dpc)/2.0;
				Dpr=(Dpr+Dpc)/2.0;

				munl=(munl+munc)/2.0;
				munr=(munr+munc)/2.0;

				mupl=(mupl+mupc)/2.0;
				mupr=(mupr+mupc)/2.0;



				nc=in->n[z][x][i]/n0;
				pc=in->p[z][x][i]/n0;

				dnc=in->dn[z][x][i]*phi0/n0;
				dpc=in->dp[z][x][i]*phi0/n0;
//				dncdphic=in->dndphi[i];
//				dpcdphic=in->dpdphi[i];

				Bfree=in->B[z][x][i]/r_bi0;
				Nad=in->Nad[z][x][i]/n0;
				nceq=in->nfequlib[z][x][i]/n0;
				pceq=in->pfequlib[z][x][i]/n0;
				Rfree=Bfree*(nc*pc-nceq*pceq);
				in->Rfree[z][x][i]=Rfree*r_bi0;

//			klc=in->kl[i];
			nlast=in->nlast[z][x][i]/n0;
			plast=in->plast[z][x][i]/n0;

			for (band=0;band<in->srh_bands;band++)
			{
				in->newton_ntlast[band]=in->ntlast[z][x][i][band];
				in->newton_ptlast[band]=in->ptlast[z][x][i][band];
			}

			dt=in->dt;

//	R=in->R[z][x][i];
	Gn=in->Gn[z][x][i]/r0;
	Gp=in->Gp[z][x][i]/r0;

	e0=(epl+epc)/2.0;
	e1=(epc+epr)/2.0;

//	kl0=(klc+kll)/2.0;
//	kl1=(klr+klc)/2.0;

	dphil= -e0/dyl/ddh;
	dphic=e0/dyl/ddh+e1/dyr/ddh;
	dphir= -e1/dyr/ddh;

	gdouble dphil_d=dphil;
	gdouble dphic_d=dphic;
	gdouble dphir_d=dphir;




	deriv=phil*dphil+phic*dphic+phir*dphir;
	dphidxic=(dnc);	//Q*
	dphidxipc= -(dpc);//Q*

	if (((in->interfaceleft==TRUE)&&(i==0))||((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1)))
	{
		dphidxic=0.0;
		dphidxipc=0.0;
	}

	if (in->ntrapnewton==TRUE)
	{
		for (band=0;band<in->srh_bands;band++)
		{
			in->newton_dphidntrap[band]=Q*(in->dnt[z][x][i][band]);
			if ((in->interfaceleft==TRUE)&&(i==0)) in->newton_dphidntrap[band]=0.0;
			if ((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1)) in->newton_dphidntrap[band]=0.0;
		}
	}

	if (in->ptrapnewton==TRUE)
	{
		for (band=0;band<in->srh_bands;band++)
		{
			in->newton_dphidptrap[band]= -Q*(in->dpt[z][x][i][band]);
			if ((in->interfaceleft==TRUE)&&(i==0)) in->newton_dphidptrap[band]=0.0;
			if ((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1)) in->newton_dphidptrap[band]=0.0;

			//dphidxipc+= -Q*(in->dpt[i]);
		}
	}




//	G=in->G[i];



			xil=Q*2.0*(3.0/2.0)*phi0*(Ecc-Ecl)/((wnc+wnl));
			xir=Q*2.0*(3.0/2.0)*phi0*(Ecr-Ecc)/((wnr+wnc));

			//gdouble dxil= -Q*2.0*(3.0/2.0)*(Ecc-Ecl)/pow((wnc+wnl),2.0);
			//gdouble dxir= -Q*2.0*(3.0/2.0)*(Ecr-Ecc)/pow((wnr+wnc),2.0);

			xipl=Q*2.0*(3.0/2.0)*phi0*(Evc-Evl)/(wpc+wpl);
			xipr=Q*2.0*(3.0/2.0)*phi0*(Evr-Evc)/(wpr+wpc);

			dJdxil=0.0;
			dJdxic=0.0;
			dJdxir=0.0;

			dJpdxipl=0.0;
			dJpdxipc=0.0;
			dJpdxipr=0.0;


			dJdphil=0.0;
			dJdphic=0.0;
			dJdphir=0.0;


			dJpdphil=0.0;
			dJpdphic=0.0;
			dJpdphir=0.0;


			Jnl=(Dnl/D0/dyl)*(B(-xil)*nc-B(xil)*nl);
			dJnldxil_l= -(Dnl/D0/dyl)*(B(xil)*dnl);
			dJnldxil_c=(Dnl/D0/dyl)*B(-xil)*dnc;

			gdouble dJnldphi_l= -phi0*(munl/D0/dyl)*(dB(-xil)*nc+dB(xil)*nl);
			gdouble dJnldphi_c=phi0*(munl/D0/dyl)*(dB(-xil)*nc+dB(xil)*nl);

			Jnr=(Dnr/D0/dyr)*(B(-xir)*nr-B(xir)*nc);
			dJnrdxir_c= -(Dnr/D0/dyr)*(B(xir)*dnc);
			dJnrdxir_r=(Dnr/D0/dyr)*(B(-xir)*dnr);

			gdouble dJnrdphi_c=phi0*(munr/D0/dyr)*(-dB(-xir)*nr-dB(xir)*nc);
			gdouble dJnrdphi_r=phi0*(munr/D0/dyr)*(dB(-xir)*nr+dB(xir)*nc);

			Jpl=(Dpl/D0/dyl)*(B(-xipl)*pl-B(xipl)*pc);
			dJpldxipl_l=(Dpl/D0/dyl)*(B(-xipl)*dpl);
			dJpldxipl_c= -(Dpl/D0/dyl)*B(xipl)*dpc;

			gdouble dJpldphi_l= -phi0*((mupl/D0)/dyl)*(dB(-xipl)*pl+dB(xipl)*pc);
			gdouble dJpldphi_c=phi0*((mupl/D0)/dyl)*(dB(-xipl)*pl+dB(xipl)*pc);

			Jpr=(Dpr/D0/dyr)*(B(-xipr)*pc-B(xipr)*pr);
			dJprdxipr_c=(Dpr/D0/dyr)*(B(-xipr)*dpc);
			dJprdxipr_r= -(Dpr/D0/dyr)*(B(xipr)*dpr);

			gdouble dJprdphi_c= -phi0*(mupr/D0/dyr)*(dB(-xipr)*pc+dB(xipr)*pr);
			gdouble dJprdphi_r=phi0*(mupr/D0/dyr)*(dB(-xipr)*pc+dB(xipr)*pr);


			if (i==0)
			{
				in->Jnleft[z][x]=Jnl*D0*n0/l0;
				in->Jpleft[z][x]=Jpl*D0*n0/l0;
			}

			if (i==in->ymeshpoints-1)
			{
				in->Jnright[z][x]=Jnr*D0*n0/l0;
				in->Jpright[z][x]=Jpr*D0*n0/l0;
			}


			in->Jn[z][x][i]=Q*(Jnl+Jnr)*D0*n0/l0/2.0;
			in->Jp[z][x][i]=Q*(Jpl+Jpr)*D0*n0/l0/2.0;

			dJdxil+= -dJnldxil_l/(dylh+dyrh);
			dJdxic+=(-dJnldxil_c+dJnrdxir_c)/(dylh+dyrh);
			dJdxir+=dJnrdxir_r/(dylh+dyrh);

			dJpdxipl+= -dJpldxipl_l/(dylh+dyrh);
			dJpdxipc+=(-dJpldxipl_c+dJprdxipr_c)/(dylh+dyrh);
			dJpdxipr+=dJprdxipr_r/(dylh+dyrh);


			dJdphil+= -dJnldphi_l/(dylh+dyrh);
			dJdphic+=(-dJnldphi_c+dJnrdphi_c)/(dylh+dyrh);
			dJdphir+=dJnrdphi_r/(dylh+dyrh);


			dJpdphil+= -dJpldphi_l/(dylh+dyrh);
			dJpdphic+=(-dJpldphi_c+dJprdphi_c)/(dylh+dyrh);
			dJpdphir+=dJprdphi_r/(dylh+dyrh);

			if (Bfree!=0.0)
			{
				dJdxic+= -Bfree*(dnc*pc);
				dJdxipc+= -Bfree*(nc*dpc);

				dJpdxipc+=Bfree*(nc*dpc);
				dJpdxic+=Bfree*(dnc*pc);

				if ((in->interfaceleft==TRUE)&&(i==0))
				{
				}else
				if ((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1))
				{
				}else
				{
					dJdphic+= -Bfree*(dnc*pc);
					dJpdphic+=Bfree*(nc*dpc);
				}

			}

			if (i==0)
			{

				dJdphil_leftl=dJnldphi_l;
				dJdphil_leftc=dJnldphi_c;
				dJpdphil_leftl=dJpldphi_l;
				dJpdphil_leftc=dJpldphi_c;
				dphil_left= -e0/dyl/ddh;
				dJdxil_leftc=dJnldxil_c;
				dJpdxipl_leftc=dJpldxipl_c;
				//dylh_left=dylh;
				//dyrh_left=dyrh;

			}

			if (i==in->ymeshpoints-1)
			{
				dJdxic_rightc=dJnrdxir_c;
				dJpdxipc_rightc=dJprdxipr_c;
				dJdphi_rightc= -dJnrdphi_r;
				dJpdphi_rightc=dJprdphi_c;
			}

			if (in->go_time==TRUE)
			{
				dJdxic+= -dnc/dt;
			}

			if (in->go_time==TRUE)
			{
				dJpdxipc+=dpc/dt;
			}



			Rtrapn=0.0;
			Rtrapp=0.0;



			in->nrelax[z][x][i]=0.0;
			in->ntrap_to_p[z][x][i]=0.0;
			in->prelax[z][x][i]=0.0;
			in->ptrap_to_n[z][x][i]=0.0;


			if (in->ntrapnewton==TRUE)
			{
				for (band=0;band<in->srh_bands;band++)
				{
					in->newton_dJdtrapn[band]=0.0;
					in->newton_dJpdtrapn[band]=0.0;
					in->newton_dntrap[band]=nc*in->srh_n_r1[z][x][i][band]-in->srh_n_r2[z][x][i][band]-pc*in->srh_n_r3[z][x][i][band]+in->srh_n_r4[z][x][i][band];
					in->newton_dntrapdntrap[band]=nc*in->dsrh_n_r1[z][x][i][band]-in->dsrh_n_r2[z][x][i][band]-pc*in->dsrh_n_r3[z][x][i][band]+in->dsrh_n_r4[z][x][i][band];
					in->newton_dntrapdn[band]=dnc*in->srh_n_r1[z][x][i][band];
					in->newton_dntrapdp[band]= -dpc*in->srh_n_r3[z][x][i][band];
					Rtrapn+=nc*in->srh_n_r1[z][x][i][band]-in->srh_n_r2[z][x][i][band];
					dJdxic-=dnc*in->srh_n_r1[z][x][i][band];
					in->newton_dJdtrapn[band]-=nc*in->dsrh_n_r1[z][x][i][band]-in->dsrh_n_r2[z][x][i][band];
					Rtrapp+= -(-pc*in->srh_n_r3[z][x][i][band]+in->srh_n_r4[z][x][i][band]);
					dJpdxipc+= -(-dpc*in->srh_n_r3[z][x][i][band]);
					in->newton_dJpdtrapn[band]= -(-pc*in->dsrh_n_r3[z][x][i][band]+in->dsrh_n_r4[z][x][i][band]);

					if (in->go_time==TRUE)
					{
						in->newton_dntrap[band]+= -(in->nt[z][x][i][band]-in->newton_ntlast[band])/dt;
						in->newton_dntrapdntrap[band]+= -(in->dnt[z][x][i][band])/dt;
					}

					in->nrelax[z][x][i]+=nc*in->srh_n_r1[z][x][i][band]-in->srh_n_r2[z][x][i][band];
					in->ntrap_to_p[z][x][i]+= -(-pc*in->srh_n_r3[z][x][i][band]+in->srh_n_r4[z][x][i][band]);

					in->nt_r1[z][x][i][band]=nc*in->srh_n_r1[z][x][i][band];
					in->nt_r2[z][x][i][band]=in->srh_n_r2[z][x][i][band];
					in->nt_r3[z][x][i][band]=pc*in->srh_n_r3[z][x][i][band];
					in->nt_r4[z][x][i][band]=in->srh_n_r4[z][x][i][band];

				}
			}

			//band=0;

			if (in->ptrapnewton==TRUE)
			{

				for (band=0;band<in->srh_bands;band++)
				{
					//dJdtrapn[band]=0.0;
					in->newton_dJpdtrapp[band]=0.0;
					in->newton_dJdtrapp[band]=0.0;
					in->newton_dptrap[band]=pc*in->srh_p_r1[z][x][i][band]-in->srh_p_r2[z][x][i][band]-nc*in->srh_p_r3[z][x][i][band]+in->srh_p_r4[z][x][i][band];
					in->newton_dptrapdptrap[band]=pc*in->dsrh_p_r1[z][x][i][band]-in->dsrh_p_r2[z][x][i][band]-nc*in->dsrh_p_r3[z][x][i][band]+in->dsrh_p_r4[z][x][i][band];
					in->newton_dptrapdp[band]=dpc*in->srh_p_r1[z][x][i][band];
					in->newton_dptrapdn[band]= -dnc*in->srh_p_r3[z][x][i][band];

					Rtrapp+=pc*in->srh_p_r1[z][x][i][band]-in->srh_p_r2[z][x][i][band];
					dJpdxipc+=dpc*in->srh_p_r1[z][x][i][band];
					in->newton_dJpdtrapp[band]+=pc*in->dsrh_p_r1[z][x][i][band]-in->dsrh_p_r2[z][x][i][band];

					Rtrapn+= -(-nc*in->srh_p_r3[z][x][i][band]+in->srh_p_r4[z][x][i][band]);
					dJdxic-= -(-dnc*in->srh_p_r3[z][x][i][band]);
					in->newton_dJdtrapp[band]-= -(-nc*in->dsrh_p_r3[z][x][i][band]+in->dsrh_p_r4[z][x][i][band]);

					if (in->go_time==TRUE)
					{
						in->newton_dptrap[band]+= -(in->pt[z][x][i][band]-in->newton_ptlast[band])/dt;
						in->newton_dptrapdptrap[band]+= -(in->dpt[z][x][i][band])/dt;
					}

					in->prelax[z][x][i]+=pc*in->srh_p_r1[z][x][i][band]-in->srh_p_r2[z][x][i][band];
					in->ptrap_to_n[z][x][i]+= -(-nc*in->srh_p_r3[z][x][i][band]+in->srh_p_r4[z][x][i][band]);

					in->pt_r1[z][x][i][band]=pc*in->srh_p_r1[z][x][i][band];
					in->pt_r2[z][x][i][band]=in->srh_p_r2[z][x][i][band];
					in->pt_r3[z][x][i][band]=nc*in->srh_p_r3[z][x][i][band];
					in->pt_r4[z][x][i][band]=in->srh_p_r4[z][x][i][band];

				}

			}

			//band=0;



			in->Rn[z][x][i]=Rtrapn+Rfree;
			in->Rp[z][x][i]=Rtrapp+Rfree;

			in->Rn_srh[z][x][i]=Rtrapn;
			in->Rp_srh[z][x][i]=Rtrapp;

			//Rtrapp=1e24;
			//Rtrapn=1e24;




			if (i!=0)
			{
				in->Ti[pos]=i;
				in->Tj[pos]=i-1;
				in->Tx[pos]=dphil_d;
				pos++;
				//electron
					in->Ti[pos]=in->ymeshpoints*(1)+i;
					in->Tj[pos]=in->ymeshpoints*(1)+i-1;
					in->Tx[pos]=dJdxil;
					pos++;

					in->Ti[pos]=in->ymeshpoints*(1)+i;
					in->Tj[pos]=i-1;
					in->Tx[pos]=dJdphil;
					pos++;

					//hole
					in->Ti[pos]=in->ymeshpoints*(1+1)+i;
					in->Tj[pos]=in->ymeshpoints*(1+1)+i-1;
					in->Tx[pos]=dJpdxipl;
					pos++;

					in->Ti[pos]=i+in->ymeshpoints*(1+1);
					in->Tj[pos]=i-1;
					in->Tx[pos]=dJpdphil;
					pos++;

			}


			if ((in->kl_in_newton==TRUE)&&(in->interfaceleft==TRUE)&&(i==0))
			{
				//getchar();
			}else
			{
				//phi
				in->Ti[pos]=i;
				in->Tj[pos]=i;
				in->Tx[pos]=dphic_d;
				pos++;
			}


			in->Ti[pos]=i;
			in->Tj[pos]=i+in->ymeshpoints*(1);
			in->Tx[pos]=dphidxic;
			//strcpy(in->Tdebug[pos],"dphidxic");
			pos++;

			in->Ti[pos]=i;
			in->Tj[pos]=i+in->ymeshpoints*(1+1);
			in->Tx[pos]=dphidxipc;
			//strcpy(in->Tdebug[pos],"dphidxipc");
			pos++;


			//electron

			in->Ti[pos]=in->ymeshpoints*(1)+i;
			in->Tj[pos]=in->ymeshpoints*(1)+i;
			in->Tx[pos]=dJdxic;
			//strcpy(in->Tdebug[pos],"dJdxic");
			pos++;

			in->Ti[pos]=in->ymeshpoints*(1)+i;
			in->Tj[pos]=in->ymeshpoints*(1+1)+i;
			in->Tx[pos]=dJdxipc;
			//strcpy(in->Tdebug[pos],"dJdxipc");
			pos++;

			in->Ti[pos]=in->ymeshpoints*(1)+i;
			in->Tj[pos]=i;
			in->Tx[pos]=dJdphic;
			//strcpy(in->Tdebug[pos],"dJdphic");
			pos++;





			//hole
			in->Ti[pos]=in->ymeshpoints*(1+1)+i;
			in->Tj[pos]=in->ymeshpoints*(1+1)+i;
			in->Tx[pos]=dJpdxipc;
			pos++;

			in->Ti[pos]=in->ymeshpoints*(1+1)+i;
			in->Tj[pos]=in->ymeshpoints*(1)+i;
			in->Tx[pos]=dJpdxic;
			pos++;

			in->Ti[pos]=in->ymeshpoints*(1+1)+i;
			in->Tj[pos]=i;
			in->Tx[pos]=dJpdphic;
			pos++;



			if (in->ntrapnewton==TRUE)
			{
				for (band=0;band<in->srh_bands;band++)
				{
					in->Ti[pos]=in->ymeshpoints*(1+1+1+band)+i;
					in->Tj[pos]=in->ymeshpoints*(1+1+1+band)+i;
					in->Tx[pos]=in->newton_dntrapdntrap[band];
					pos++;

					in->Ti[pos]=in->ymeshpoints*(1+1+1+band)+i;
					in->Tj[pos]=in->ymeshpoints*1+i;
					in->Tx[pos]=in->newton_dntrapdn[band];
					pos++;

					in->Ti[pos]=in->ymeshpoints*(1+1+1+band)+i;
					in->Tj[pos]=in->ymeshpoints*(1+1)+i;
					in->Tx[pos]=in->newton_dntrapdp[band];
					pos++;

					in->Ti[pos]=in->ymeshpoints*(1)+i;
					in->Tj[pos]=in->ymeshpoints*(1+1+1+band)+i;
					in->Tx[pos]=in->newton_dJdtrapn[band];
					pos++;

					in->Ti[pos]=in->ymeshpoints*(1+1)+i;
					in->Tj[pos]=in->ymeshpoints*(1+1+1+band)+i;
					in->Tx[pos]=in->newton_dJpdtrapn[band];
					pos++;

					in->Ti[pos]=i;
					in->Tj[pos]=in->ymeshpoints*(1+1+1+band)+i;
					in->Tx[pos]=in->newton_dphidntrap[band];
					pos++;


				}


			}

			if (in->ptrapnewton==TRUE)
			{
				for (band=0;band<in->srh_bands;band++)
				{
					in->Ti[pos]=in->ymeshpoints*(1+1+1+in->srh_bands+band)+i;
					in->Tj[pos]=in->ymeshpoints*(1+1+1+in->srh_bands+band)+i;
					in->Tx[pos]=in->newton_dptrapdptrap[band];
					pos++;

					in->Ti[pos]=in->ymeshpoints*(1+1+1+in->srh_bands+band)+i;
					in->Tj[pos]=in->ymeshpoints*(1+1)+i;
					in->Tx[pos]=in->newton_dptrapdp[band];
					pos++;

					in->Ti[pos]=in->ymeshpoints*(1+1+1+in->srh_bands+band)+i;
					in->Tj[pos]=in->ymeshpoints*(1)+i;
					in->Tx[pos]=in->newton_dptrapdn[band];
					pos++;

					in->Ti[pos]=in->ymeshpoints*(1+1)+i;
					in->Tj[pos]=in->ymeshpoints*(1+1+1+in->srh_bands+band)+i;
					in->Tx[pos]=in->newton_dJpdtrapp[band];
					pos++;

					in->Ti[pos]=in->ymeshpoints*(1)+i;
					in->Tj[pos]=in->ymeshpoints*(1+1+1+in->srh_bands+band)+i;
					in->Tx[pos]=in->newton_dJdtrapp[band];
					pos++;

					in->Ti[pos]=i;
					in->Tj[pos]=in->ymeshpoints*(1+1+1+in->srh_bands+band)+i;
					in->Tx[pos]=in->newton_dphidptrap[band];
					pos++;
				}

			}

			if (i!=(in->ymeshpoints-1))
			{

				if ((in->kl_in_newton==TRUE)&&(in->interfaceleft==TRUE)&&(i==0))
				{
					//getchar();
				}else
				{
					//phi
					in->Ti[pos]=i;
					in->Tj[pos]=i+1;
					in->Tx[pos]=dphir_d;
					pos++;
				}



				//electron
				in->Ti[pos]=in->ymeshpoints*(1)+i;
				in->Tj[pos]=in->ymeshpoints*(1)+i+1;
				in->Tx[pos]=dJdxir;
				pos++;

				in->Ti[pos]=i+in->ymeshpoints*(1);
				in->Tj[pos]=i+1;
				in->Tx[pos]=dJdphir;
				pos++;

				//hole
				in->Ti[pos]=in->ymeshpoints*(1+1)+i;
				in->Tj[pos]=in->ymeshpoints*(1+1)+i+1;
				in->Tx[pos]=dJpdxipr;
				pos++;

				in->Ti[pos]=i+in->ymeshpoints*(1+1);
				in->Tj[pos]=i+1;
				in->Tx[pos]=dJpdphir;
				pos++;




			}

			//Possion
			gdouble build=0.0;
			if ((in->interfaceleft==TRUE)&&(i==0))
			{
				build= -0.0;
			}else
			if ((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1))
			{
				build= -0.0;
			}else
			{
				build= -(deriv);

				build+= -(-(pc-nc+Nad));

				for (band=0;band<in->srh_bands;band++)
				{
					build+= -(-Q*(in->pt[z][x][i][band]-in->nt[z][x][i][band]));
				}

				//build+= -(-Q*in->Nad[i]);

			}
			in->b[i]=build;

			build=0.0;
			build= -((Jnr-Jnl)/(dylh+dyrh)-Rtrapn-Rfree);

			if (in->go_time==TRUE)
			{
				build-= -(nc-nlast)/dt;
			}

			//getchar();
			build-=Gn;
			in->b[in->ymeshpoints*(1)+i]=build;

			//hole
			build=0.0;
			build= -((Jpr-Jpl)/(dylh+dyrh)+Rtrapp+Rfree);

			build-= -Gp;

			if (in->go_time==TRUE)
			{
				build-=(pc-plast)/dt;
			}

			in->b[in->ymeshpoints*(1+1)+i]=build;

			if (in->ntrapnewton==TRUE)
			{
				for (band=0;band<in->srh_bands;band++)
				{
					in->b[in->ymeshpoints*(1+1+1+band)+i]= -(in->newton_dntrap[band]);
				}
			}

			if (in->ptrapnewton==TRUE)
			{
				for (band=0;band<in->srh_bands;band++)
				{
					in->b[in->ymeshpoints*(1+1+1+in->srh_bands+band)+i]= -(in->newton_dptrap[band]);
				}

			}

		}


if (pos>in->N)
{
	ewe(sim,"Error %d %d %d\n",pos,in->N,in->kl_in_newton);
}

//solver_dump_matrix_comments(in->M,pos,in->Tdebug,in->Ti,in->Tj, in->Tx,in->b,"");
//getchar();
//fclose(file_j);
//getchar();

}

gdouble get_cur_error(struct simulation *sim, struct device *in)
{
int i;
gdouble phi=0.0;
gdouble n=0.0;
gdouble p=0.0;
gdouble x=0.0;
gdouble te=0.0;
gdouble th=0.0;
gdouble tl=0.0;
gdouble ttn=0.0;
gdouble ttp=0.0;
gdouble i0=0.0;
int band=0;
for (i=0;i<in->ymeshpoints;i++)
{
		if ((in->interfaceleft==TRUE)&&(i==0))
		{
		}else
		if ((in->interfaceright==TRUE)&&(i==in->ymeshpoints-1))
		{
		}else
		{
			phi+=gfabs(in->b[i]);
		}

		n+=gfabs(in->b[in->ymeshpoints*(1)+i]);
		p+=+gfabs(in->b[in->ymeshpoints*(1+1)+i]);

		if (in->ntrapnewton==TRUE)
		{
			for (band=0;band<in->srh_bands;band++)
			{
				ttn+=gfabs(in->b[in->ymeshpoints*(1+1+1+band)+i]);
			}
		}

		if (in->ptrapnewton==TRUE)
		{
			for (band=0;band<in->srh_bands;band++)
			{
				ttp+=gfabs(in->b[in->ymeshpoints*(1+1+1+in->srh_bands+band)+i]);
			}
		}


}
gdouble tot=phi+n+p+x+te+th+tl+ttn+ttp+i0;
if (isnan( tot))
{
	printf_log(sim,"%Le %Le %Le %Le %Le %Le %Le %Le %Le\n",phi,n,p,x,te,th,tl,ttn,ttp);
	//dump_matrix(sim,in->M,in->N,in->Ti,in->Tj, in->Tx,in->b,"");

	ewe(sim,"nan detected in newton solver\n");
}

return tot;
}

gdouble get_abs_error(struct device *in)
{
int i;
gdouble tot=0.0;

for (i=0;i<in->M;i++)
{
	tot+=gfabs(in->b[i]);
}

return tot;
}
void solver_cal_memory(struct device *in,int *ret_N,int *ret_M,int dim)
{
int i=0;
int N=0;
int M=0;
N=in->ymeshpoints*3-2;	//Possion main

N+=in->ymeshpoints*3-2;	//Je main
N+=in->ymeshpoints*3-2;	//Jh main

N+=in->ymeshpoints*3-2;	//dJe/phi
N+=in->ymeshpoints*3-2;	//dJh/phi

N+=in->ymeshpoints;		//dphi/dn
N+=in->ymeshpoints;		//dphi/dh

N+=in->ymeshpoints;		//dJndp
N+=in->ymeshpoints;		//dJpdn

M=in->ymeshpoints;	//Pos

M+=in->ymeshpoints;		//Je
M+=in->ymeshpoints;		//Jh

if (in->ntrapnewton==TRUE)
{
	for (i=0;i<in->srh_bands;i++)
	{
		N+=in->ymeshpoints;		//dntrapdn
		N+=in->ymeshpoints;		//dntrapdntrap
		N+=in->ymeshpoints;		//dntrapdp
		N+=in->ymeshpoints;		//dJndtrapn
		N+=in->ymeshpoints;		//dJpdtrapn
		N+=in->ymeshpoints;		//dphidntrap

		M+=in->ymeshpoints;		//nt
	}

}

if (in->ptrapnewton==TRUE)
{
	for (i=0;i<in->srh_bands;i++)
	{
		N+=in->ymeshpoints;		//dptrapdp
		N+=in->ymeshpoints;		//dptrapdptrap
		N+=in->ymeshpoints;		//dptrapdn
		N+=in->ymeshpoints;		//dJpdtrapp
		N+=in->ymeshpoints;		//dJdtrapp
		N+=in->ymeshpoints;		//dphidptrap

		M+=in->ymeshpoints;		//pt
	}
}
*ret_N=N;
*ret_M=M;
}


void dllinternal_solver_free_memory(struct device *in)
{
if (in->srh_bands>0)
{
	free(in->newton_dntrap);
	free(in->newton_dntrapdntrap);
	free(in->newton_dntrapdn);
	free(in->newton_dntrapdp);
	free(in->newton_dJdtrapn);
	free(in->newton_dJpdtrapn);
	free(in->newton_dphidntrap);
	free(in->newton_dptrapdp);
	free(in->newton_dptrapdptrap);
	free(in->newton_dptrap);
	free(in->newton_dptrapdn);
	free(in->newton_dJpdtrapp);
	free(in->newton_dJdtrapp);
	free(in->newton_dphidptrap);

	free(in->newton_ntlast);
	free(in->newton_ptlast);
}

free(in->Ti);
free(in->Tj);
free(in->Tx);
free(in->b);

//for (i=0;i<in->N;i++)
//{
//	free(in->Tdebug[i]);
//}
//free(in->Tdebug);

in->newton_dntrapdntrap=NULL;
in->newton_dntrap=NULL;
in->newton_dntrapdn=NULL;
in->newton_dntrapdp=NULL;
in->newton_dJdtrapn=NULL;
in->newton_dJpdtrapn=NULL;
in->newton_dphidntrap=NULL;
in->newton_dptrapdp=NULL;
in->newton_dptrapdptrap=NULL;
in->newton_dptrap=NULL;
in->newton_dptrapdn=NULL;
in->newton_dJpdtrapp=NULL;
in->newton_dJdtrapp=NULL;
in->newton_dphidptrap=NULL;


in->newton_ntlast=NULL;
in->newton_ptlast=NULL;

in->Ti=NULL;
in->Tj=NULL;
in->Tx=NULL;
in->b=NULL;
in->Tdebug=NULL;

}

int dllinternal_solve_cur(struct simulation *sim,struct device *in,int z, int x)
{
T0=sim->T0;//300.0;
D0=sim->D0;//243.75;
n0=sim->n0;//1e20;
phi0=((kb/Q)*T0);
l0=sqrtl(epsilon0*(kb/Q)*T0/(Q*n0));
r0=n0*D0/l0/l0;
r_bi0=D0/(n0*l0*l0);

	
gdouble last_J=get_J(in);
gdouble delta_J=0.0;

gdouble error=0.0;
int ittr=0;
if (get_dump_status(sim,dump_print_newtonerror)==TRUE)
{
	printf_log(sim,"Solve cur\n");
}





#ifdef only
only_update_thermal=FALSE;
#endif
//in->enable_back=FALSE;
int stop=FALSE;
int thermalrun=0;
gdouble check[10];
int cpos=0;
char temp[PATHLEN];

gdouble abs_error=0.0;
	do
	{

		fill_matrix(sim,in,z,x);
		abs_error=get_abs_error(in);

		plot_now(sim,in,"plot");
	//solver_dump_matrix(in->M,in->N,in->Ti,in->Tj, in->Tx,in->b);
//getchar();

			if (in->stop==TRUE)
			{
				break;
			}

			solver(sim,in->M,in->N,in->Ti,in->Tj, in->Tx,in->b);

			update_solver_vars(sim,in,z,x,TRUE);

			//solver_dump_matrix(in->M,in->N,in->Ti,in->Tj, in->Tx,in->b);

			//getchar();
		delta_J=fabs(last_J-get_J(in));
		last_J=get_J(in);
		error=get_cur_error(sim,in);

		//thermalrun++;
		if (thermalrun==40) thermalrun=0;
		//update(in);
//getchar();

		if (get_dump_status(sim,dump_print_newtonerror)==TRUE)
		{
			printf_log(sim,"%d error = %Le  abs_error = %Le dJ=%Le %Le I=%Le",ittr,error,abs_error,delta_J,contact_get_active_contact_voltage(sim,in),get_J(in));

			printf_log(sim,"\n");
		}
		error=delta_J;
		in->last_error=error;
		in->last_ittr=ittr;
		ittr++;

		if (get_dump_status(sim,dump_write_converge)==TRUE)
		{
			sim->converge=fopena(get_output_path(sim),"converge.dat","a");
			fprintf(sim->converge,"%Le\n",error);
			fclose(sim->converge);
		}

		stop=TRUE;

		if (ittr<in->max_electrical_itt)
		{
			if (error>in->min_cur_error)
			{
				stop=FALSE;

			}
		}

		if (ittr<in->newton_min_itt)
		{
			stop=FALSE;
		}

		if (in->newton_clever_exit==TRUE)
		{
			check[cpos]=error;
			cpos++;

			if (cpos>10)
			{
				cpos=0;
			}

			if (ittr>=in->newton_min_itt)
			{
					if ((check[0]<error)||(check[1]<error))
					{
						stop=TRUE;
					}
			}
		}



	}while(stop==FALSE);

in->newton_last_ittr=ittr;

//if (error>1e-3)
//{
//	printf_log(sim,"warning: The solver has not converged very well.\n");
//}

//getchar();

if (get_dump_status(sim,dump_newton)==TRUE)
{
	join_path(2,temp,get_output_path(sim),"solver");
	dump_1d_slice(sim,in,temp);
}

//plot_now(in,"plot");
//getchar();
in->odes+=in->M;
//getchar();
return 0;
}

void dllinternal_solver_realloc(struct simulation *sim,struct device *in, int dim)
{
int N=0;
int M=0;
long double* dtemp=NULL;
int *itemp=NULL;

solver_cal_memory(in,&N,&M,dim);


int alloc=FALSE;
if ((in->N==0)||(in->M==0))
{
	in->N=N;
	in->M=M;
	alloc=TRUE;
}else
if ((N!=in->N)||(M!=in->M))
{
	in->N=N;
	in->M=M;
	alloc=TRUE;
}


if (alloc==TRUE)
{

	itemp =realloc(in->Ti,in->N*sizeof(int));
	if (itemp==NULL)
	{
		ewe(sim,"memory error\n");
	}else
	{
		in->Ti=itemp;
	}

	itemp =realloc(in->Tj,in->N*sizeof(int));
	if (itemp==NULL)
	{
		ewe(sim,"memory error\n");
	}else
	{
		in->Tj=itemp;
	}


	dtemp = realloc(in->Tx,in->N*sizeof(long double));
	if (dtemp==NULL)
	{
		ewe(sim,"memory error\n");
	}else
	{
		in->Tx=dtemp;
	}


	//int i=0;
	//in->Tdebug = (char**)malloc(in->N*sizeof(char*));

	//for (i=0;i<in->N;i++)
	//{
	//	in->Tdebug[i]= (char*)malloc(20*sizeof(char));
	//	strcpy(in->Tdebug[i],"");
	//}


	dtemp = realloc(in->b,in->M*sizeof(long double));
	if (dtemp==NULL)
	{
		ewe(sim,"memory error\n");
	}else
	{
		in->b=dtemp;
	}


	if (in->srh_bands>0)
	{
		dtemp=realloc(in->newton_dntrap,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dntrap=dtemp;
		}


		dtemp=realloc(in->newton_dntrapdntrap,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dntrapdntrap=dtemp;
		}

		dtemp=realloc(in->newton_dntrapdn,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dntrapdn=dtemp;
		}

		dtemp=realloc(in->newton_dntrapdp,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dntrapdp=dtemp;
		}

		dtemp=realloc(in->newton_dJdtrapn,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dJdtrapn=dtemp;
		}

		dtemp=realloc(in->newton_dJpdtrapn,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dJpdtrapn=dtemp;
		}

		dtemp=realloc(in->newton_dphidntrap,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dphidntrap=dtemp;
		}


		dtemp=realloc(in->newton_ntlast,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_ntlast=dtemp;
		}



		dtemp=realloc(in->newton_dptrapdp,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dptrapdp=dtemp;
		}

		dtemp=realloc(in->newton_dptrapdptrap,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dptrapdptrap=dtemp;
		}

		dtemp=realloc(in->newton_dptrap,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dptrap=dtemp;
		}

		dtemp=realloc(in->newton_dptrapdn,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dptrapdn=dtemp;
		}

		dtemp=realloc(in->newton_dJpdtrapp,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dJpdtrapp=dtemp;
		}


		dtemp=realloc(in->newton_dJdtrapp,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dJdtrapp=dtemp;
		}

		dtemp=realloc(in->newton_dphidptrap,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_dphidptrap=dtemp;
		}

		dtemp=realloc(in->newton_ptlast,in->srh_bands*sizeof(gdouble));
		if (dtemp==NULL)
		{
			ewe(sim,"memory error\n");
		}else
		{
			in->newton_ptlast=dtemp;
		}
	}

}

}
