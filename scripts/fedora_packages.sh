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

yum install texlive ghostscript ImageMagick python-inotify vte mencoder pywebkitgtk python-matplotlib unifdef indent numpy zlib-devel libzip-devel suitesparse-devel openssl-devel gsl-devel libcurl-devel blas-devel help2man librsvg2-tools

yum install libmatheval-devel valgrind @development-tools fedora-packager pciutils-devel python-crypto python-awake mingw32-gcc

yum install suitesparse-devel openssl-libs openssl-libs-devel openssl-devel libzip libzip-devel gsl-devel blas-devel numpy notify-python python-inotify.noarch python-matplotlib webkitgtk-devel pywebkitgtk

yum install unifdef indent libcurl-devel python2-matplotlib-gtk poedit

