#!/usr/bin/env python3

import os
import sys
from build_log import log

apt_present=True
try:
	import apt
except:
	apt_present=False

if apt_present==True:
	class LogInstallProgress(apt.progress.base.InstallProgress):
		def conffile(self, current, new):
			log("conffile prompt: "+ current+" " +new)

		def processing(self, pkg, stage):
			log("Processing " + pkg+ " stage: "+ stage)

		def error(self, pkg, errormsg):
			log("Package "+ pkg+ " error: "+ errormsg)

		def finish_update(self):
			log("Installation is complete")

		def status_change(self, pkg, percent, status):
			log("Package: "+ pkg + " at "+ str(percent) + " -> "+ status)

		def dpkg_status_change(self, pkg, status):
			log("Package "+ pkg + ", Status: "+ status)
	
		def start_update(self):
			log("start")

		def finish_update(self):
			log("Closing package cache")

def apt_install(d,my_list):
	f=open("out.dat", "w+")
	f.close()

	global apt_present
	if apt_present==False:
		d.msgbox("apt-get is not present on system are you sure you are on a Debian/Ubunu system?")
		return False

	newpid = os.fork()
	if newpid == 0:
		#dev_null = open('/dev/null', 'w')
		#os.dup2(dev_null, sys.stdout.fileno())                         
                                   
		#os.dup2(dev_null, sys.stderr.fileno())  
		cache = apt.cache.Cache()
		log("update")
		cache.update()
	




		print("Installing "+str(len(my_list))+" packages")
		installed=0
		already_installed=0
		for i in range(0,len(my_list)):
			if my_list[i] in cache:
				pkg = cache[my_list[i]]
				if pkg.is_installed:
					text=my_list[i]+" (Installed)"
					already_installed=already_installed+1
				else:
					text=my_list[i]+" (Will install)"
					installed=installed+1
					pkg.mark_install()

				log(text)
			else:
				log(my_list[i]+" Not found")
		#try:
		log("commit")
		prog=LogInstallProgress()
		cache.commit(install_progress=prog)
		print("\nInstalled= "+str(installed)+" Already installed "+str(already_installed))
		#except:
		#	log( "Sorry, package installation failed")

		cache.close()
		sys.exit()

#apt_install(["python3-xstatic-bootswatch"])
#pause = input('wait')
