//    Organic Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for organic solar cells. 
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; version 2 of the License
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <util.h>
#include "newton.h"
#include "../../dll_interface.h"
#include "../../solver_interface.h"
#include "../../dll_export.h"

struct dll_interface *fun;

EXPORT void set_interface(struct dll_interface *in)
{
	fun = in;
}

EXPORT void dll_newton_set_min_ittr(int ittr)
{
	dllinternal_newton_set_min_ittr(ittr);
}

EXPORT int dll_solve_cur(struct device *in)
{
	return dllinternal_solve_cur(in);
}

EXPORT void dll_solver_realloc(struct device *in)
{
	dllinternal_solver_realloc(in);
}

EXPORT void dll_solver_free_memory(struct device *in)
{
	dllinternal_solver_free_memory(in);
}
