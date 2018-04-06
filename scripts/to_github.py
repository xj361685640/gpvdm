#! /usr/bin/env python3

"""An extension that ensures that given features are present."""
import os
import sys
from build_log import log
import shutil
from dialog import Dialog
from os.path import expanduser

def clean(dest,name):
	shutil.rmtree(os.path.join(dest,name),ignore_errors=True)

def copytree(src, dst, symlinks=False, ignore=None):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            shutil.copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)

def to_github(d):
	src="./pub/build/raw_code/"
	my_dir=os.getcwd()
	dest="/home/rod/webpage/git/gpvdm/"
	code,text=d.inputbox("What happened:")
	if code == d.OK:
		for filename in os.listdir(dest):
			full_name=os.path.join(dest,filename)
			if os.path.isdir(full_name)==True:
				if filename!=".git":
					clean(dest,full_name)
			else:
				os.remove(full_name)

		copytree(src, dest)

		os.chdir(dest)

		os.system("git add --all")
		os.system("git commit -m \""+text+"\"")
		os.system("git push origin master --force")
		os.chdir(my_dir)

def src_to_web(d):
	zip_dir="./pub/build/"
	web_path=os.path.join(expanduser("~"),"webpage/gpvdm.com/public_html/download_src")
	for item in os.listdir(zip_dir):
		full_path=os.path.join(zip_dir,item)
		if full_path.endswith("tar.gz")==True:
			shutil.copyfile(full_path,os.path.join(web_path,item))
			#print(item)
#if __name__ == "__main__":
#	d = Dialog(dialog="dialog")
#
#	Dialog.set_background_title() requires pythondialog 2.13 or later
#	d.set_background_title("https://www.gpvdm.com build configure, Roderick MacKenzie 2018")
#	to_github(d)

