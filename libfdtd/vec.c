/** @file vec.c
	@brief Basic vector manipulation routines
*/
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE
//<clean=none></clean>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "vec.h"

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

	cpy_vec(&b1,b);
	sub_vec(&b1,a);

	cpy_vec(&b2,c);
	sub_vec(&b2,b);
	
	cpy_vec(&b3,d);
	sub_vec(&b3,c);

	cros_vec(&b23,&b2,&b3);
	cros_vec(&b12,&b1,&b2);
	
	double mb2=mod_vec(&b2);
	double aa=dot_vec(&b1,&b23);

        double cross=0.0;;
	cross=dot_vec(&b12,&b23);

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

void rot_vec(struct vec *in,struct vec *unit,struct vec *base, double ang)
{
struct vec temp;
struct vec rot;

double c;
double s;

cpy_vec(&rot,in);
sub_vec(&rot,base);
c=cos(ang);
s=sin(ang);
	temp.x=rot.x*(unit->x*unit->x*(1.0-c)+c) + rot.y*(unit->x*unit->y*(1.0-c)-unit->z*s)	+ rot.z*(unit->x*unit->z*(1.0-c)+unit->y*s);
	temp.y=rot.x*(unit->x*unit->y*(1-c)+unit->z*s)	+ rot.y*(unit->y*unit->y*(1-c)+c)	+ rot.z*(unit->y*unit->z*(1.0-c)-unit->x*s);
	temp.z=rot.x*(unit->x*unit->z*(1.0-c)-unit->y*s) + rot.y*(unit->y*unit->z*(1.0-c)+unit->x*s) + rot.z*(unit->z*unit->z*(1.0-c)+c);

add_vec(&temp,base);
cpy_vec(in,&temp);
}


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


///Set a vector
void set_vec(struct vec *my_vec, double x,double y, double z)
{
my_vec->x=x;
my_vec->y=y;
my_vec->z=z;
}

///Add to a vector
void add_to_vec(struct vec *my_vec, double x,double y, double z)
{
my_vec->x+=x;
my_vec->y+=y;
my_vec->z+=z;
}

///Copy a vector
void cpy_vec(struct vec *my_vec1,struct vec *my_vec2)
{
my_vec1->x=my_vec2->x;
my_vec1->y=my_vec2->y;
my_vec1->z=my_vec2->z;
}

///Compare two vector
int cmp_vec(struct vec *my_vec1,struct vec *my_vec2)
{
struct vec temp;
cpy_vec(&temp,my_vec1);
sub_vec(&temp,my_vec2);
//printf ("%lf\n",);
double test=mod_vec(&temp);
if (test<1e-6) return 0;
return 1;
}




///Add two vectors together
void add_vec(struct vec *my_vec1,struct vec *my_vec2)
{
my_vec1->x+=my_vec2->x;
my_vec1->y+=my_vec2->y;
my_vec1->z+=my_vec2->z;
}

///Subtract two vectors
void sub_vec(struct vec *my_vec1,struct vec *my_vec2)
{
my_vec1->x-=my_vec2->x;
my_vec1->y-=my_vec2->y;
my_vec1->z-=my_vec2->z;
}

///Divide one vector by a double
void div_vec(struct vec *my_vec1,double n)
{
my_vec1->x/=n;
my_vec1->y/=n;
my_vec1->z/=n;
}

///Calculate the smallest angle between two vectors
double ang_vec(struct vec *one,struct vec *two)
{
	double dot;
	double cos_;

	dot=dot_vec(one,two);


	cos_=(acos(dot/(mod_vec(one)*mod_vec(two)))/(3.1415026))*180.0;
	//if (dot<0) cos_=360.0-cos_;
	//printf("%lf\n",dot);
	return cos_;
}


///Multiply a vector by a double
void mul_vec(struct vec *my_vec1,double n)
{
my_vec1->x*=n;
my_vec1->y*=n;
my_vec1->z*=n;
}

///Return |Vector|
double mod_vec(struct vec *my_vec)
{
return sqrt(pow(my_vec->x,2.0)+pow(my_vec->y,2.0)+pow(my_vec->z,2.0));
}

///Normalize a vector
void norm_vec(struct vec *my_vec1)
{
double mod=mod_vec(my_vec1);
my_vec1->x/=mod;
my_vec1->y/=mod;
my_vec1->z/=mod;
}

///Make all vector component signs positive
void plus_vec(struct vec *my_vec1)
{
if (my_vec1->x<0) my_vec1->x*=-1.0;
if (my_vec1->y<0) my_vec1->y*=-1.0;
if (my_vec1->z<0) my_vec1->z*=-1.0;
}

///Perform a dot product between two vectors
double dot_vec(struct vec *a,struct vec *b)
{
return (a->x*b->x)+(a->y*b->y)+(a->z*b->z);
}


struct vec null_vec;

///Perform the cross product of two vectors
void cros_vec(struct vec *ret,struct vec *a,struct vec *b)
{
ret->x=(a->y*b->z)-(a->z*b->y);

ret->y=(a->z*b->x)-(a->x*b->z);

ret->z=(a->x*b->y)-(a->y*b->x);

}

///Print a vector to stdout
void print_vec(struct vec *my_vec)
{
	printf("%lf %lf %lf\n",my_vec->x,my_vec->y,my_vec->z);
}

///Print a vector to a file
void fprint_vec(FILE *out,struct vec *my_vec)
{
	fprintf(out,"%lf %lf %lf\n",my_vec->x,my_vec->y,my_vec->z);
}
