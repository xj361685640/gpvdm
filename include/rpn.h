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

/** @file rpn.h
@brief RPN functions which gpvdm can handle.
*/

#ifndef rpn_h
#define rpn_h

struct rpn_function_type
{
	char name[10];
	char* (*f)(char* out,char* a, char* b);
};

struct rpn_opperator_type
{
	char name[10];
	int prec;
	int right_left;
	char* (*f)(char* out,char* a, char* b);

};

struct rpn_vars_type
{
	char name[10];
	double value;
};

struct rpn
{
	char output[10][40];
	int output_pos;

	char stack[10][40];
	int stack_pos;

	struct rpn_vars_type vars[40];
	int vars_pos;


	struct rpn_function_type functions[40];
	int functions_count;

	struct rpn_opperator_type opps[40];
	int opp_count;

	int last_was_number;
	int last_was_e;

};

//vars
void rpn_add_var(struct simulation *sim,struct rpn *in,char *name,double value);
int rpn_is_var(struct simulation *sim,struct rpn *in,char *out,char *name);

//rpn
void rpn_init(struct simulation *sim,struct rpn *in);
void add_var(struct simulation *sim,struct rpn *in,char *name,double value);
void add_function(struct simulation *sim,struct rpn *in,char *name,void *f);
int is_function(struct simulation *sim,struct rpn *in,char *val);
char* function_run(struct simulation *sim,struct rpn *in,char *val,char *out,char* a,char* b);

//opp
void add_opp(struct simulation *sim,struct rpn *in,char *name, int prec, int right_left,void *f);
int is_opp(struct simulation *sim,struct rpn *in,char *val);
char* opp_run(struct simulation *sim,struct rpn *in,char *val,char *out,char* a,char* b);
int opp_pr(struct simulation *sim,struct rpn *in,char *val);
int opp_lr(struct simulation *sim,struct rpn *in,char *val);

//stack
void output_push(struct simulation *sim,struct rpn *in,char *val);
void stack_push(struct simulation *sim,struct rpn *in,char *val);
char* stack_pop(struct simulation *sim,struct rpn *in);
char* stack_peak(struct simulation *sim,struct rpn *in);
void print_stack(struct simulation *sim,struct rpn *in);
void print_output(struct simulation *sim,struct rpn *in);

int isnumber(char a);


int edge_detect(struct simulation *sim,struct rpn *in,char *buf,char next);
void pro(struct simulation *sim,struct rpn *in,char *buf,int type);
double rpn_evaluate(struct simulation *sim,struct rpn *in,char *string);

//Functions
char* eval_sin(char *out,char* a,char* b);
char* eval_abs(char *out,char* a,char* b);
char* eval_pos(char *out,char* a,char* b);
char* eval_add(char *out,char* a,char* b);
char* eval_sub(char *out,char* a,char* b);
char* eval_mul(char *out,char* a,char* b);
char* eval_pow(char *out,char* a,char* b);
char* eval_div(char *out,char* a,char* b);
char* eval_log10(char *out,char* a,char* b);
char* eval_bg(char *out,char* a,char* b);
char* eval_sm(char *out,char* a,char* b);
#endif
