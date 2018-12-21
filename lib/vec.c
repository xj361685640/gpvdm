//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


/** @file vec.c
	@brief Basic vector manipulation routines
*/

#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE
#include <stdio.h>
#include <math.h>
#include "const.h"
#include "vec.h"


///Return |Vector|
double vec_mod(struct vec *my_vec)
{
return sqrt(pow(my_vec->x,2.0)+pow(my_vec->y,2.0)+pow(my_vec->z,2.0));
}

///Make all vector component signs positive
void vec_plus(struct vec *my_vec1)
{
if (my_vec1->x<0) my_vec1->x*= -1.0;
if (my_vec1->y<0) my_vec1->y*= -1.0;
if (my_vec1->z<0) my_vec1->z*= -1.0;
}


///Print a vector to a file
void vec_fprintf(FILE *out,struct vec *my_vec)
{
	fprintf(out,"%lf %lf %lf\n",my_vec->x,my_vec->y,my_vec->z);
}

///Convert from rad to deg
double deg(double in)
{
	return in*360.0/(2.0*PI);
}

double vec_ang(struct vec *in_v0,struct vec *in_v1)
{
	struct vec v0;
	struct vec v1;

	vec_cpy(&v0,in_v0);
	vec_cpy(&v1,in_v1);

	vec_norm(&v0);
	vec_norm(&v1);
	double dot=vec_dot(&v0,&v1);
	double ang=acos(dot);

	if (ang>PI/2.0)
	{
		ang-=PI/2.0;
	}

	return ang;
}

///Subtract two vectors
void vec_sub(struct vec *my_vec1,struct vec *my_vec2)
{
my_vec1->x-=my_vec2->x;
my_vec1->y-=my_vec2->y;
my_vec1->z-=my_vec2->z;
}


///Add two vectors together
void vec_add(struct vec *my_vec1,struct vec *my_vec2)
{
my_vec1->x+=my_vec2->x;
my_vec1->y+=my_vec2->y;
my_vec1->z+=my_vec2->z;
}

///Divide one vector by a double
void vec_div(struct vec *my_vec1,double n)
{
my_vec1->x/=n;
my_vec1->y/=n;
my_vec1->z/=n;
}

void vec_mul(struct vec *my_vec1,double n)
{
my_vec1->x*=n;
my_vec1->y*=n;
my_vec1->z*=n;
}


double vec_fabs(struct vec *my_vec)
{
return sqrt(pow(my_vec->x,2.0)+pow(my_vec->y,2.0)+pow(my_vec->z,2.0));
}

void vec_rotate(struct vec *my_vec,double ang)
{
double x=0.0;
double y=0.0;

x=my_vec->x*cos(ang)-my_vec->y*sin(ang);
y=my_vec->x*sin(ang)+my_vec->y*cos(ang);
my_vec->x=x;
my_vec->y=y;

}

///Perform a dot product between two vectors
double vec_dot(struct vec *a,struct vec *b)
{
return (a->x*b->x)+(a->y*b->y)+(a->z*b->z);
}

void vec_init(struct vec *my_vec)
{
	my_vec->x=0.0;
	my_vec->y=0.0;
	my_vec->z=0.0;
}

///Perform the cross product of two vectors
void vec_cross(struct vec *ret,struct vec *a,struct vec *b)
{
double x=0.0;
double y=0.0;
double z=0.0;

x=(a->y*b->z)-(a->z*b->y);
y=(a->z*b->x)-(a->x*b->z);
z=(a->x*b->y)-(a->y*b->x);

ret->x=x;
ret->y=y;
ret->z=z;

}

void vec_swap(struct vec *my_vec)
{
double temp=0.0;
temp=my_vec->x;
my_vec->x=my_vec->y;
my_vec->y=temp;
}

double vec_dist(struct vec *a,struct vec *b)
{
	return sqrt(pow(a->x-b->x,2.0)+pow(a->y-b->y,2.0)+pow(a->z-b->z,2.0));
}

///Copy a vector
void vec_cpy(struct vec *my_vec1,struct vec *my_vec2)
{
my_vec1->x=my_vec2->x;
my_vec1->y=my_vec2->y;
my_vec1->z=my_vec2->z;
}

///Normalize a vector
void vec_norm(struct vec *my_vec)
{
	double mag=vec_fabs(my_vec);
	my_vec->x/=mag;
	my_vec->y/=mag;
	my_vec->z/=mag;
}

///Print a vector to stdout
void vec_print(struct vec *my_vec)
{
	//printf_log(sim,"%lf %lf %lf\n",my_vec->x,my_vec->y,my_vec->z);
}

///Set a vector
void vec_set(struct vec *my_vec,double x, double y, double z)
{
my_vec->x=x;
my_vec->y=y;
my_vec->z=z;
}

