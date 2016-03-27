#!/bin/bash -e
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
#	www.gpvdm.com
#	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License v2.0, as published by
#    the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

mkdir pub
cp go.o ./pub/gpvdm_core.exe
cp ./libeay32.dll ./pub/
cp ./libgcc_s_sjlj-1.dll ./pub/
cp ./umfpack.dll ./pub/
cp ./crypto.dll ./pub/
cp ./zlib1.dll ./pub/
cp ./libzip-2.dll ./pub/


mkdir ./pub/materials
cp ./materials/*.spectra ./pub/materials/

for i in ito npb generic_organic generic_metal pedotpss p3htpcbm al ag au c60 p3ht air cds pcbm si zno cigs; do

	cp ./materials/$i ./pub/materials/ -r

done


cp ./device_lib ./pub/device_lib -r

#python -m py_compile ./gui/*.py

mkdir ./pub/images
cp ./images/*.ico ./pub/images/
cp ./images/*.jpg ./pub/images/
cp ./images/*.png ./pub/images/
cp ./images/*.svg ./pub/images/

mkdir ./pub/gui
cp ./gui/*.py ./pub/gui/

cp ./sim.gpvdm ./pub/

mkdir ./pub/light
cp ./light/*.dll ./pub/light/

mkdir ./pub/solvers
cp ./solvers/*.dll ./pub/solvers/


cp ./lang ./pub/lang -rf
cp ./update.sh ./pub/

cp pub ~/windows/share/ -rf






