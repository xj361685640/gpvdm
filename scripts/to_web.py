#! /usr/bin/env python3

"""An extension that ensures that given features are present."""
import os
import sys
from build_log import log
import shutil
try:
	from dialog import Dialog
except:
	from menu import Dialog

from os.path import expanduser

from build_paths import get_package_path
from build_paths import get_pub_path

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

def package_to_lib():
	src=get_pub_path()
	dest=get_package_path()
	for item in os.listdir(src):
		full_name=os.path.join(src,item)
		if os.path.isfile(full_name) and item.count("debug")==0:
			shutil.copyfile(full_name,os.path.join(dest,item))

def rpm_to_web(d):
	src=get_pub_path()
	web_path=os.path.join(expanduser("~"),"webpage/gpvdm.com/public_html/download_fedora")
	for item in os.listdir(src):
		full_name=os.path.join(src,item)
		if os.path.isfile(full_name) and item.count("debug")==0:
			shutil.copyfile(full_name,os.path.join(web_path,item))


def src_to_web(d):
	zip_dir="./pub/build/"
	web_path=os.path.join(expanduser("~"),"webpage/gpvdm.com/public_html/download_src")
	for item in os.listdir(zip_dir):
		full_path=os.path.join(zip_dir,item)
		if full_path.endswith("tar.gz")==True:
			shutil.copyfile(full_path,os.path.join(web_path,item))
			#print(item)


