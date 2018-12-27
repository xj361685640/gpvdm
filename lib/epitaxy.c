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

/** @file epitaxy.c
	@brief Load the epitaxy structure.
*/

#include <string.h>
#include "epitaxy.h"
#include "inp.h"
#include "util.h"
#include "const.h"
#include <cal_path.h>

/**
 * @brief Load the epitaxy in from epitaxy.inp
 *
 */
void epitaxy_load(struct simulation *sim,struct epitaxy *in, char *file)
{
	int i;
	char dos_file[20];
	char pl_file[20];
	struct inp_file inp;
	in->electrical_layers=0;

	inp_init(sim,&inp);
	if (inp_load(sim, &inp , file)!=0)
	{
		ewe(sim,"Epitaxy: I can't find file %s\n",file);
	}

	inp_check(sim,&inp,1.3);
	inp_reset_read(sim,&inp);
	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%d",&(in->layers));

	if (in->layers>20)
	{
		ewe(sim,"Too many material layers\n");
	}

	if (in->layers<1)
	{
		ewe(sim,"No material layers\n");
	}

	for (i=0;i<in->layers;i++)
	{
		inp_get_string(sim,&inp);	//token
		strcpy(in->name[i],inp_get_string(sim,&inp));

		inp_get_string(sim,&inp);	//token
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->width[i]));
		in->width[i]=fabs(in->width[i]);

		inp_get_string(sim,&inp);	//token
		strcpy(in->mat_file[i],inp_get_string(sim,&inp));
		assert_platform_path(in->mat_file[i]);
		
		inp_get_string(sim,&inp);	//token
		strcpy(dos_file,inp_get_string(sim,&inp));

		inp_get_string(sim,&inp);	//token
		strcpy(pl_file,inp_get_string(sim,&inp));

		char temp[20];
		char full_path[PATHLEN];
		if (strcmp_begin(dos_file,"dos")==0)
		{
			strcpy(temp,dos_file);
			strcat(temp,".inp");
			join_path(2, full_path, get_input_path(sim), temp);
			in->electrical_layer[i]=TRUE;
			if (inp_isfile(sim,full_path)!=0)
			{
				ewe(sim,"dos file %s does not exist",full_path);
			}
			strcpy(in->dos_file[in->electrical_layers],dos_file);

			strcpy(temp,pl_file);
			strcat(temp,".inp");
			join_path(2, full_path, get_input_path(sim), temp);
			if (inp_isfile(sim,full_path)!=0)
			{
				ewe(sim,"pl file %s does not exist",full_path);
			}
			strcpy(in->pl_file[in->electrical_layers],pl_file);

			in->electrical_layers++;
		}else
		{
			in->electrical_layer[i]=FALSE;
		}

	}

	char * ver = inp_get_string(sim,&inp);
	if (strcmp(ver,"#ver")!=0)
	{
			ewe(sim,"No #ver tag found in file\n");
	}

	inp_free(sim,&inp);

	epitaxy_load_materials(sim,in);
}

/**
 * @brief Get the height of the layers which are electrically active.
 *
 */
gdouble epitaxy_get_electrical_length(struct epitaxy *in)
{
int i=0;
gdouble tot=0.0;

for (i=0;i<in->layers;i++)
{
	if (in->electrical_layer[i]==TRUE)
	{
		tot+=in->width[i];
	}
}
//if (tot>300e-9)
//{
//	ewe(sim,"Can't simulate structures bigger than 300 nm\n");
//}
return tot;
}

/**
 * @brief Get the height of the layers which are optically active. i.e. the full height of the device. 
 *
 */
gdouble epitaxy_get_optical_length(struct epitaxy *in)
{
int i=0;
gdouble tot=0.0;

for (i=0;i<in->layers;i++)
{
	tot+=in->width[i];
}

return tot;
}

/**
 * @brief Return the index of a layer in the device.
 *
 */
int epitaxy_get_optical_material_layer(struct epitaxy *in,gdouble pos)
{
int i=0;
gdouble layer_end=0.0;
for (i=0;i<in->layers;i++)
{
	layer_end+=in->width[i];

	if (pos<layer_end)
	{
		return i;
	}

}

return -1;
}

/**
 * @brief Return the index of a layer in the device, but setting the zero position at the start of the electrical layer.
 *
 */
int epitaxy_get_electrical_material_layer(struct epitaxy *in,gdouble pos)
{
int i=0;
gdouble layer_end=0.0;
int electrical_layer=0;
for (i=0;i<in->layers;i++)
{
	if (in->electrical_layer[i]==TRUE)
	{
		layer_end+=in->width[i];

		if (pos<layer_end)
		{
			return electrical_layer;
		}
		electrical_layer++;
	}

}

return -1;
}

/**
 * @brief Return the index of a layer in the device, but setting the zero position at the start of the electrical layer. - Duplicate?
 *
 */
int epitaxy_get_epitaxy_layer_using_electrical_pos(struct epitaxy *in,gdouble pos)
{
int i=0;
gdouble layer_end=0.0;
int electrical_layer=0;
for (i=0;i<in->layers;i++)
{
	if (in->electrical_layer[i]==TRUE)
	{
		layer_end+=in->width[i];

		if (pos<layer_end)
		{
			return i;
		}
		electrical_layer++;
	}

}

return -1;
}

/**
 * @brief Return the position of the start of the first electrical layer in the device.
 *
 */
gdouble epitaxy_get_device_start(struct epitaxy *in)
{
int i=0;
gdouble pos=0.0;
for (i=0;i<in->layers;i++)
{

	if (in->electrical_layer[i]==TRUE)
	{
		return pos;
	}
	pos+=in->width[i];

}

return -1;
}

/**
 * @brief Return the position of the end of the electrical layers in the device.
 *
 */
gdouble epitaxy_get_device_stop(struct epitaxy *in)
{
int i=0;
gdouble pos=0.0;
int found=FALSE;
for (i=0;i<in->layers;i++)
{
	pos+=in->width[i];

	if (in->electrical_layer[i]==TRUE)
	{
		found=TRUE;
	}

	if ((in->electrical_layer[i]==FALSE)&&(found==TRUE))
	{
		return pos;
	}

}

if (found==TRUE)
{
	return pos;
}


return -1;
}

/**
 * @brief Return the index of the layer where the device starts.
 *
 */
gdouble epitaxy_get_device_start_i(struct epitaxy *in)
{
int i=0;
for (i=0;i<in->layers;i++)
{

	if (in->electrical_layer[i]==TRUE)
	{
		return i;
	}

}

return -1;
}

/**
 * @brief Free the epitaxy structure.
 *
 */
void epitaxy_free(struct epitaxy *in)
{
	epitaxy_free_materials(in);
}


