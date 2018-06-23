
import os
try:
	from dialog import Dialog
except:
	from menu import Dialog

from to_web import to_github
from to_web import src_to_web
from publish import publish_src
def publish_menu(d):
	code, tag = d.menu("publish for:",
		               choices=[("(windows)", "Publish for windows"),
		                        ("(linux)", "Publish for linux"),
								("(github)", "Publish for github"),
								])
	if code == d.OK:
		if tag=="(windows)":
			publish_src(d,publication_mode="windows")
			#os.system("./pub.sh --windows &>out.dat &")
			ret=d.tailbox("out.dat", height=None, width=100)

#		if tag=="(debian)":
#			os.system("./pub.sh --debian")

		if tag=="(linux)":
			publish_src(d,publication_mode="gpl_distro")

		if tag=="(github)":
			#os.system("./pub.sh --github &>out.dat &")
			publish_src(d,publication_mode="gpl_distro")
			ret=d.tailbox("out.dat", height=None, width=100)
			to_github(d)
			src_to_web(d)

		print(tag)
