#!/bin/bash -e
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
#	https://www.gpvdm.com
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
cp gpvdm_core.exe ./pub/gpvdm_core.exe
cp ~/windll/compiled_dlls/*.dll ./pub/
cp ~/windll/opengl_dlls/*.dll ./pub/

mkdir ./pub/materials
cp ./materials/ ./pub/ -rf

mkdir ./pub/spectra
cp ./spectra/ ./pub/ -rf

cp ./device_lib ./pub/ -rf


mkdir ./pub/images
cp ./images/*.ico ./pub/images/
cp ./images/*.jpg ./pub/images/
cp ./images/*.png ./pub/images/

cp ./images/16x16 ./pub/images/ -rf
cp ./images/32x32 ./pub/images/ -rf
cp ./images/64x64 ./pub/images/ -rf
cp ./images/48x32 ./pub/images/ -rf
cp ./images/splash ./pub/images/ -rf

mkdir ./pub/gui
cp ./gui/*.py ./pub/gui/
cp ./gui/*.bat ./pub/gui/

mkdir ./pub/css
cp ./css/*.css ./pub/css/

mkdir ./pub/html
cp ./html/*.html ./pub/html/

cp ./base.gpvdm ./pub/

mkdir ./pub/plugins
cp ./plugins/*.dll ./pub/plugins/

mkdir ./pub/cluster_
cp ./cluster_/*.zip ./pub/cluster_/

mkdir ./pub/ui
cp ./ui/*.ui ./pub/ui/

cp ./lang ./pub/lang -rf
rm ./pub/lang/*.*

cp ../../../update.sh ./pub/

rm ~/windows/share/pub -rf
cp pub ~/windows/share/ -rf






