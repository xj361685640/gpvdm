#!/bin/bash
pwd

flags="-I/usr/include/"

./configure CPPFLAGS="$flags" --host=i686-linux-gnu --build=i686-linux-gnu CC="gcc -m32" CXX="g++ -m32"
