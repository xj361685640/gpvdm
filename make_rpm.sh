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

unzip -p sim.gpvdm ver.inp >data.dat
ver=`cat data.dat|sed -n 2p`
dist=fc22
mydir=`pwd`
rpmdir=~/rpmbuild
mytarget=x86_64
#i686
#make clean 
#make
rm ${rpmdir}/BUILD ${rpmdir}/SOURCES ${rpmdir}/SPECS -rf
mkdir ${rpmdir} 
cd ${rpmdir} 
mkdir BUILD RPMS SOURCES SPECS SRPMS
cd $mydir
mkdir gpvdm-${ver}

cp ./* ./gpvdm-${ver}/ -rf

tar -cf  ${rpmdir}/SOURCES/gpvdm-${ver}.tar ./gpvdm-${ver}/

rm ${rpmdir}/BUILDROOT -rf
mkdir ${rpmdir}/BUILDROOT

cat > ${rpmdir}/SPECS/gpvdm.spec << EOF
# spec file for gpvdm

Summary:		General-purpose  solar cell device model (gpvdm)
License:		GPLv2
Name:			gpvdm
Version:		${ver}
Release:		3%{dist}
Source:			http://www.roderickmackenzie.eu/gpvdm-${ver}.tar
Url:			http://www.gpvdm.com
Group:			Development/Tools


BuildRequires: suitesparse-devel, zlib-devel, gsl-devel, blas-devel, libcurl-devel, gnuplot, numpy, python-matplotlib, texlive, ghostscript, ImageMagick, vte  ,pywebkitgtk, python-crypto, awake, python-awake, notify-python

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

%build
make %{?_smp_mflags} OPT_FLAGS="%{optflags}" OPT_ARCH=%{_arch}

%install
make  DESTDIR=%{buildroot} DESTLIB=%{_lib} install


%files
%{_bindir}/gpvdm
%{_bindir}/gpvdm_core
%{_libdir}/gpvdm/
%{_datadir}/applications/gpvdm.desktop
%{_datadir}/mime/packages/gpvdm-gpvdm.xml
%{_datadir}/icons/gnome/scalable/mimetypes/application-gpvdm.svg

%{_datadir}/gpvdm/
%{_mandir}/man1/gpvdm*

%doc README
%doc license.txt

%changelog
* Sun Nov 16 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-10
  - Added cluster support to gpvdm python script
  - Ported to windows 
  - There have been lots of changes since May but I have not kept track of them.
* Sat May 31 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-9
  - Cleaned up fitting code.
  - homo0.inp lumo0.inp now have version numbers
* Tue May 20 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-8
  - Rewrote optical model to account for non relfecting back contact.
  - Replaced plotting code in gpvdm to used matplot lib instead of gnuplot
  - Fixed bugs in printing of carrier distributions
  - Plot information now written to file with data
  - Removed code to handle 2D data from i.c
  - Removed simple exp() optical model.
  - moved zipping of output files to end of program execution.
  - made homo and lumo file format better.
* Mon Apr 14 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-7
  - Added undo button
  - added --clean command
  - added n.dat and p.dat files
  - Fiexed xy parameter selector dialouge box
  - Rewrote sections of tab.py to make it compatible with the undo function.
* Sat Apr 05 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-7
  - Fixed xy parameter selector dialogue box
  - made inp.h thread safe, by removing statics
  - tided up how light interface allocates memory.
  - Made find_voc dump charge density and carrier recombination rates.
* Wed Apr 02 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-5
  - Fixed bug in scan tab so that it can run more than once.
  - Fixed open files left by mkstemp.
  - Gives an error if electrical and optical mesh are not the same size.
  - Fixed bugs in plotted files menu.
* Mon Mar 31 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-4
  - Fixed more bugs in rpm build scripts.
  - Model fails nicly if it can't find the correct optical model by defaulting to the exponential model.
  - Made gpvdm --import also import scan directories
  - Fixed rpm so it also requires vte
* Sun Mar 30 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-3
  - tab.py can now do comboboxes
  - removed mod function  
* Sat Mar 29 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-2
  - Added virtual terminal tab
  - Consolidated gui functions startting and stopping the simulation
  - Added the –lock command line option to gpvdm_core
  - move the gui_hooks to main and out of the plugins
  - Auto switching to terminal tab when running
* Sat Mar 22 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.70-1
  - Bug fixes in rpm build for new release. 
* Fri Mar 21 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.59-1
  - Optical model now dumps to a zip file to save inodes.
* Thu Mar 20 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.58-1
  - Removed the GUI from this rpm leaving just gpvdm_core and the input files.
* Sun Mar 16 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.57-1
  - Fixed broken find_voc function
* Mon Feb 10 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.56-1
  - Updated archive import function to handle compressed archives
  - Updated rpm building tools to handle new compressed archives
* Sat Feb 08 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.55-1
  - Started moving input files into archive.
  - dynamic files placed in own directory
  - files that no longer exist are automaticly removed from used file menu.
  - scan_tab opendialog opens in correct directory.
* Mon Jan 20 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.54-1
  - Fixed bug in the close tab button reported by Simon Schmeisser.
  - Fixed bug in rpm_build script which damaged the optical model code
  - removed gpvdm_import
* Sun Jan 19 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.53-1
  - Made the about window display the correct version number
  - removed simulation_dir variable from gpvdm, fixed more
  - removed gpvdm_clone replaced with gpvdm --clone
  - Fixed missing icon in optical simulation window
* Fri Jan 17 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.52-1
  - removed bash script gpvdm_dump and added option --dump-tab to gpvdm
  - made tool bar show before material parameter window
  - disabled menu callbacks during load
  - improved the import function so it tries to import a simulation even if it an old version
  - made version numbers between gui and core the same
* Wed Jan 15 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.5-2  
  - Windows now remember where they are put after application shutdown.
  - Fixed loading bugs
  - Added logging
* Tue Jan 14 2014 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.5-1  
  - Made the optical model dynamicly linked rather than statically linked
  - Improved plotting of single point data, i.e FF, Voc and Jsc.
  - Added window to analyze simulation data
  - More meaningful icons
  - Unified progress bar/spinner widgets
  - Moved optical output into one directory
  - Parameter scan window can now do multiple simulations
  - Parameter scan window can deal with multiple CPUs/cores
  - Improved parameter scan window GUI
  - Mesh editor in it's own window
* Thu Nov 14 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.4-1  
  - Added export material parameters to pdf/tar.gz/jpg file
  - Import material parameters from tar.gz file 
  - Parameter scan window updated and fixed
  - 32 bit compatibility fixed
* Sat Nov 09 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.2-1  
  - Improved gui
  - Better error handeling
  - Fixed rpm installer bugs
* Fri Oct 18 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.1-1
  - Fixed memory leeks in gpvdm_core
  - Fixed bugs in free-free recombination calculation that made solver crash wheh it was turned off
  - Made structure of input files more logical  
  - Improved gui
  - Better intergration to x-desktop
* Fri Sep 06 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.0-2
  - Edits to conform to fedora requirements as suggested by Christopher Meng/Michael Schwendt.
* Wed Sep 04 2013 Roderick MacKenzie <roderick.mackenzie@nottingham.ac.uk> - 2.0-1
  -  Original submission to bugzilla
EOF

cd ${rpmdir}

rpmbuild -v --target ${mytarget} -ba --clean ./SPECS/gpvdm.spec

cp ./SRPMS/*.rpm ~/webpage/
cp ./RPMS/${mytarget}/*.rpm ~/webpage/
cp ./SOURCES/*.tar ~/webpage/
#cp ./SPECS/* ~/webpage/

cd $mydir

cp ~/rpmbuild/RPMS/${mytarget}/gpvdm-*.rpm ../
