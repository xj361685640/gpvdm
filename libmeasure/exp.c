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


#include <exp.h>
#include "sim.h"
#include "i.h"
#include "buffer.h"
#include "contacts.h"
#include <memory.h>

static gdouble n_count=0.0;
static gdouble p_count=0.0;
static gdouble rn_count=0.0;
static gdouble rp_count=0.0;

/**
* @brief Average position of electron and holes (not updated)
*/
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

/**
* @brief Get the change in charge density (not updated)
*/
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

	diff/=(n+p);
	diff*=100.0;
}

return diff;
}



/**
* @brief Get the average recombination density
*/
gdouble get_avg_recom(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->ptrap_to_n);
ret+=three_d_avg(in, in->ntrap_to_p);

return ret;
}

/**
* @brief Get the average electron relaxation rate
*/
gdouble get_avg_relax_n(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->nrelax);

return ret;
}

/**
* @brief Get the average hole relaxation rate
*/
gdouble get_avg_relax_p(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->prelax);

return ret;
}


/**
* @brief Free electron recombination rate
* Note this calculates the real free electron recombination rate
* not the removal of electrons from the band
*/
gdouble get_avg_recom_n(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->ptrap_to_n);

return ret;
}

/**
* @brief Free hole recombination rate
* Note this calculates the real free hole recombination rate
* not the removal of holes from the band
*/
gdouble get_avg_recom_p(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->ntrap_to_p);

return ret;
}

/**
* @brief Removal of electrons from the band
*/
gdouble get_avg_Rn(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->Rn);

return ret;
}

/**
* @brief Removal of holes from the band
*/
gdouble get_avg_Rp(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->Rp);

return ret;
}

/**
* @brief Calculation of k the recombination prefactor
*/
gdouble get_avg_k(struct device *in)
{
gdouble n=(get_extracted_n(in)*get_extracted_p(in));
gdouble R=(get_avg_recom_n(in)+get_avg_recom_p(in))/2.0;
gdouble k=R/n;
return k;
}

/**
* @brief Carrier count (depreshated)
*/
gdouble carrier_count_get_n(struct device *in)
{
return n_count;
}

/**
* @brief Carrier count (depreshated)
*/
gdouble carrier_count_get_p(struct device *in)
{
return p_count;
}

/**
* @brief Carrier count (depreshated)
*/
gdouble  carrier_count_get_rn(struct device *in)
{
return rn_count;
}

/**
* @brief Carrier count (depreshated)
*/
gdouble carrier_count_get_rp(struct device *in)
{
return rp_count;
}

/**
* @brief Carrier count (depreshated)
*/
void carrier_count_reset(struct device *in)
{
	n_count=0.0;
	p_count=0.0;
	rn_count=0.0;
	rp_count=0.0;
}


/**
* @brief Calculate J from the recombination rate
* Not updated for variable mesh
*/
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


/**
* @brief Calculate the average J and the std of J
* Not updated for variable mesh
*/
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

				
			}
		}
	}
}
Jtot/=(gdouble)in->xmeshpoints;
Jtot/=(gdouble)in->ymeshpoints;
Jtot/=(gdouble)in->zmeshpoints;


return Javg;
}

/**
* @brief Calculate the average Jn
*/
gdouble get_jn_avg(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->Jn);

return ret;
}

/**
* @brief Calculate the average Jp
*/
gdouble get_jp_avg(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->Jp);

return ret;
}

/**
* @brief Carrier count (depreshated)
*/
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
rn_count+=locat_n_tot*in->dt;
rp_count+=locat_p_tot*in->dt;

}

/**
* @brief Calculate extracted np
*/
gdouble get_extracted_np(struct device *in)
{
return (get_extracted_n(in)+get_extracted_p(in))/2.0;
}

/**
* @brief Calculate extracted n from a save point
*/
gdouble get_extracted_n(struct device *in)
{
long double ret=0.0;

long double n=0.0;
long double nf_save=0.0;

long double nt_all=0.0;
long double nt_save=0.0;

n=three_d_avg(in, in->n);
nf_save=three_d_avg(in, in->nf_save);

nt_all=three_d_avg(in, in->n);
nt_save=three_d_avg(in, in->nf_save);

ret=n-nf_save +nt_all-nt_save;

return ret;
}

/**
* @brief Get the total number of charge carriers in the device.
*/
gdouble get_total_np(struct device *in)
{
long double ret=0.0;
long double n=0.0;
long double p=0.0;
long double nt_all=0.0;
long double pt_all=0.0;

n=three_d_avg(in, in->n);
p=three_d_avg(in, in->p);

nt_all=three_d_avg(in, in->n);
pt_all=three_d_avg(in, in->n);

ret=n+p+nt_all+pt_all;

ret/=2.0;
return ret;
}

/**
* @brief Get the number of extracted holes
*/
gdouble get_extracted_p(struct device *in)
{
long double sum_p=0.0;

sum_p=three_d_avg(in, in->p);
sum_p-=three_d_avg(in, in->pf_save);

sum_p+=three_d_avg(in, in->pt_all);
sum_p-=three_d_avg(in, in->pt_save);

return sum_p;
}

