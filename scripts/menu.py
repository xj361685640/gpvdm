#! /usr/bin/env python3

import sys


import os
import locale
from tail import tail

class Dialog():

	def __init__(self,dialog="dialog"):
		self.OK=0
		self.CANCEL=1
		return

	def clear(self):
		print(chr(27) + "[2J")

	def set_background_title(self,text):
		print(text)

	def tailbox(self,fname, height=None, width=100):
		tail(fname)
		return

	def yesno(self,text):
		#self.clear()
		n=input(text+" (y/n)")
		if n=="y":
			return self.OK
		return self.cancel

	def msgbox(self,text):
		print(text)
		n=input('Press enter:')
		return

	def menu(self,text, choices=[]):
		#self.clear()
		print("The python3 module Dialog is not installed")
		print("If you are on Ubuntu/Debian system, try apt-get install python3-dialog")
		print("If you are on Fedora/Redhat system, try yum install python3-dialog\n\n")
		for i in range(0,len(choices)):
			print(i,choices[i][1])

		code=self.OK

		n=input('Enter your input:')	
		n=int(n)
		tag=choices[n][0]	
		return code, tag


