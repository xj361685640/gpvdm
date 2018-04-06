#! /usr/bin/env python3

"""An extension that ensures that given features are present."""
import os
import sys
from build_log import log
try:
	import dnf.rpm

	class TransactionProgress(dnf.callback.TransactionProgress):

		def __init__(self):
			self.actions = {dnf.callback.PKG_CLEANUP: 'cleanup',
				            dnf.callback.PKG_DOWNGRADE: 'downgrade',
				            dnf.callback.PKG_REMOVE: 'erase',
				            dnf.callback.PKG_INSTALL: 'install',
				            dnf.callback.PKG_OBSOLETE: 'obsolete',
				            dnf.callback.PKG_REINSTALL: 'reinstall',
				            dnf.callback.PKG_UPGRADE: 'update',
				            dnf.callback.PKG_VERIFY: 'verify'}

			super(dnf.callback.TransactionProgress, self).__init__()
			self.do_verify = False

		def progress(self, package, action, ti_done, ti_total, ts_done, ts_total):
			"""
			@param package: A yum package object or simple string of a package name
			@param action: A constant transaction set state
			@param te_current: current number of bytes processed in the transaction
			element being processed
			@param te_total: total number of bytes in the transaction element being
			processed
			@param ts_current: number of processes completed in whole transaction
			@param ts_total: total number of processes in the transaction.
			"""
			if package:
				# package can be both str or dnf package object
				if not isinstance(package, str):
					pkg_id = str(package)
				else:
					pkg_id = package
				if action in self.actions:
					action = self.actions[action]

				if action=="verify":
					log("installed "+pkg_id)
					#print()

				#print ("aaa %s %s"%(pkg_id, action),package, type(action), ti_done, ti_total, ts_done, ts_total)
				#self.base.RPMProgress(
				#pkg_id, action, te_current, te_total, ts_current, ts_total)
except:
	print("not a dnf system")

def check_installed(sack_query,name):
	installed = sack_query.installed()

	for pkg in installed:
		if pkg.name==name:
			return True
	return False


def dnf_install(d,packages):
	newpid = os.fork()
	if newpid == 0:
		with dnf.Base() as base:
			f=open("out.dat", "w+")
			f.close()
			# Substitutions are needed for correct interpretation of repo files.
			RELEASEVER = dnf.rpm.detect_releasever(base.conf.installroot)
			base.conf.substitutions['releasever'] = RELEASEVER
			# Repositories are needed if we want to install anything.
			base.read_all_repos()
			#base.redirect_logger( stdout=None, stderr=None)
			# A sack is required by marking methods and dependency resolving.
			base.fill_sack()

			sack_query = base.sack.query()


			avail_rpms = sack_query.available()
			to_install=[]
			for pkg in packages:
				if check_installed(sack_query,pkg)==False:
					to_install.append(pkg)
				else:
					log("Already installed: "+pkg)
					

			#installed = sack_query.installed()

			#for name in packages:
			#	for i in avail_rpms:
			#		iname=i.name
					
			#	if pkg.name in packages:
			#		print(type(pkg),pkg.installtime)

			for pkg in avail_rpms:

				if pkg.name in to_install:
					#print(type(pkg),pkg.installtime)
					log("I will install :"+pkg.name)
					base.install(pkg.name)

			base.resolve()

			base.download_packages(base.transaction.install_set)

			base.do_transaction(display=TransactionProgress())

	log("Finished")
#dnf_install(["python3-zope-structuredtext"])

