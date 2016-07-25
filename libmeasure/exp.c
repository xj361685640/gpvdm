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


#include <exp.h>
#include "sim.h"
#include "i.h"
#include "buffer.h"
#include "contacts.h"

static gdouble n_count=0.0;
static gdouble p_count=0.0;
static gdouble rn_count=0.0;
static gdouble rp_count=0.0;

void get_avg_np_pos(struct device *in,gdouble *nx,gdouble *px)
{
int x;
int y;
int z;

gdouble navg=0.0;
gdouble pavg=0.0;
gdouble nsum=0.0;
gdouble psum=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (z=0;z<in->zmeshpoints;z++)
		{
			navg+=(in->n[z][x][y]+in->nt_all[z][x][y]-in->n_orig[z][x][y])*in->ymesh[y];
			pavg+=(in->p[z][x][y]+in->pt_all[z][x][y]-in->p_orig[z][x][y])*in->ymesh[y];
			nsum+=(in->n[z][x][y]+in->nt_all[z][x][y]-in->n_orig[z][x][y]);
			psum+=in->p[z][x][y]+in->pt_all[z][x][y]-in->p_orig[z][x][y];
		}
	}
}

if (nsum!=0.0)
{
	*nx=navg/nsum;
}else
{
	*nx=0.0;
}

if (psum!=0.0)
{
*px=pavg/psum;
}else
{
*px=0.0;
}



}

gdouble get_charge_change(struct device *in)
{

gdouble diff=0.0;
gdouble n=0.0;
gdouble p=0.0;

int x=0;
int y=0;
int z=0;

int band=0;
if (in->go_time==TRUE)
{

	for (z=0;z<in->zmeshpoints;z++)
	{
		for (x=0;x<in->xmeshpoints;x++)
		{
			for (y=0;y<in->ymeshpoints;y++)
			{
				n=0.0;
				p=0.0;

				for (band=0;band<in->srh_bands;band++)
				{
					n+=in->ntlast[z][x][y][band];
					p+=in->ptlast[z][x][y][band];
				}

				n+=in->nlast[z][x][y];
				p+=in->plast[z][x][y];

				diff+=fabs(in->p[z][x][y]+in->pt_all[z][x][y]-in->n[z][x][y]-in->nt_all[z][x][y]+n-p);
			}
		}
	}
	//printf("%le\n",diff);
	diff/=(n+p);
	diff*=100.0;
}

return diff;
}



gdouble get_avg_recom(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble sum=0.0;
gdouble add=0.0;
	for (z=0;z<in->zmeshpoints;z++)
	{
		for (x=0;x<in->xmeshpoints;x++)
		{
			for (y=0;y<in->ymeshpoints;y++)
			{
				add=in->ptrap_to_n[z][x][y]+in->ntrap_to_p[z][x][y];

				if ((in->interfaceleft==TRUE)&&(y==0))
				{
					add/=2.0;
				}

				if ((in->interfaceright==TRUE)&&(y==in->ymeshpoints-1))
				{
					add/=2.0;
				}

				sum+=add;
			}
		}
	}

return sum/(((gdouble)(in->ymeshpoints*in->xmeshpoints*in->zmeshpoints)));
}

gdouble get_avg_relax_n(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Rtot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{

		for (y=0;y<in->ymeshpoints;y++)
		{
			Rtot+=(in->nrelax[z][x][y]);
		}
	}
}
return Rtot/(((gdouble)(in->zmeshpoints*in->xmeshpoints*in->ymeshpoints)));
}

gdouble get_avg_relax_p(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Rtot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{

		for (y=0;y<in->ymeshpoints;y++)
		{
			Rtot+=(in->prelax[z][x][y]);
		}
	}
}

return Rtot/(((gdouble)(in->zmeshpoints*in->xmeshpoints*in->ymeshpoints)));
}


//Note this calculates the real free electron recombination rate
//not the removal of electrons from the band
gdouble get_avg_recom_n(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Rtot=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Rtot+=(in->ptrap_to_n[z][x][y]);
		}
	}
}
return Rtot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

//Note this calculates the real free hole recombination rate
//not the removal of holes from the band
gdouble get_avg_recom_p(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Rtot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Rtot+=(in->ntrap_to_p[z][x][y]);
		}
	}
}
return Rtot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

