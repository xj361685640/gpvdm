#!/bin/bash
pwd
./make_m4.py
# hpc
aclocal
automake --add-missing
automake
autoconf
flags="-I/usr/include/suitesparse/"

./configure CPPFLAGS="$flags" 
