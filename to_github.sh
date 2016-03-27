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

kiallmy_dir=`pwd`

dest=/home/rod/webpage/git/gpvdm/


read -p "What happened : " message

make clean

rm $dest/*.c -f
rm $dest/*.inp -f
rm $dest/makefile -f
rm $dest/include -rf
rm $dest/solvers -rf
rm $dest/light -rf
rm $dest/device_lib -rf
rm $dest/lib -rf
rm $dest/libdos -rf
rm $dest/libdump -rf
rm $dest/liblight -rf
rm $dest/libmeasure -rf
rm $dest/libmesh -rf
rm $dest/libserver -rf

cp ./* $dest -rf 

cd $dest
for i in `find|grep -v .git|grep -v .o$|grep -v ~$|grep -v materials|grep -v dll$|grep -v .so$`
do
git add $i
done
git commit -m "$message"
git push origin master --force
cd $my_dir
