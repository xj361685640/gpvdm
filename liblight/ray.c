//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

#include <stdio.h>
#include <ray.h>
#include <const.h>
#include <math.h>

void image_init(struct image *in)
{
	in->lines=0;
	in->nrays=0;
	in->objects=0;
	in->n_start_rays=0;
}

int between(double v, double x0, double x1)
{
	double min;
	double max;
	if (x0>x1)
	{
		min=x1;
		max=x0;
	}else
	{
		min=x0;
		max=x1;
	}
	
	if ((v-x0)>=-1e-12)
	{
		if ((v-x1)<=1e-12)
		{
			return 0;
		}
	}

	return -1;
}

void add_plane(struct image *in, double x0,double y0,double x1,double y1,int id,int edge)
{
	in->p[in->lines].xy0.x=x0;
	in->p[in->lines].xy1.x=x1;
	in->p[in->lines].xy0.y=y0;
	in->p[in->lines].xy1.y=y1;
	in->p[in->lines].edge=edge;
	in->p[in->lines].id=id;
	in->lines++;
}


void ray_reset(struct image *in)
{
	in->nrays=0;
}

void add_ray(struct image *in,struct vec *start,struct vec *dir,double mag)
{

	if (mag>1e-3)
	{
		vec_cpy(&(in->rays[in->nrays].xy),start);
		vec_cpy(&(in->rays[in->nrays].dir),dir);
		in->rays[in->nrays].state=WAIT;
		in->rays[in->nrays].bounce=0;
		in->rays[in->nrays].mag=mag;
		in->nrays++;
	}
		
}


void dump_plane_to_file(struct image *in)
{
	FILE *out=fopen("lines.out","w");
	int i=0;

	for (i=0;i<in->lines;i++)
	{
		fprintf(out,"%le %le\n",in->p[i].xy0.x,in->p[i].xy0.y);
		fprintf(out,"%le %le\n",in->p[i].xy1.x,in->p[i].xy1.y);
		fprintf(out,"\n");
		fprintf(out,"\n");

	}

	fclose(out);

	out=fopen("light.out","a");

	for (i=0;i<in->nrays;i++)
	{
		if (in->rays[i].state==DONE)
		{
			fprintf(out,"%le %le\n",in->rays[i].xy.x,in->rays[i].xy.y);
			fprintf(out,"%le %le\n",in->rays[i].xy_end.x,in->rays[i].xy_end.y);
			fprintf(out,"\n");
		}
		
	}

	fclose(out);
	
	out=fopen("start.out","w");
	for (i=0;i<in->n_start_rays;i++)
	{
		fprintf(out,"%le %le\n\n",in->start_rays[i].x,in->start_rays[i].y);
	}
	fclose(out);
	
}

