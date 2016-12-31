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



#ifndef h_dump
#define h_dump
#include "frame.h"
#include "device.h"
#include "dump_ctrl.h"
#include "dynamic_store.h"
#include "buffer.h"
#include <sim_struct.h>

void dump_init(struct simulation *sim,struct device* in);
void dump_load_config(struct simulation* sim,struct device *in);
void dump_remove_snapshots(struct device *in);
void dump_dynamic_init(struct simulation *sim,struct dynamic_store *store,struct device *in);
void dump_dynamic_save(struct simulation *sim,char *outputpath,struct dynamic_store *store);
void dump_dynamic_add_data(struct simulation *sim,struct dynamic_store *store,struct device *in, gdouble x_value);
void dump_dynamic_free(struct simulation *sim,struct device *in,struct dynamic_store *store);
void dump_build_2d_charge_frame(struct map *mapin_e,struct map *mapin_h,struct device *in);
void dump_write_2d_charge_map(struct map *in_e,struct map *in_h,struct device *dev);
void frame_add_data(struct map *in,gdouble x,gdouble y,gdouble data);
void dump_slice(struct device *in,char *prefix);
void dump_energy_slice(struct simulation *sim,char *out_dir,struct device *in);
void dump_write_2d_charge_single_map(struct map *in,struct device *dev);
void dump_build_2d_charge_single_frame(struct map *mapin,struct device *in);
void dump_device_map(struct simulation *sim,char* out_dir,struct device *in);
void dump_1d_slice(struct simulation *sim,struct device *in,char *out_dir);
void dump_write_to_disk(struct simulation *sim,struct device* in);
void buffer_add_3d_device_data(struct buffer *buf,struct device *in, gdouble ***data);
void buffer_set_graph_type(struct buffer *buf,struct device *in);
void dumpfiles_load(struct simulation* sim);
void dumpfiles_free(struct simulation* sim);
int dumpfiles_should_dump(struct simulation* sim,char *name);
void dumpfiles_process(struct simulation* sim,struct istruct *in,char *name);
#endif
