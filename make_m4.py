#!/usr/bin/env python2.7
import os
config_files=[]
link_libs=""
config_files.append("")
config_files.append("lang")

config_files.append("lib")
link_libs=link_libs+" -lgpvdm_lib"

config_files.append("libdos")
link_libs=link_libs+" -lgpvdm_dos"

config_files.append("liblight")
link_libs=link_libs+" -lgpvdm_light"

config_files.append("libmeasure")
link_libs=link_libs+" -lgpvdm_measure"

config_files.append("libdump")
link_libs=link_libs+" -lgpvdm_dump"

config_files.append("libserver")
link_libs=link_libs+" -lgpvdm_server"

config_files.append("libmesh")
link_libs=link_libs+" -lgpvdm_mesh"

if os.path.isdir("libfit"):
	config_files.append("libfit")
	link_libs=link_libs+" -lgpvdm_fit"

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


f = open("local_link.m4", "w")
f.write( "AC_SUBST(LOCAL_LINK,\"")
f.write(link_libs)
f.write("\")")

f.close()


