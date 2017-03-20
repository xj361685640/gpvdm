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

#to compile C
apt-get install gcc autoconf make libtool automake libzip libzip-devel suitesparse-devel help2man gsl-devel gettext-tools dbus-1-devel zlib-devel gnuplot

#for gui
apt-get install python-qt5-utils python3-qt5-devel python3-qt5 python3-pycrypto python3-numpy python3-matplotlib python3-matplotlib-qt-shared python3-psutil python3-openpyxl python3-opengl texlive

#building rpm
apt-get install rpmbuild unifdef
