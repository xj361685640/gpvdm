#!/bin/bash
pwd
./make_m4.py --win

aclocal
automake --add-missing
automake
autoconf

flags="-I$HOME/windll/libzip/libzip-0.11.2/lib/ -I$HOME/windll/gsl-1.16/ -I$HOME/windll/umfpack/UFconfig/ -I$HOME/windll/umfpack/AMD/Include/ -I$HOME/windll/umfpack/UMFPACK/Include/"

./configure --host=i686-w64-mingw32 CPPFLAGS="$flags"  --enable-noplots --enable-noman
