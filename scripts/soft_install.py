#! /usr/bin/env python3
import os
import shutil
try:
	from dialog import Dialog
except:
	from menu import Dialog

def install_icons(dest_dir):
	path=os.path.join(os.getcwd(),"images","icons")

	for root, dirs, files in os.walk(path):
		for file in files:
			if file.endswith(".png") or file.endswith(".svg"):
				src=os.path.join(root, file)
				dest=os.path.join(dest_dir,root[len(path)+1:],"mimetypes",file)
				if os.path.islink(dest) or os.path.isfile(dest):
					os.unlink(dest)
				shutil.copyfile(src, dest)

def soft_install(d):
	if os.geteuid() != 0:
		d.msgbox("You need to be root to do a soft install")
		return

	if d.yesno("Run soft install") == d.OK:
		bindir=open(os.path.join(os.getcwd(),"desktop","bindir")).readline().rstrip()
		datadir=open(os.path.join(os.getcwd(),"desktop","datadir")).readline().rstrip()

		bindir_gpvdm=os.path.join(bindir,"gpvdm")
		bindir_thumbnailer=os.path.join(bindir,"gpvdm-thumbnailer")

		datadir_thumbnailer=os.path.join(datadir,"thumbnailers","gpvdm.thumbnailer")

		icon=os.path.join(datadir,"icons","hicolor","scalable","mimetypes","simulation-gpvdm.svg")
		mime_dir=os.path.join(datadir,"mime")
		mime=os.path.join(mime_dir,"packages","gpvdm-gpvdm.xml")
		desktop_dir=os.path.join(datadir,"applications")
		desktop=os.path.join(desktop_dir,"gpvdm.desktop")

		

		if os.path.islink(bindir_gpvdm) or os.path.isfile(bindir_gpvdm):
			os.unlink(bindir_gpvdm)
		os.symlink( os.path.join(os.getcwd(),"gpvdm"), bindir_gpvdm)

		if os.path.islink(bindir_thumbnailer) or os.path.isfile(bindir_thumbnailer):
			os.unlink(bindir_thumbnailer)
		os.symlink( os.path.join(os.getcwd(),"gui","gpvdm-thumbnailer.py"), bindir_thumbnailer)

		install_icons(os.path.join(datadir,"icons","hicolor"))

		if os.path.islink(datadir_thumbnailer) or os.path.isfile(datadir_thumbnailer):
			os.unlink(datadir_thumbnailer)
		shutil.copyfile(os.path.join(os.getcwd(),"desktop","gpvdm.thumbnailer"), datadir_thumbnailer)

		if os.path.islink(mime) or os.path.isfile(mime):
			os.unlink(mime)
		shutil.copyfile(os.path.join(os.getcwd(),"desktop","gpvdm-gpvdm.xml"), mime)

		if os.path.islink(desktop) or os.path.isfile(desktop):
			os.unlink(desktop)
		shutil.copyfile(os.path.join(os.getcwd(),"desktop","gpvdm.desktop"), desktop)

		os.system("update-desktop-database "+desktop_dir)

		os.system("update-mime-database "+mime_dir)

		os.system("gtk-update-icon-cache /usr/share/icons/hicolor/")


def soft_install_menu(d):
	while(1):
		menu=[]
		menu.append(("(softinstall)", "Do soft install"))
		menu.append(("(exit)", "Exit"))

		code, tag = d.menu("gpvdm build system:", choices=menu)
		if code == d.OK:
			if tag=="(softinstall)":
				soft_install(d)

			if tag=="(exit)":
				break


	



