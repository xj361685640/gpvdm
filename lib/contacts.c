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


#include <string.h>
#include "epitaxy.h"
#include "inp.h"
#include "util.h"
#include "const.h"
#include <cal_path.h>
#include "contacts.h"

void contacts_time_step(struct simulation *sim,struct device *in)
{
	int i;

	for (i=0;i<in->ncontacts;i++)
	{
		in->contacts[i].voltage_last=in->contacts[i].voltage;
	}
}

void contacts_load(struct simulation *sim,struct device *in)
{
	int i;
	struct inp_file inp;
	in->ncontacts=0;

	inp_init(sim,&inp);
	if (inp_load(sim, &inp , "contacts.inp")!=0)
	{
		ewe(sim,"Can't open the file contacts\n");
	}

	inp_check(sim,&inp,1.1);
	inp_reset_read(sim,&inp);
	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%d",&(in->ncontacts));

	if (in->ncontacts>10)
	{
		ewe(sim,"Too many contacts\n");
	}

	if (in->ncontacts<1)
	{
		ewe(sim,"No contacts\n");
	}

	gdouble pos=0.0;
	int active=FALSE;
	for (i=0;i<in->ncontacts;i++)
	{
		
		inp_get_string(sim,&inp);	//active contact
		strcpy(in->contacts[i].name,inp_get_string(sim,&inp));

		inp_get_string(sim,&inp);	//position
		in->contacts[i].position=english_to_bin(sim, inp_get_string(sim,&inp));
	
		inp_get_string(sim,&inp);	//active contact
		in->contacts[i].active=english_to_bin(sim, inp_get_string(sim,&inp));

		inp_get_string(sim,&inp);	//start
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].start));

		inp_get_string(sim,&inp);	//width
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].width));

		inp_get_string(sim,&inp);	//depth
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].depth));

		inp_get_string(sim,&inp);	//voltage
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].voltage));
		in->contacts[i].voltage_last=in->contacts[i].voltage;


		pos+=in->contacts[i].width;
	}

	char * ver = inp_get_string(sim,&inp);
	if (strcmp(ver,"#ver")!=0)
	{
			ewe(sim,"No #ver tag found in file\n");
	}

	inp_free(sim,&inp);

	contacts_update(sim,in);
}

void contacts_force_to_zero(struct simulation *sim,struct device *in)
{
int x;
int z;

for (x=0;x<in->xmeshpoints;x++)
{
	for (z=0;z<in->zmeshpoints;z++)
	{
		in->Vapplied_l[z][x]=0.0;
		in->Vapplied_r[z][x]=0.0;
	}

}

}

void contacts_dump(struct device *in)
{
int i;
	for (i=0;i<in->ncontacts;i++)
	{
		printf("%Le\n",in->contacts[i].voltage);
	}
	
}

void contacts_update(struct simulation *sim,struct device *in)
{
int i;
int x;
int z;
int n;
int found=FALSE;

gdouble value=0.0;

if (in->xmeshpoints==1)
{
	for (z=0;z<in->zmeshpoints;z++)
	{
		for (i=0;i<in->ncontacts;i++)
		{
			if (in->contacts[i].position==TOP)
			{
				in->Vapplied_r[z][0]=in->contacts[i].voltage;
			}else
			{
				in->Vapplied_l[z][0]=in->contacts[i].voltage;
			}
		}
	}

	return;
}

//Contacts on top
for (x=0;x<in->xmeshpoints;x++)
{
	found=FALSE;
	for (i=0;i<in->ncontacts;i++)
	{
		if ((in->xmesh[x]>=in->contacts[i].start)&&(in->xmesh[x]<in->contacts[i].start+in->contacts[i].width))
		{
			if (in->contacts[i].position==TOP)
			{
				value=in->contacts[i].voltage;
				n=i;
				found=TRUE;
				break;
			}
		}
	}

	if (found==FALSE)
	{
		value=0.0;
		n=-1;
	}

	for (z=0;z<in->zmeshpoints;z++)
	{
		in->Vapplied_r[z][x]=value;
		in->n_contact_r[z][x]=n;
	}
	
}

//Contacts on btm
for (x=0;x<in->xmeshpoints;x++)
{
	found=FALSE;
	for (i=0;i<in->ncontacts;i++)
	{
		if ((in->xmesh[x]>=in->contacts[i].start)&&(in->xmesh[x]<in->contacts[i].start+in->contacts[i].width))
		{
			if (in->contacts[i].position==BOTTOM)
			{
				value=in->contacts[i].voltage;
				n=i;
				found=TRUE;
				break;
			}
		}
	}

	if (found==FALSE)
	{
		value=0.0;
		n=-1;
	}

	for (z=0;z<in->zmeshpoints;z++)
	{
		in->Vapplied_l[z][x]=value;
		in->n_contact_l[z][x]=n;
	}
	
}
}

gdouble contact_get_voltage_last(struct simulation *sim,struct device *in,int contact)
{
	return in->contacts[contact].voltage_last;
}