gdouble get_avg_Rn(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Rtot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Rtot+=(in->Rn[z][x][y]);
		}
	}
}

return Rtot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

gdouble get_avg_Rp(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Rtot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Rtot+=(in->Rp[z][x][y]);
		}
	}
}

return Rtot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

gdouble get_avg_k(struct device *in)
{
gdouble n=(get_extracted_n(in)*get_extracted_p(in));
gdouble R=(get_avg_recom_n(in)+get_avg_recom_p(in))/2.0;
gdouble k=R/n;
return k;
}

gdouble carrier_count_get_n(struct device *in)
{
return n_count;
}

gdouble carrier_count_get_p(struct device *in)
{
return p_count;
}

gdouble  carrier_count_get_rn(struct device *in)
{
return rn_count;
}

gdouble carrier_count_get_rp(struct device *in)
{
return rp_count;
}

void carrier_count_reset(struct device *in)
{
	n_count=0.0;
	p_count=0.0;
	rn_count=0.0;
	rp_count=0.0;
}

gdouble get_J_recom(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Rtot=0.0;
gdouble add=0.0;
gdouble dx=in->ymesh[1]-in->ymesh[0];



for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{

			add=(in->Rn[z][x][y]-in->Gn[z][x][y]-in->Rp[z][x][y]-in->Gn[z][x][y])*dx/2.0;
			if ((in->interfaceleft==TRUE)&&(y==0))
			{
				add/=2.0;
			}

			if ((in->interfaceright==TRUE)&&(y==in->ymeshpoints-1))
			{
				add/=2.0;
			}

			Rtot+=add;
		}
	}
}

return Rtot*Q;
}

gdouble get_J_recom_n(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Rtot=0.0;
gdouble dx=in->ymesh[1]-in->ymesh[0];
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Rtot+=(in->Rn[z][x][y])*dx;

		}
	}
}

return Rtot;
}

gdouble get_J_recom_p(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Rtot=0.0;
gdouble dx=in->ymesh[1]-in->ymesh[0];

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Rtot+=(in->Rp[z][x][y])*dx;
		}
	}
}

return Rtot;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
gdouble get_avg_J(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble J=0.0;
gdouble Javg=0.0;
gdouble Jstd_dev=0.0;
gdouble Jtot=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Javg+=in->Jn[z][x][y]+in->Jp[z][x][y];
		}
	}
}

Javg/=(gdouble)in->ymeshpoints;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Jstd_dev+=pow(((in->Jn[z][x][y]+in->Jp[z][x][y])-Javg),2.0);
		}
	}
}

Jstd_dev/=(gdouble)in->ymeshpoints;
Jstd_dev=sqrt(Jstd_dev);

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			J=in->Jn[z][x][y]+in->Jp[z][x][y];
			if (fabs(J-Javg)<Jstd_dev*0.05)
			{
				Jtot+=in->Jn[z][x][y]+in->Jp[z][x][y];
				//printf("%d count\n",i);
			}else
			{
			//printf("%d not count\n",i);
			}
		}
	}
}
Jtot/=(gdouble)in->xmeshpoints;
Jtot/=(gdouble)in->ymeshpoints;
Jtot/=(gdouble)in->zmeshpoints;


return Javg;
}

gdouble get_jn_avg(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Jtot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Jtot+=in->Jn[z][x][y];
		}
	}
}
return Jtot/((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints));
}

gdouble get_jp_avg(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble Jtot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			Jtot+=in->Jp[z][x][y];
		}
	}
}
return Jtot/(gdouble)((in->xmeshpoints*in->ymeshpoints*in->zmeshpoints));
}

void carrier_count_add(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble locat_n_tot=0.0;
gdouble locat_p_tot=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		n_count+=(in->Jn[z][x][0]+in->Jn[z][x][in->ymeshpoints-1])*in->dt/Q;
		p_count+=(in->Jp[z][x][0]+in->Jp[z][x][in->ymeshpoints-1])*in->dt/Q;
	}
}