gdouble get_background_charge(struct device *in)
{
long double sum=0.0;

sum=three_d_avg(in, in->nf_save);
sum+=three_d_avg(in, in->nt_save);

sum+=three_d_avg(in, in->pf_save);
sum+=three_d_avg(in, in->pt_save);

sum/=2.0;

return sum;
}

/**
* @brief Calculate the value of k
* Not updated for a variable mesh
*/
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

return tot/n;
}

/**
* @brief Calculate the average generation rate
*/
gdouble get_avg_gen(struct device *in)
{
long double ret_Gn=0.0;
long double ret_Gp=0.0;

ret_Gn=three_d_avg(in, in->Gn);
ret_Gp=three_d_avg(in, in->Gp);

return (ret_Gn+ret_Gp)/2.0;
}

/**
* @brief Calculate the average electron mobility
*/
gdouble get_avg_mue(struct device *in)
{
int x=0;
int y=0;
int z=0;
long double sum=0.0;
long double ret=0.0;
	for (z = 0; z < in->zmeshpoints; z++)
	{
		for (x = 0; x < in->xmeshpoints; x++)
		{
			for (y = 0; y < in->ymeshpoints; y++)
			{
				sum+=in->mun[z][x][y]*in->n[z][x][y]*in->dxmesh[x]*in->dymesh[y]*in->dzmesh[z]/(in->nt_all[z][x][y]+in->n[z][x][y]);
			}
			
		}
	}

ret=sum/(in->zlen*in->xlen*in->ylen);

return ret;
}

/**
* @brief Calculate the average hole mobility
*/
gdouble get_avg_muh(struct device *in)
{
int x=0;
int y=0;
int z=0;
long double sum=0.0;
long double ret=0.0;
	for (z = 0; z < in->zmeshpoints; z++)
	{
		for (x = 0; x < in->xmeshpoints; x++)
		{
			for (y = 0; y < in->ymeshpoints; y++)
			{
				sum+=in->mup[z][x][y]*in->p[z][x][y]*in->dxmesh[x]*in->dymesh[y]*in->dzmesh[z]/(in->pt_all[z][x][y]+in->p[z][x][y]);
			}
			
		}
	}

ret=sum/(in->zlen*in->xlen*in->ylen);

return ret;
}

/**
* @brief Calculate average charge density in the device.
*/
gdouble get_np_tot(struct device *in)
{
long double sum=0.0;

sum=three_d_avg(in, in->n);
sum+=three_d_avg(in, in->p);

sum+=three_d_avg(in, in->pt_all);
sum+=three_d_avg(in, in->nt_all);

sum/=2.0;

return sum;
}

/**
* @brief Calculate average change in free carrier density in the device
*/
gdouble get_free_np_avg(struct device *in)
{
long double sum=0.0;

sum=three_d_avg(in, in->n);
sum+=three_d_avg(in, in->p);

sum-=three_d_avg(in, in->nf_save);
sum-=three_d_avg(in, in->pf_save);

sum/=2.0;

return sum;
}

/**
* @brief Calculate free electron density
*/
gdouble get_free_n_charge(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->n);

return ret;
}

/**
* @brief Calculate free hole density
*/
gdouble get_free_p_charge(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->p);

return ret;
}

/**
* @brief Total charge in the device
*/
gdouble get_charge_tot(struct device *in)
{
long double ret=0.0;
long double n=0.0;
long double p=0.0;
long double pt_all=0.0;
long double nt_all=0.0;

n=three_d_avg(in, in->n);
p=three_d_avg(in, in->p);
nt_all=three_d_avg(in, in->nt_all);
pt_all=three_d_avg(in, in->pt_all);

ret=p-n+pt_all-nt_all;
return ret;
}

/**
* @brief Set the origonal charge density
*/
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

/**
* @brief Calculate the change in free electron density
*/
gdouble get_free_n_charge_delta(struct device *in)
{
long double sum=0.0;

sum=three_d_avg(in, in->n);
sum-=three_d_avg(in, in->n_orig_f);

return sum;
}

/**
* @brief Calculate the change in free hole density
*/
gdouble get_free_p_charge_delta(struct device *in)
{
long double sum=0.0;

sum=three_d_avg(in, in->p);
sum-=three_d_avg(in, in->p_orig_f);

return sum;
}

/**
* @brief Save the device parameters
*/
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

/**
* @brief Reset np equlib
*/
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

/**
* @brief Get the trapped electron density
*/
gdouble get_n_trapped_charge(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->nt_all);

return ret;
}

/**
* @brief Get the trapped hole density
*/
gdouble get_p_trapped_charge(struct device *in)
{
long double ret=0.0;

ret=three_d_avg(in, in->pt_all);

return ret;
}

