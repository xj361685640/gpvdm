#!/usr/bin/env python2.7
import os
config_files=[]
config_files.append("")
config_files.append("lib")
config_files.append("libdos")
config_files.append("liblight")
config_files.append("libmeasure")
config_files.append("libdump")
config_files.append("libserver")
config_files.append("libmesh")
config_files.append("libfit")

for root, dirs, files in os.walk("./plugins"):
    for file in files:
        if file.endswith("Makefile.am"):
			name=os.path.join(root, file)[2:-12]
			config_files.append(name)

for root, dirs, files in os.walk("./exp"):
    for file in files:
        if file.endswith("Makefile.am"):
			name=os.path.join(root, file)[2:-12]
			config_files.append(name)

config_files.append("src")
config_files.append("images")
config_files.append("man")

f = open("config_files.m4", "w")
for i in range(0,len(config_files)):
	f.write( "AC_CONFIG_FILES(["+os.path.join(config_files[i],"Makefile")+"])\n")

f.close()

f = open("make_files.m4", "w")
f.write( "AC_SUBST(BUILD_DIRS,\"")
for i in range(0,len(config_files)):
	f.write(config_files[i]+" ")

f.write("\")")

f.close()