gdouble dx=in->ymesh[1]-in->ymesh[0];
//printf("\n\n%e %e\n\n",dx*(gdouble)in->ymeshpoints,in->ymesh[in->ymeshpoints-1]);
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			locat_n_tot+=in->Rfree[z][x][y]*dx;
			locat_p_tot+=in->Rfree[z][x][y]*dx;
		}
	}
}
//printf("%e\n",in->ymesh[in->ymeshpoints-1]);
//getchar();
rn_count+=locat_n_tot*in->dt;
rp_count+=locat_p_tot*in->dt;

//printf("\n%e %e\n",rn_count,rp_count);
}

gdouble get_extracted_np(struct device *in)
{
return (get_extracted_n(in)+get_extracted_p(in))/2.0;
}

gdouble get_extracted_n(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble sum_n=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			sum_n+=(in->n[z][x][y]-in->nf_save[z][x][y]);
			sum_n+=(in->nt_all[z][x][y]-in->nt_save[z][x][y]);
		}
	}
}
//printf("%le %le %le\n",in->n[i]-in->nfinit[i],in->nt_all[i],in->ntinit[i]);
//getchar();


return sum_n/((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints));
}

gdouble get_total_np(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble sum=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			sum+=(in->n[z][x][y]+in->p[z][x][y]);
			sum+=(in->nt_all[z][x][y]+in->pt_all[z][x][y]);
		}
	}
}

return sum/((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)*2.0);
}

gdouble get_extracted_p(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble sum_p=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			sum_p+=(in->p[z][x][y]-in->pf_save[z][x][y]);
			sum_p+=(in->pt_all[z][x][y]-in->pt_save[z][x][y]);
		}
	}
}

return sum_p/((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints));
}

gdouble get_background_charge(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble sum=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			sum+=(in->nf_save[z][x][y]+in->nt_save[z][x][y]+in->pf_save[z][x][y]+in->pt_save[z][x][y])/2.0;
		}
	}
}
return sum/((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints));
}

gdouble get_extracted_k(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
gdouble n=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=in->Rfree[z][x][y];
			n+=in->n[z][x][y]*in->p[z][x][y];
		}
	}
}
//printf("%le %le %le\n",tot,n,tot/n);
//getchar();
return tot/n;
}

gdouble get_avg_gen(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->Gn[z][x][y]+in->Gp[z][x][y])/2.0;
		}
	}
}

return tot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

gdouble get_avg_mue(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=in->mun[z][x][y]*in->n[z][x][y]/(in->nt_all[z][x][y]+in->n[z][x][y]);
		}
	}
}
return tot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

gdouble get_avg_muh(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=in->mup[z][x][y]*in->p[z][x][y]/(in->pt_all[z][x][y]+in->p[z][x][y]);
		}
	}
}
return tot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

gdouble get_np_tot(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->n[z][x][y]+in->p[z][x][y]+in->pt_all[z][x][y]+in->nt_all[z][x][y]);
		}
	}
}

return tot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)))/2.0;
}

gdouble get_free_np_avg(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{

			tot+=(in->n[z][x][y]+in->p[z][x][y]-in->nf_save[z][x][y]-in->pf_save[z][x][y]);
		}
	}
}

return tot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)))/2.0;
}

gdouble get_free_n_charge(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->n[z][x][y]);
		}
	}
}

return tot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

gdouble get_free_p_charge(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->p[z][x][y]);
		}
	}
}

return tot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

gdouble get_charge_tot(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->p[z][x][y]-in->n[z][x][y]+in->pt_all[z][x][y]-in->nt_all[z][x][y]);
		}
	}
}

return tot;
}

void set_orig_charge_den(struct device *in)
{
int x=0;
int y=0;
int z=0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			in->n_orig[z][x][y]=in->n[z][x][y]+in->nt_all[z][x][y];
			in->p_orig[z][x][y]=in->p[z][x][y]+in->pt_all[z][x][y];

			in->n_orig_f[z][x][y]=in->n[z][x][y];
			in->p_orig_f[z][x][y]=in->p[z][x][y];

			in->n_orig_t[z][x][y]=in->nt_all[z][x][y];
			in->p_orig_t[z][x][y]=in->pt_all[z][x][y];
		}
	}
}
}

gdouble get_free_n_charge_delta(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->n[z][x][y]-in->n_orig_f[z][x][y]);
		}
	}
}

return tot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

