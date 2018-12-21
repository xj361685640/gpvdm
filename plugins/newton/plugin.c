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

/** @file plugin.c
	@brief Interface for Newton solver plugin
*/

#include "newton.h"
#include <solver_interface.h>
#include <dll_export.h>
#include <log.h>


EXPORT int dll_solve_cur(struct simulation *sim,struct device *in,int z, int x)
{
return dllinternal_solve_cur(sim,in,z,x);
}

EXPORT void dll_solver_realloc(struct simulation *sim,struct device *in)
{
dllinternal_solver_realloc(sim,in,1);
}

EXPORT void dll_solver_free_memory(struct device *in)
{
dllinternal_solver_free_memory(in);
}

