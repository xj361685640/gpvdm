
import os
from dialog import Dialog
from to_github import to_github
from to_github import src_to_web

def publish_menu(d):
	code, tag = d.menu("publish for:",
		               choices=[("(windows)", "Publish for windows"),
		                        ("(debian)", "Publish for debian"),
								("(github)", "Publish for github"),
								])
	if code == d.OK:
		if tag=="(windows)":
			os.system("./pub.sh --windows &>out.dat &")
			ret=d.tailbox("out.dat", height=None, width=100)

		if tag=="(debian)":
			os.system("./pub.sh --windows")

		if tag=="(github)":
			os.system("./pub.sh --github")
			to_github(d)
			src_to_web(d)

		print(tag)
