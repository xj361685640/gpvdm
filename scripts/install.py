import os
from soft_install import soft_install

try:
	from dialog import Dialog
except:
	from menu import Dialog

def install(d):
	if os.geteuid() != 0:
		d.msgbox("You need to be root to install packages")
		return

	if d.yesno("Run install") == d.OK:
		os.system("make install  >out.dat 2>out.dat &")
		et=d.tailbox("out.dat", height=None, width=150)


def uninstall(d):
	if os.geteuid() != 0:
		d.msgbox("You need to be root to install packages")
		return

	if d.yesno("Run uninstall") == d.OK:
		os.system("make uninstall  >out.dat 2>out.dat &")
		et=d.tailbox("out.dat", height=None, width=150)

def desktopinstall(d):
	soft_install(d)

def install_menu(d):
	while(1):
		menu=[]
		menu.append(("(install)", "Install"))
		menu.append(("(uninstall)", "Uninstall"))
		menu.append(("(desktop)", "Link xdesktop hooks to this dir"))
		menu.append(("(exit)", "Exit"))

		code, tag = d.menu("gpvdm build system:", choices=menu)
		if code == d.OK:
			if tag=="(install)":
				install(d)

			if tag=="(uninstall)":
				uninstall(d)

			if tag=="(desktop)":
				desktopinstall(d)

			if tag=="(exit)":
				break