/**
* @brief Get the change in charge using orig
*/
gdouble get_charge_delta(struct device *in)
{
long double tot=0.0;

tot=three_d_avg(in, in->n);
tot+=three_d_avg(in, in->nt_all);
tot-=three_d_avg(in, in->n_orig);

tot+=three_d_avg(in, in->p);
tot+=three_d_avg(in, in->pt_all);
tot-=three_d_avg(in, in->p_orig);

tot/=2.0;

return tot;
}

/**
* @brief Get the change in trapped electrons using orig
*/
gdouble get_n_trapped_charge_delta(struct device *in)
{
long double tot=0.0;

tot=three_d_avg(in, in->nt_all);
tot-=three_d_avg(in, in->n_orig_t);

return tot;
}

/**
* @brief Get the change in trapped holes using orig
*/
gdouble get_p_trapped_charge_delta(struct device *in)
{
long double tot=0.0;

tot=three_d_avg(in, in->pt_all);
tot-=three_d_avg(in, in->p_orig_t);

return tot;
}

/**
* @brief Calculate the external current using internal recombination
*/
gdouble get_I_recomb(struct device *in)
{
gdouble ret=0.0;

ret=(get_J_recom(in))*(in->xlen*in->zlen)/2.0;

return ret;
}

/**
* @brief Calculate the external current using the values on the contacts
*/
gdouble get_I(struct device *in)
{
gdouble ret=0.0;
ret+=(get_J_left(in)+get_J_right(in))*(in->xlen*in->zlen)/2.0;

return ret;
}

/**
* @brief Calculate i by intergrating across the device.
*/
gdouble get_i_intergration(struct device *in)
{
long double tot=0.0;
long double ret=0.0;

tot=three_d_avg(in, in->Jn);
tot+=three_d_avg(in, in->Jp);

ret=(in->xlen*in->zlen)*tot;
return ret;
}

/**
* @brief Calculate I including paracitic commponents
*/
gdouble get_equiv_I(struct simulation *sim,struct device *in)
{
gdouble Iout=0.0;
Iout=get_equiv_J(sim,in)*(in->xlen*in->zlen);
return Iout;
}

/**
* @brief Calculate J including paracitic commponents
*/
gdouble get_equiv_J(struct simulation *sim,struct device *in)
{
gdouble Vapplied=0.0;
Vapplied=contact_get_active_contact_voltage(sim,in);
gdouble J=0.0;
J=get_J(in);
if (in->lr_pcontact==RIGHT) J*= -1.0;
J+=Vapplied/in->Rshunt/in->area;

return J;
}

/**
* @brief Get I for charge extraction
*/
gdouble get_I_ce(struct simulation *sim,struct device *in)
{
gdouble Vapplied=0.0;
Vapplied=contact_get_active_contact_voltage(sim,in);

gdouble ret=Vapplied/(in->Rcontact+in->Rshort);
if (in->time<0.0)
{
	ret=0.0;
}
return ret;
}

/**
* @brief Get the average field across the device
*/
gdouble get_avg_field(struct device *in)
{
	return (in->phi[0][0][in->ymeshpoints-1]-in->phi[0][0][0]);
}

/**
* @brief Calculate the drift and diffusion currents
* (Not updated for varialbe mesh)
*/
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

/**
* @brief Jn diffusion
*/
gdouble get_Jn_diffusion(struct device *in)
{
long double J=0.0;

J=three_d_avg_fabsl(in, in->Jn_diffusion);

return J;
}

/**
* @brief Jn drift
*/
gdouble get_Jn_drift(struct device *in)
{
long double J=0.0;

J=three_d_avg_fabsl(in, in->Jn_drift);

return J;
}

/**
* @brief Jp diffusion
*/
gdouble get_Jp_diffusion(struct device *in)
{
long double J=0.0;

J=three_d_avg_fabsl(in, in->Jp_diffusion);

return J;
}

/**
* @brief Jp drift
*/
gdouble get_Jp_drift(struct device *in)
{
long double J=0.0;

J=three_d_avg_fabsl(in, in->Jp_drift);

return J;
}

/**
* @brief Get equivlent V
*/
gdouble get_equiv_V(struct simulation *sim,struct device *in)
{
gdouble J=0.0;
gdouble Vapplied=0.0;
Vapplied=contact_get_active_contact_voltage(sim,in);
//if (in->adv_sim==FALSE)
//{
//J=get_J_recom(in);
//}else
//{
J=get_equiv_J(sim,in);
//}

gdouble V=J*in->Rcontact*in->area+Vapplied;
return V;
}

/**
* @brief Get J
*/
gdouble get_J(struct device *in)
{
//int i;
gdouble ret=0.0;

ret=(get_J_left(in)+get_J_right(in))/2.0;

return ret;
}

/**
* @brief Get J left
*/
gdouble get_J_left(struct device *in)
{
long double ret=0.0;
ret = contacts_get_Jleft(in);
return ret;
}

/**
* @brief Get J right
*/
gdouble get_J_right(struct device *in)
{
long double ret=0.0;
ret = contacts_get_Jright(in);
return ret;
}