void dump_plane(struct image *in)
{
	int i=0;
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	for (i=0;i<in->n_start_rays;i++)
	{
		printf("%le %le\n",in->start_rays[i].x,in->start_rays[i].y);
	}

	printf("lines:\n");
	for (i=0;i<in->lines;i++)
	{
		printf("%le %le %le %le %d\n",in->p[i].xy0.x,in->p[i].xy0.y,in->p[i].xy1.x,in->p[i].xy1.y,in->p[i].edge);


	}

	printf("rays x,y,x_vec,y_vec:\n");


	for (i=0;i<in->nrays;i++)
	{
		printf("%ld (%le,%le) (%le,%le) %lf %lf mag=%lf\n",in->rays[i].state,in->rays[i].xy.x,in->rays[i].xy.y,in->rays[i].xy_end.x,in->rays[i].xy_end.y,in->rays[i].dir.x,in->rays[i].dir.y,in->rays[i].mag);
		
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

}


void obj_norm(struct vec *ret,struct plane *my_obj)
{
	double norm=0.0;
	vec_cpy(ret,&(my_obj->xy0));
	vec_sub(ret,&(my_obj->xy1));
	vec_swap(ret);
	norm=vec_fabs(ret);
	vec_div(ret,norm);
}


int ray_intersect(struct vec *ret,struct plane *my_obj,struct ray *my_ray)
{
double x1=my_obj->xy0.x;
double x2=my_obj->xy1.x;
double x3=my_ray->xy.x;
double x4=my_ray->xy.x+my_ray->dir.x;

double y1=my_obj->xy0.y;
double y2=my_obj->xy1.y;
double y3=my_ray->xy.y;
double y4=my_ray->xy.y+my_ray->dir.y;

double x12 = x1 - x2;

double x34 = x3 - x4;
double y12 = y1 - y2;
double y34 = y3 - y4;

double c = x12 * y34 - y12 * x34;
double x=0.0;
double y=0.0;
int hit=0;
if (fabs(c) < 1e-10)
{
  
}
else
{
	double a = x1 * y2 - y1 * x2;
	double b = x3 * y4 - y3 * x4;

	x = (a * x34 - b * x12) / c;
	y = (a * y34 - b * y12) / c;

	//printf("here  x0=%le x=%le %le x1=%le %d\n",my_obj->xy0.x,x,my_obj->xy1.x,my_ray->xy.x,between(x,my_obj->xy0.x,my_obj->xy1.x));
	//getchar();
	if (between(x,my_obj->xy0.x,my_obj->xy1.x)==0)
	{
		//printf("here0\n");

		if (between(y,my_obj->xy0.y,my_obj->xy1.y)==0)
		{
			//printf("here1\n");
			if (my_ray->dir.y>=0)
			{
				if (y>my_ray->xy.y)
				{
					hit=1;
				}else
				{
					hit=0;
				}
			}

			if (my_ray->dir.y<=0)
			{
				if (y<my_ray->xy.y)
				{
					hit=1;
				}else
				{
					hit=0;
				}
			}


			if (my_ray->dir.x>0)
			{
				if (x>my_ray->xy.x)
				{
					hit=1;
				}else
				{
					hit=0;
				}
			}

			if (my_ray->dir.x<0)
			{
				if (x<my_ray->xy.x)
				{
					hit=1;
				}else
				{
					hit=0;
				}
			}
		}
	}
}


ret->x=x;
ret->y=y;
//printf("exit %le %le %d\n",x,y,hit);
return hit;
}

int search_ojb(struct image *in,int ray)
{
int i;
int found=-1;
int pos=-1;

double min_dist=1000.0;
double dist=0;

struct vec ret;
vec_init(&ret);

struct vec tmp;
vec_init(&tmp);

struct vec store;
vec_init(&store);

	for (i=0;i<in->lines;i++)
	{
		found=ray_intersect(&ret,&(in->p[i]),&(in->rays[ray]));
		printf("check %d %d %d\n",i,ray,found);

		if (found==1)
		{
			vec_cpy(&tmp,&ret);
			//vec_print(&ret);
			//getchar();
			//vec_print(&(in->rays[ray].xy));			
			vec_sub(&tmp,&(in->rays[ray].xy));
			dist=vec_fabs(&tmp);
			//printf("dist=%lf\n",dist);
			if (dist>1e-12)
			{
				if (dist<min_dist)
				{
					pos=i;
					vec_cpy(&(in->rays[ray].xy_end),&ret);
					min_dist=dist;
				}
			}
		}
		
	}

//printf("chosen %d\n",pos);
return pos;	
}

int activate_rays(struct image *in)
{
	int i=0;
	int changed=0;
	for (i=0;i<in->nrays;i++)
	{
		if (in->rays[i].state==WAIT)
		{
			in->rays[i].state=READY;
			changed++;
		}
		
	}

return changed;
}

int pnpoly(struct image *in, struct vec *xy,int id)
{
	int c=1;
	int i=0;
	double x=xy->x;
	double y=xy->y;
	for (i=0;i<in->lines;i++)
	{
		if (in->p[i].id==id)
		{
			if (between(y,in->p[i].xy0.y,in->p[i].xy1.y)==0)
			{
				if (x < (in->p[i].xy1.x - in->p[i].xy0.x) * (y - in->p[i].xy0.y) / (in->p[i].xy1.y - in->p[i].xy0.y) + in->p[i].xy0.x)
				{
					c = !c;
				}
					
			}
		}
		
	}
	
	return c;
}

void get_refractive(struct image *in,double *n0,double *n1,int ray)
{
	struct vec tmp;
	vec_init(&tmp);

	struct vec back;
	vec_init(&back);

	struct vec fwd;
	vec_init(&fwd);

	vec_cpy(&tmp,&(in->rays[ray].dir));
	vec_mul(&tmp,-1e-12);
	vec_cpy(&back,&(in->rays[ray].xy_end));
	vec_add(&back,&tmp);

	vec_cpy(&tmp,&(in->rays[ray].dir));
	vec_mul(&tmp,1e-12);
	vec_cpy(&fwd,&(in->rays[ray].xy_end));
	vec_add(&fwd,&tmp);
	//vec_print(&fwd);
	//vec_print(&back);

	int i;
	int i_fwd=-1;
	int i_back=-1;
	
	for (i=in->objects-1;i>=0;i--)
	{
		if (pnpoly(in, &back,i)==0)
		{
			i_back=i;
			break;
		}
	}

	for (i=in->objects-1;i>=0;i--)
	{
		if (pnpoly(in, &fwd,i)==0)
		{
			i_fwd=i;
			break;
		}
	}
	

	if ((i_fwd!=-1))
	{
		*n1=in->obj_n[i_fwd];
	}

	if (i_back!=-1)
	{
		*n0=in->obj_n[i_back];
	}

}
    
int propergate_next_ray(struct image *in)
{
	struct vec n;
	vec_init(&n);

	struct vec n_inv;
	vec_init(&n_inv);
	
	struct vec r;
	vec_init(&r);

	struct vec t;
	vec_init(&t);

	struct vec temp;
	vec_init(&temp);

	double threshold=0.0;
	double ang_out=0.0;
	int ray=0;

	double R=0.0;
	double T=0.0;
	double mag=0.0;
	
	for (ray=0;ray<in->nrays;ray++)
	{
		if (in->rays[ray].state==READY)
		{
			
			in->rays[ray].state=DONE;

			int item=search_ojb(in,ray);
			if (item==-1)
			{
				printf("I have not hit anything\n");
				dump_plane_to_file(in);
				exit(0);
			}
			printf("intersect %d %le %le\n",item,in->rays[ray].xy_end.x,in->rays[ray].xy_end.y);
			//getchar();
			//printf("norm=\n");

			int bounce=in->rays[ray].bounce;
			mag=in->rays[ray].mag;
			bounce=bounce+1;

			double n0=1.0;
			double n1=1.0;
			get_refractive(in,&n0,&n1,ray);
			//printf("back index=%lf fwd index=%lf\n",n0,n1);

			//Calculate norm of object
			obj_norm(&n,&(in->p[item]));
			vec_cpy(&n_inv,&n);
			vec_mul(&n_inv,-1.0);

			//vec_print(&n);

			////Cal normal to surface
			double a=vec_overlap(&n,&(in->rays[ray].dir));
			vec_mul(&n,-1.0);
			vec_mul(&n_inv,-1.0);
			double b=vec_overlap(&n,&(in->rays[ray].dir));

			if (a<b)
			{
				vec_mul(&n,-1.0);
				vec_mul(&n_inv,-1.0);

			}

			/////////////Calculate reflected ray.
			double ret=2.0*vec_dot(&(in->rays[ray].dir),&n);

			vec_cpy(&temp,&n);
			vec_mul(&temp,ret);

			vec_cpy(&r,&(in->rays[ray].dir));
			vec_sub(&r,&temp);
			//////Angle between inceident ray and surface
			double dot=0.0;
			double ang_in=0.0;
			dot=vec_dot(&n_inv,&(in->rays[ray].dir));
			ang_in=PI/2.0-acos(dot);

			
			//printf("in=%lf\n",deg(ang_in));
			//

			if (n1>n0)
			{
				threshold=0.0;
			}else
			{
				threshold=asin(n1/n0);
			}

			///////////////Calculate Snell's law in vector form.
			struct vec left;
			vec_init(&left);
			vec_cross(&left,&n,&(in->rays[ray].dir));
			vec_mul(&left,-1.0);
			vec_cross(&left,&n,&left);
			vec_mul(&left,n0/n1);
			
			struct vec cr;
			vec_init(&cr);
			vec_cross(&cr,&n,&(in->rays[ray].dir));

			dot=0.0;
			dot=vec_dot(&cr,&cr);
			dot=dot*(pow((n0/n1),2.0));
			dot=sqrt(1.0-dot);

			struct vec right;
			vec_init(&right);
			vec_cpy(&right,&n);
			vec_mul(&right,dot);
			vec_sub(&left,&right);
			
			vec_cpy(&t,&left);


			//////Out angle

			if (ang_in>threshold)
			{
				dot=vec_dot(&n_inv,&t);
				ang_out=PI/2.0-acos(dot);

				R=((n0*cos(PI/2.0-ang_in)-n1*cos(PI/2.0-ang_out))/(n0*cos(PI/2.0-ang_in)+n1*cos(PI/2.0-ang_out)));
				R=fabs(R);
				T=1.0-R;

			}else
			{
				ang_out=-100.0;
				R=1.0;
				T=0.0;
			}

			//printf("ang_in=%lf %lf\n",deg(ang_in),deg(threshold));
			//printf("ang_out=%lf\n",deg(ang_out));
			//printf("T=%lf R=%lf\n",T,R);
			//getchar();
			//////////////////////////////


			//vec_print(&r);

				if (bounce<100)
				{
					if (in->p[item].edge==FALSE)
					{
						add_ray(in,&(in->rays[ray].xy_end),&r,R*mag);
						in->rays[in->nrays-1].bounce=bounce;

						//printf("ang_in=%lf threshold=%lf %lf %lf\n",ang_in,threshold,n1,n0);
						//getchar();
						if (ang_in>=threshold)
						{
							add_ray(in,&(in->rays[ray].xy_end),&(t),T*mag);
							in->rays[in->nrays-1].bounce=bounce;
						}	
							
					}
				}	

			}
	}

return 0;
}

void add_box(struct image *in,double start_x,double start_y,double x_len,double y_len,double n,int sim_edge)
{
	add_plane(in,start_x,start_y,start_x+x_len,start_y,in->objects,sim_edge);	
	add_plane(in,start_x+x_len,start_y,start_x+x_len,start_y+y_len,in->objects,sim_edge);
	add_plane(in,start_x,start_y+y_len,start_x+x_len,start_y+y_len,in->objects,sim_edge);
	add_plane(in,start_x,start_y,start_x,start_y+y_len,in->objects,sim_edge);
	in->obj_n[in->objects]=n;
	in->objects++;
}

double get_eff(struct image *in)
{
int i;
double tot=0.0;
	for (i=0;i<in->nrays;i++)
	{
		if (in->rays[i].state==DONE)
		{
			if (in->rays[i].xy_end.y==0.0)
			{
				tot+=in->rays[i].mag;
			}
		}
		
	}

return tot;
}