gdouble get_free_p_charge_delta(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->p[z][x][y]-in->p_orig_f[z][x][y]);
		}
	}
}

return tot/(((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints)));
}

void reset_np_save(struct device *in)
{
int x=0;
int y=0;
int z=0;

int band;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			in->nf_save[z][x][y]=in->n[z][x][y];
			in->pf_save[z][x][y]=in->p[z][x][y];
			in->nt_save[z][x][y]=in->nt_all[z][x][y];
			in->pt_save[z][x][y]=in->pt_all[z][x][y];
			in->phi_save[z][x][y]=in->phi[z][x][y];
			for (band=0;band<in->srh_bands;band++)
			{
				in->ntb_save[z][x][y][band]=in->nt[z][x][y][band];
				in->ptb_save[z][x][y][band]=in->pt[z][x][y][band];
			}
		}
	}
}

}

void reset_npequlib(struct device *in)
{
	int x=0;
	int y=0;
	int z=0;
	for (z=0;z<in->zmeshpoints;z++)
	{
		for (x=0;x<in->xmeshpoints;x++)
		{
			for (y=0;y<in->ymeshpoints;y++)
			{
				in->nfequlib[z][x][y]=in->n[z][x][y];
				in->pfequlib[z][x][y]=in->p[z][x][y];
				in->ntequlib[z][x][y]=in->nt_all[z][x][y];
				in->ptequlib[z][x][y]=in->pt_all[z][x][y];
			}
		}
	}
}

gdouble get_n_trapped_charge(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->nt_all[z][x][y]);
		}
	}
}

return tot/(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints);
}

gdouble get_p_trapped_charge(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->pt_all[z][x][y]);
		}
	}
}

return tot/(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints);
}

gdouble get_charge_delta(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(((in->n[z][x][y]+in->nt_all[z][x][y])-in->n_orig[z][x][y])+((in->p[z][x][y]+in->pt_all[z][x][y])-in->p_orig[z][x][y]))/2.0;
		}
	}
}

return tot/(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints);
}

gdouble get_n_trapped_charge_delta(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->nt_all[z][x][y]-in->n_orig_t[z][x][y]);
		}
	}
}

return tot/(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints);
}

gdouble get_p_trapped_charge_delta(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble tot=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			tot+=(in->pt_all[z][x][y]-in->p_orig_t[z][x][y]);
		}
	}
}

return tot/(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints);
}

gdouble get_I_recomb(struct device *in)
{
gdouble ret=0.0;

ret=(get_J_recom(in))*(in->xlen*in->zlen)/2.0;

return ret;
}

gdouble get_I(struct device *in)
{
gdouble ret=0.0;
ret+=(get_J_left(in)+get_J_right(in))*(in->xlen*in->zlen)/2.0;

return ret;
}

gdouble get_i_intergration(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble ret=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			ret+=(in->Jn[z][x][y]+in->Jp[z][x][y]);
		}
	}
}

ret*=(in->xlen*in->zlen)/((gdouble)(in->xmeshpoints*in->ymeshpoints*in->zmeshpoints));
return ret;
}

gdouble get_equiv_I(struct simulation *sim,struct device *in)
{
gdouble Iout=0.0;
Iout=get_equiv_J(sim,in)*(in->xlen*in->zlen);
return Iout;
}

gdouble get_equiv_J(struct simulation *sim,struct device *in)
{
gdouble Vapplied=0.0;
Vapplied=contact_get_voltage(sim,in,0);
gdouble J=0.0;
J=get_J(in);
if (in->lr_pcontact==RIGHT) J*= -1.0;
J+=Vapplied/in->Rshunt/in->area;
//printf("%e %e %e\n",(in->xlen*in->zlen),in->Rshunt,in->area);
return J;
}

gdouble get_I_ce(struct simulation *sim,struct device *in)
{
gdouble Vapplied=0.0;
Vapplied=contact_get_voltage(sim,in,0);

gdouble ret=Vapplied/(in->Rcontact+in->Rshort);
if (in->time<0.0)
{
	ret=0.0;
}
return ret;
}

gdouble get_avg_field(struct device *in)
{
	return (in->phi[0][0][in->ymeshpoints-1]-in->phi[0][0][0]);
}