double overlap(double x0,double x1)
{
	double a=x0;
	double b=x1;
	
	if (a>b)
	{
		a=x1;
		b=x0;
	}

	if ((a<=0.0)&&(b>=0.0))
	{
		return 0;
	}else
	if ((a<0.0)&&(b<0.0))
	{
		return fabs(b);
	}else
	if ((a>=0.0)&&(b>=0.0))
	{
		return fabs(a);
	}

	return -2.0;

}

double vec_overlap(struct vec *a,struct vec *b)
{
	return overlap(a->x,b->x)+overlap(a->y,b->y)+overlap(a->z,b->z);
}

/** @brief Basic vector manipulation routines
this function needs four points and will calculate the dihedral angle

 a            d
  \          /
   \        /
    b------c
*/
double vec_get_dihedral(struct vec *a,struct vec *b,struct vec *c,struct vec *d)
{
double cos_;
	struct vec b1;
	struct vec b2;
	struct vec b3;
	struct vec b23;
	struct vec b12;

	vec_cpy(&b1,b);
	vec_sub(&b1,a);

	vec_cpy(&b2,c);
	vec_sub(&b2,b);

	vec_cpy(&b3,d);
	vec_sub(&b3,c);

	vec_cross(&b23,&b2,&b3);
	vec_cross(&b12,&b1,&b2);

	double mb2=vec_mod(&b2);
	double aa=vec_dot(&b1,&b23);

	double cross=0.0;;
	cross=vec_dot(&b12,&b23);

	cos_=(atan2(mb2*aa,cross)/(3.1415026))*180.0;
	if (cos_<0) cos_=360-sqrt(pow(cos_,2.0));

return cos_;
}

/**Rotate a vector around an arbitrary axis
@arg in   Input vector
@arg unit A unit vector by around which the vector should be rotated
@arg base ??
@arg ang  Angle by which the rotation should be performed
*/

void vec_rot(struct vec *in,struct vec *unit,struct vec *base, double ang)
{
struct vec temp;
struct vec rot;

double c;
double s;

vec_cpy(&rot,in);
vec_sub(&rot,base);
c=cos(ang);
s=sin(ang);

temp.x=rot.x*(unit->x*unit->x*(1.0-c)+c) + rot.y*(unit->x*unit->y*(1.0-c)-unit->z*s)	+ rot.z*(unit->x*unit->z*(1.0-c)+unit->y*s);
temp.y=rot.x*(unit->x*unit->y*(1-c)+unit->z*s)	+ rot.y*(unit->y*unit->y*(1-c)+c)	+ rot.z*(unit->y*unit->z*(1.0-c)-unit->x*s);
temp.z=rot.x*(unit->x*unit->z*(1.0-c)-unit->y*s) + rot.y*(unit->y*unit->z*(1.0-c)+unit->x*s) + rot.z*(unit->z*unit->z*(1.0-c)+c);

vec_add(&temp,base);
vec_cpy(in,&temp);
}

///Add to a vector
void vec_add_values(struct vec *my_vec, double x,double y, double z)
{
my_vec->x+=x;
my_vec->y+=y;
my_vec->z+=z;
}

///Compare two vector
int vec_cmp(struct vec *my_vec1,struct vec *my_vec2)
{
struct vec temp;
vec_cpy(&temp,my_vec1);
vec_sub(&temp,my_vec2);
double test=vec_mod(&temp);
if (test<1e-6) return 0;
return 1;
}



////////////////Old vector code///////////////

/*void rotx_vec(struct vec *out, struct vec *in,double a)
{
	out->x=in->atom[i].x*1.0 + in->atom[i].y*0.0 +in->atom[i].z*0.0;
	out->y=in->atom[i].x*0.0 + in->atom[i].y*cos(a) +in->atom[i].z*(-sin(a));
	out->z=in->atom[i].x*0.0 + in->atom[i].y*sin(a) +in->atom[i].z*cos(a);
}

void roty_vec(struct vec *out, struct vec *in,double a)
{
	out->x=in->atom[i].x*cos(a)    + in->atom[i].y*0.0 + in->atom[i].z*sin(a);
	out->y=in->atom[i].x*0.0       + in->atom[i].y*1.0 + in->atom[i].z*0.0;
	out->z=in->atom[i].x*(-sin(a)) + in->atom[i].y*0.0 + in->atom[i].z*cos(a);
}

void rotz_vec(struct vec *out, struct vec *in,double a)
{
	out->x=in->atom[i].x*cos(a) + in->atom[i].y*(-sin(a)) +in->atom[i].z*0.0;
	out->y=in->atom[i].x*sin(a) + in->atom[i].y*cos(a) +in->atom[i].z*0.0;
	out->z=in->atom[i].x*0.0 + in->atom[i].y*0.0 +in->atom[i].z*1.0;
}*/



///Calculate the smallest angle between two vectors
double ang_vec(struct vec *one,struct vec *two)
{
	double dot;
	double cos_;

	dot=vec_dot(one,two);


	cos_=(acos(dot/(vec_mod(one)*vec_mod(two)))/(3.1415026))*180.0;
	//if (dot<0) cos_=360.0-cos_;
	return cos_;
}


