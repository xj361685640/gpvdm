#!/bin/bash
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

#Electrical plugins
platform=$2
CFLAGS=$1
if [ "$platform" = "windows" ]; then
cp ~/windll/compiled_dlls/*.dll ./
pwd
echo i686-w64-mingw32-windres ./images/gpvdm.rc -o ./images/res.o
i686-w64-mingw32-windres ./images/gpvdm.rc -o ./images/res.o
fi

mydir=`pwd`
for i in lib libdos libdump liblight libserver libmeasure libmesh; do
	cd $i
	make platform="$2" CC="$3" LD="$4" -j4
	cd $mydir
done


#Optical plugins
for i in `find ./light/ |grep light.c`; do
newdir=`dirname $i`
echo "Building optical plugin" $newdir
cd $newdir
echo make CFLAGS="$CFLAGS" platform="$2" CC="$3" LD="$4"
make CFLAGS="$CFLAGS" platform="$2" CC="$3" LD="$4"
nowdir=`pwd`
curname=`basename $nowdir`
echo $platform
if [ "$platform" = "linux" ]; then
cp plugin.so ../${curname}.so
else
cp plugin.dll ../${curname}.dll
fi

if [ $? -ne 0 ]; then
	exit
fi
cd $mydir

done

#solver plugins
for i in `find ./solvers/ |grep makefile`; do
newdir=`dirname $i`
echo "Building solver plugin" $newdir
cd $newdir
echo make platform="$2" CC="$3" LD="$4"
make platform="$2" CC="$3" LD="$4"
nowdir=`pwd`
curname=`basename $nowdir`
echo $platform
if [ "$platform" = "linux" ]; then
cp plugin.so ../${curname}.so
else
cp plugin.dll ../${curname}.dll
fi

if [ $? -ne 0 ]; then
	exit
fi
cd $mydir

done
