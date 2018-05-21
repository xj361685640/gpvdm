#
#  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
#  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
# 
#  Copyright (C) 2012-2015 Roderick C. I. MacKenzie
#
#	r.c.i.mackenzie@googlemail.com
#	https://www.gpvdm.com
#	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
#
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.

# spec file for gpvdm

Summary:		General-purpose  solar cell device model (gpvdm)
License:		GPLv2
Name:			gpvdm
Version:		${ver}
Release:		1%{dist}
Source:			http://www.gpvdm.com/gpvdm-${ver}.tar
Url:			http://www.gpvdm.com
Group:			Development/Tools


BuildRequires: suitesparse-devel, zlib-devel, gsl-devel, blas-devel, gnuplot, numpy, python3-matplotlib, texlive, ghostscript, ImageMagick, python3-crypto, python3-matplotlib-qt5, notify-python, python3-pyopengl, python3-qt5-devel, python3, python3-openpyxl

#rpmbuild does not pick up gnuplot because it's called using popen
#there is no arch requirement is it is callued using popen
Requires: gnuplot

%description
General-purpose solar cell device model, is a drift-diffusion/Shockley-Read-Hall
solar cell simulator specifically developed for the simulation of 1st, 2nd and
3rd generation solar cells.  It can simulate light/dark JV curves, charge
extraction data and provide information on average recombination rates (tau)
as would be measured from transient photo-voltage experiments.



%prep
%setup -q

%configure
./configure

%build
make %{?_smp_mflags} OPT_FLAGS="%{optflags}" OPT_ARCH=%{_arch}

%install
make  DESTDIR=%{buildroot} prefix=%{_prefix} bindir=%{_bindir} libdir=%{_libdir} mandir=%{_mandir} docdir=%{_docdir} appicondir=%{_datadir}/icons/gnome/scalable/mimetypes/ install



%files
%{_bindir}/gpvdm
%{_bindir}/gpvdm_core
%{_libdir}/gpvdm/
%{_datadir}/applications/gpvdm.desktop
%{_datadir}/mime/packages/gpvdm-gpvdm.xml
%{_datadir}/icons/gnome/scalable/mimetypes/application-gpvdm.svg
%{_datadir}/icons/hicolor/scalable/mimetypes/simulation-gpvdm.svg
%{_datadir}/gpvdm/
%{_mandir}/man1/gpvdm*


%doc %{_docdir}/README

%changelog
* Fri Dec 23 2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com> - ${ver}
  - See github for detailed change log

