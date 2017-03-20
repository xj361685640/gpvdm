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

apt-get upgrade

#Python deps
apt-get install python3 python3-matplotlib python3-pyqt5.qtopengl python3-opengl python3-numpy python3-crypto python3-dbus.mainloop.pyqt5

#C deps
apt-get install libsuitesparse-dev indent unifdef libsuitesparse-dev libssl-dev libedbus-dev libzip-dev  libgsl0-dev libmatheval-dev help2man pluma build-essential imagemagick license-reconcile autoconf codespell librsvg2-bin

#tools
apt-get install rsync pluma build-essential convert imagemagick license-reconcile autoconf python-bashate codespell complexity apt-file pofileSpell gettext-lint inkscape spellintian pep8 i18nspector python-bashate automake

#debian build enviroment
apt-get install dh-virtualenv debhelper