void cal_J_drift_diffusion(struct device *in)
{
gdouble Ecl=0.0;
gdouble Ecr=0.0;
gdouble dEc=0.0;
gdouble nl=0.0;
gdouble nr=0.0;
gdouble dn=0.0;
gdouble pl=0.0;
gdouble pr=0.0;
gdouble dp=0.0;
gdouble xl=0.0;
gdouble xr=0.0;
gdouble dx=0.0;

int x=0;
int y=0;
int z=0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			if (y==0)
			{
				nl=in->n[z][x][0];
				pl=in->p[z][x][0];
				Ecl=in->Ec[z][x][0];
				xl=in->ymesh[0];
			}else
			{
				nl=in->n[z][x][y-1];
				pl=in->p[z][x][y-1];
				Ecl=in->Ec[z][x][y-1];
				xl=in->ymesh[y-1];
			}

			if (y==in->ymeshpoints-1)
			{
				nr=in->n[z][x][in->ymeshpoints-1];
				pr=in->p[z][x][in->ymeshpoints-1];
				Ecr=in->Ec[z][x][in->ymeshpoints-1];
				xr=in->ymesh[in->ymeshpoints-1];
			}else
			{
				nr=in->n[z][x][y+1];
				pr=in->p[z][x][y+1];
				Ecr=in->Ec[z][x][y+1];
				xr=in->ymesh[y+1];
			}
			dn=(nr-nl);
			dp=(pr-pl);
			dEc=Ecr-Ecl;
			dx=(xr-xl);
			in->Jn_diffusion[z][x][y]=Q*in->Dn[z][x][y]*dn/dx;
			in->Jn_drift[z][x][y]=Q*in->mun[z][x][y]*in->n[z][x][y]*dEc/dx;

			in->Jp_diffusion[z][x][y]= -Q*in->Dp[z][x][y]*dp/dx;
			in->Jp_drift[z][x][y]=Q*in->mup[z][x][y]*in->p[z][x][y]*dEc/dx;
		}
	}
}

}

gdouble get_Jn_diffusion(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble J=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			J+=fabs(in->Jn_diffusion[z][x][y]);
		}
	}
}

return J;
}

gdouble get_Jn_drift(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble J=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			J+=fabs(in->Jn_drift[z][x][y]);
		}
	}
}

return J;
}

gdouble get_Jp_diffusion(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble J=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			J+=fabs(in->Jp_diffusion[z][x][y]);
		}
	}
}

return J;
}

gdouble get_Jp_drift(struct device *in)
{
int x=0;
int y=0;
int z=0;

gdouble J=0.0;
for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			J+=fabs(in->Jp_drift[z][x][y]);
		}
	}
}

return J;
}

gdouble get_equiv_V(struct simulation *sim,struct device *in)
{
gdouble J=0.0;
gdouble Vapplied=0.0;
Vapplied=contact_get_voltage(sim,in,0);
//if (in->adv_sim==FALSE)
//{
//J=get_J_recom(in);
//}else
//{
J=get_equiv_J(sim,in);
//}
//printf("%e\n",in->Rcontact);
gdouble V=J*in->Rcontact*in->area+Vapplied;
return V;
}

gdouble get_J(struct device *in)
{
//int i;
gdouble ret=0.0;

ret=(get_J_left(in)+get_J_right(in))/2.0;

return ret;
}

gdouble get_J_left(struct device *in)
{
int z=0;
int x=0;
gdouble ret=0.0;
gdouble count=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		if (in->n_contact[z][x]==0)
		{
			ret+=in->Jpleft[z][x]+in->Jnleft[z][x];
			count=count+1.0;
		}
	}
}
ret/=count;

//printf("%le %le\n",in->Jpleft,in->Jnleft);
return ret*Q;
}

gdouble get_J_right(struct device *in)
{
int z=0;
int x=0;
gdouble ret=0.0;
gdouble count=0.0;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		if (in->n_contact[z][x]==0)
		{
			ret+=in->Jnright[z][x]+in->Jpright[z][x];
			count=count+1.0;
		}
	}
}
ret/=count;
//printf("%e %e\n",in->Jn[0][0],in->Jn[0][in->ymeshpoints-2]);
//getchar();
return ret*Q;
}

