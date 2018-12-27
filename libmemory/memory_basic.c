//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

/** @file memory_basic.c
@brief memory functions for 3D arrays
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lang.h>
#include <complex_solver.h>
#include "sim.h"
#include "dump.h"
#include "mesh.h"
#include <math.h>
#include "log.h"
#include <solver_interface.h>
#include "memory.h"

void free_srh_bands(struct device *in, gdouble **** var)
{
	int x=0;
	int y=0;
	int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{
		for (x = 0; x < in->xmeshpoints; x++)
		{
			for (y = 0; y < in->ymeshpoints; y++)
			{
					free(var[z][x][y]);
			}
			free(var[z][x]);
		}
		free(var[z]);
	}

	free(var);
}

void malloc_3d_gdouble(struct device *in, gdouble * (***var))
{
	int x=0;
	int y=0;
	int z=0;


	*var = (gdouble ***) malloc(in->zmeshpoints * sizeof(gdouble **));

	for (z = 0; z < in->zmeshpoints; z++)
	{
		(*var)[z] = (gdouble **) malloc(in->xmeshpoints * sizeof(gdouble*));
		for (x = 0; x < in->xmeshpoints; x++)
		{
			(*var)[z][x] = (gdouble *) malloc(in->ymeshpoints * sizeof(gdouble));
			memset((*var)[z][x], 0, in->ymeshpoints * sizeof(gdouble));
		}
	}

}

void three_d_set_gdouble(struct device *in, gdouble ***var, gdouble val)
{
int x=0;
int y=0;
int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{
		for (x = 0; x < in->xmeshpoints; x++)
		{
			for (y = 0; y < in->ymeshpoints; y++)
			{
				var[z][x][y]=val;
			}

		}
	}

}


void three_d_sub_gdouble(struct device *in, gdouble ***var, gdouble ***sub)
{
int x=0;
int y=0;
int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{
		for (x = 0; x < in->xmeshpoints; x++)
		{
			for (y = 0; y < in->ymeshpoints; y++)
			{
				var[z][x][y]-=sub[z][x][y];
			}

		}
	}

}

void three_d_copy_gdouble(struct device *in, gdouble ***dst, gdouble ***src)
{
int x=0;
int y=0;
int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{
		for (x = 0; x < in->xmeshpoints; x++)
		{
			for (y = 0; y < in->ymeshpoints; y++)
			{
				dst[z][x][y]=src[z][x][y];
			}

		}
	}

}

void three_d_add_gdouble(struct device *in, gdouble ***var, gdouble ***add)
{
int x=0;
int y=0;
int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{
		for (x = 0; x < in->xmeshpoints; x++)
		{
			for (y = 0; y < in->ymeshpoints; y++)
			{
				var[z][x][y]+=add[z][x][y];
			}

		}
	}

}

void three_d_mul_gdouble(struct device *in, gdouble ***src, gdouble val)
{
int x=0;
int y=0;
int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{
		for (x = 0; x < in->xmeshpoints; x++)
		{
			for (y = 0; y < in->ymeshpoints; y++)
			{
				src[z][x][y]*=val;
			}

		}
	}

}

long double three_d_avg(struct device *in, long double ***src)
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
				sum+=src[z][x][y]*in->dxmesh[x]*in->dymesh[y]*in->dzmesh[z];
//				printf("%Le %Le %Le %Le %Le %Le\n",in->dxmesh[x],in->dymesh[y],in->dzmesh[z],in->zlen,in->xlen,in->ylen);
			}

		}
	}

ret=sum/(in->zlen*in->xlen*in->ylen);
//printf("ret=%Le\n",ret);
return ret;
}

void three_d_printf(struct device *in, long double ***src)
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
				printf("%Le\n",src[z][x][y]);
			}

		}
	}

return;
}

long double three_d_avg_fabsl(struct device *in, long double ***src)
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
				sum+=fabsl(src[z][x][y])*in->dxmesh[x]*in->dymesh[y]*in->dzmesh[z];
			}

		}
	}

ret=sum/(in->zlen*in->xlen*in->ylen);
return ret;
}

long double three_d_integrate(struct device *in, long double ***src)
{
int x=0;
int y=0;
int z=0;
long double sum=0.0;
	for (z = 0; z < in->zmeshpoints; z++)
	{
		for (x = 0; x < in->xmeshpoints; x++)
		{
			for (y = 0; y < in->ymeshpoints; y++)
			{
				sum+=src[z][x][y]*in->dxmesh[x]*in->dymesh[y]*in->dzmesh[z];
			}

		}
	}

return sum;
}

void free_3d_gdouble(struct device *in, gdouble ***var)
{
	int x=0;
	int y=0;
	int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{

		for (x = 0; x < in->xmeshpoints; x++)
		{
			free(var[z][x]);
		}
		free(var[z]);
	}

	free(var);

}

void malloc_zx_gdouble(struct device *in, gdouble * (**var))
{
	int z=0;

	*var = (gdouble **) malloc(in->zmeshpoints * sizeof(gdouble *));

	for (z = 0; z < in->zmeshpoints; z++)
	{
		(*var)[z] = (gdouble *) malloc(in->xmeshpoints * sizeof(gdouble));
		memset((*var)[z], 0, in->xmeshpoints * sizeof(gdouble));
	}

}

void free_zx_gdouble(struct device *in, gdouble **var)
{
	int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{
		free(var[z]);
	}

	free(var);
}

void malloc_zx_int(struct device *in, int * (**var))
{
	int z=0;

	*var = (int **) malloc(in->zmeshpoints * sizeof(int *));

	for (z = 0; z < in->zmeshpoints; z++)
	{
		(*var)[z] = (int *) malloc(in->xmeshpoints * sizeof(int));
		memset((*var)[z], 0, in->xmeshpoints * sizeof(int));
	}

}

void free_zx_int(struct device *in, int **var)
{
	int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{
		free(var[z]);
	}

	free(var);
}


void malloc_3d_int(struct device *in, int * (***var))
{
	int x=0;
	int y=0;
	int z=0;


	*var = (int ***) malloc(in->zmeshpoints * sizeof(int **));

	for (z = 0; z < in->zmeshpoints; z++)
	{
		(*var)[z] = (int **) malloc(in->xmeshpoints * sizeof(int*));
		for (x = 0; x < in->xmeshpoints; x++)
		{
			(*var)[z][x] = (int *) malloc(in->ymeshpoints * sizeof(int));
			memset((*var)[z][x], 0, in->ymeshpoints * sizeof(int));
		}
	}

}

void free_3d_int(struct device *in, int ***var)
{
	int x=0;
	int y=0;
	int z=0;

	for (z = 0; z < in->zmeshpoints; z++)
	{

		for (x = 0; x < in->xmeshpoints; x++)
		{
			free(var[z][x]);
		}
		free(var[z]);
	}

	free(var);

}

void malloc_srh_bands(struct device *in, gdouble * (****var))
{
	int x=0;
	int y=0;
	int z=0;

	*var = (gdouble ****) malloc(in->zmeshpoints * sizeof(gdouble ***));

	for (z = 0; z < in->zmeshpoints; z++)
	{
		(*var)[z] = (gdouble ***) malloc(in->xmeshpoints * sizeof(gdouble**));
		for (x = 0; x < in->xmeshpoints; x++)
		{
			(*var)[z][x] = (gdouble **) malloc(in->ymeshpoints * sizeof(gdouble*));
			for (y = 0; y < in->ymeshpoints; y++)
			{
				if (in->srh_bands != 0)
				{
					(*var)[z][x][y] = (gdouble *) malloc(in->srh_bands * sizeof(gdouble));
					memset((*var)[z][x][y], 0, in->srh_bands * sizeof(gdouble));
				}else
				{
					(*var)[z][x][y] = NULL;
				}
			}

		}
	}



}
