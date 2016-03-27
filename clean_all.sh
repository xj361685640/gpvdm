#!/bin/bash
for i in `find -iname "*.o" -o -iname "*.a" -o -iname "*.dll" -o -iname "*.so"  -o -iname "debhelper.log" -o -iname  "substvars"` ; do
rm $i
done

rm -f gpvdm_core
