import os
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