gdouble contact_get_voltage(struct simulation *sim,struct device *in,int contact)
{
	return in->contacts[contact].voltage;
}

void contact_set_voltage(struct simulation *sim,struct device *in,int contact,gdouble voltage)
{
	in->contacts[contact].voltage=voltage;
	contacts_update(sim,in);
}

void contact_set_active_contact_voltage(struct simulation *sim,struct device *in,gdouble voltage)
{
	int i=0;

	for (i=0;i<in->ncontacts;i++)
	{
		if (in->contacts[i].active==TRUE)
		{
			in->contacts[i].voltage=voltage;
		}
	}

	contacts_update(sim,in);

}

long double contact_get_active_contact_voltage(struct simulation *sim,struct device *in)
{
	int i=0;

	if ((in->xmeshpoints==1)&&(in->zmeshpoints==1))
	{
		return in->contacts[0].voltage;

	}else
	{
		for (i=0;i<in->ncontacts;i++)
		{
			if (in->contacts[i].active==TRUE)
			{
				return in->contacts[i].voltage;
			}
		}
	}

}

void contact_set_all_voltages(struct simulation *sim,struct device *in,gdouble voltage)
{
int i;
	for (i=0;i<in->ncontacts;i++)
	{
		in->contacts[i].voltage=voltage;
	}

	contacts_update(sim,in);
}

long double contacts_get_Jleft(struct device *in)
{
int i;
int x;
int z;

long double tot=0.0;
long double count=0.0;

for (x=0;x<in->xmeshpoints;x++)
{
		for (z=0;z<in->zmeshpoints;z++)
		{
			if (in->n_contact_l[z][x]>=0)
			{
				tot+=in->Jpleft[z][x]+in->Jnleft[z][x];
				count=count+1.0;						//this will need updating for meshes which change
			}
		}
}

tot=tot/count;

return tot*Q;
}

long double contacts_get_Jright(struct device *in)
{
int i;
int x;
int z;

long double tot=0.0;
long double count=0.0;

for (x=0;x<in->xmeshpoints;x++)
{
		for (z=0;z<in->zmeshpoints;z++)
		{
			if (in->n_contact_r[z][x]>=0)
			{
				tot+=in->Jpright[z][x]+in->Jnright[z][x];
				count=count+1.0;
			}
		}
}

tot=tot/count;

return tot*Q;
}


long double contacts_get_J(struct device *in, int n)
{
int i;
int x;
int z;

long double tot=0.0;
long double count=0.0;

for (x=0;x<in->xmeshpoints;x++)
{
		for (z=0;z<in->zmeshpoints;z++)
		{
			for (i=0;i<in->ncontacts;i++)
			{
				if (in->n_contact_r[z][x]==n)
				{
					tot+=in->Jpright[z][x]+in->Jnright[z][x];
					count=count+1.0;						//this will need updating for meshes which change
				}
				
				if (in->n_contact_l[z][x]==n)
				{
					tot+=in->Jpleft[z][x]+in->Jnleft[z][x];
					count=count+1.0;						//this will need updating for meshes which change
				}
			}
		}
}

tot=tot/count;

return tot*Q;
}

void contacts_passivate(struct simulation *sim,struct device *in)
{
int i;
int x;
int y;
int z;

//passivate under each contact
for (x=0;x<in->xmeshpoints;x++)
{
	for (y=0;y<in->ymeshpoints;y++)
	{
		for (z=0;z<in->zmeshpoints;z++)
		{

			for (i=0;i<in->ncontacts;i++)
			{
				if (in->contacts[i].position==TOP)
				{
					if ((in->ylen-in->ymesh[y]<=in->contacts[i].depth)&&(in->xmesh[x]>in->contacts[i].start)&&(in->xmesh[x]<in->contacts[i].start+in->contacts[i].width))
					{
						in->mun[z][x][y]=1e-15;
						in->mup[z][x][y]=1e-15;
					}
				}

				if (in->contacts[i].position==BOTTOM)
				{
					if ((in->ymesh[y]<=in->contacts[i].depth)&&(in->xmesh[x]>in->contacts[i].start)&&(in->xmesh[x]<in->contacts[i].start+in->contacts[i].width))
					{
						in->mun[z][x][y]=1e-15;
						in->mup[z][x][y]=1e-15;
					}
				}
			}
		}
	}
}

for (x=0;x<in->xmeshpoints;x++)
{
	for (z=0;z<in->zmeshpoints;z++)
	{
		i=in->n_contact_r[z][x];
		if (i==-1)
		{
			in->mun[z][x][in->ymeshpoints-1]=1e-15;
			in->mup[z][x][in->ymeshpoints-1]=1e-15;
		}

		i=in->n_contact_l[z][x];
		if (i==-1)
		{
			in->mun[z][x][0]=1e-15;
			in->mup[z][x][0]=1e-15;
		}
	}
}

}
