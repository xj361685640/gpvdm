#!/usr/bin/env python2.7
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
#
#	www.gpvdm.com
#	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License v2.0, as published by
#    the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.



import pygtk
pygtk.require('2.0')
import gtk
import sys
import os
#import shutil
#from search import return_file_list
from util import str2bool
from inp import inp_get_token_value
import threading
#import gobject
import multiprocessing
import time
#import glob
import socket

from time import sleep
from win_lin import running_on_linux
import subprocess
from util import gui_print_path
from progress import progress_class

#from copying import copying
from cal_path import get_exe_command
#from global_objects import global_object_get
from help import my_help_class
from sim_warnings import sim_warnings
from inp_util import inp_search_token_value
from stat import *
from encrypt import encrypt
from encrypt import decrypt
from encrypt import encrypt_load
import i18n
_ = i18n.language.gettext

def server_find_simulations_to_run(commands,search_path):
	for root, dirs, files in os.walk(search_path):
		for my_file in files:
			if my_file.endswith("sim.gpvdm")==True:
#				full_name=os.path.join(root, my_file)
				commands.append(root)

class node:
	ip=""
	load=""
	cpus=""

class cluster:
	def cluster_init(self):
		self.socket = False
		self.cluster=False
		self.nodes=[]
		self.server_ip=inp_get_token_value("server.inp","#server_ip")

	def connect(self):
		if self.cluster==False:
			encrypt_load()
			print "conecting to:",self.server_ip
			self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			port=int(inp_get_token_value("server.inp","#port"))
			try:
				self.socket.connect((self.server_ip, port))
			except:
				print "Failed to connect to ",self.server_ip
				return False

			self.cluster=True

			header="gpvdmregistermaster\n"

			self.send_command(header)


			if self.running==False:
				self.mylock=False
				self.thread = threading.Thread(target = self.listen)
				self.thread.daemon = True
				self.thread.start()

			print "conected to cluster"
		else:
			self.socket.close()
			try:
				self.thread.stop()
			except:
				print "error stopping thread",sys.exc_info()[0]

			self.cluster=False
			print "not conected to cluster"

		return True

	def wake_nodes(self):
		if self.cluster==True:
			s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			port = 8888;
			s.sendto("wake_nodes", (self.server_ip, port))
			s.close()

	def clear_cache(self):
		#if self.cluster==True:
		#	if self.running==True:
		#		print "Clear cache on server"
		#		self.mylock=True
		#		s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		#		port = 8888;
		#		s.sendto("clear_cache#", (self.server_ip, port))
		#		s.close()
		#		print "I should be waiting for mylock"
		#		self.wait_lock()
		#		print "I have finished waiting"
		print "cache clear"

	def recvall(self,n):
		data = ''
		while len(data) < n:
			packet = self.socket.recv(n - len(data))
			if not packet:
				print "not packet"
				return None
			data += packet
		print "rx total=",len(data)
		data=decrypt(data)
		return data

	def cluster_make(self):
		my_dir="src"
		cmd=inp_get_token_value("server.inp","#make_command")
		header="gpvdmheadexe\n#dir\n"+my_dir+"\n#command\n"+cmd+"\n#end"

		self.send_command(header)


	def send_dir(self,path,target):
		count=0
		for root, dirs, files in os.walk(path):
			for name in files:
				fname=os.path.join(root, name)

				stat=os.stat(fname)[ST_MODE]

				print fname
				f = open(fname, 'rb')   
				bytes = f.read()
				size=len(bytes)
				f.close()

				expand=((int(size)/int(512))+1)*512-size
				bytes+= "\0" * expand

				#build header
				tx_name= os.path.normpath(fname[len(path):])

				#don't send strings starting in /
				start=0
				for i in range(0,len(tx_name)):
					if tx_name[i]=='\\':
						start=start+1
					else:
						break
				tx_name= tx_name[start:]

				buf=bytearray(512)
				if target=="":
					target=path


				header="gpvdmfile\n#file_name\n"+tx_name+"\n#file_size\n"+str(size)+"\n#target\n"+target+"\n#stat\n"+str(stat)+"\n#end"
				for i in range(0,len(header)):
					buf[i]=header[i]

				buf=buf+bytes
				buf=encrypt(buf)
				self.socket.sendall(buf)
				count=count+1
		print "total=",count

	def process_node_list(self,data):
		self.nodes=[]
		data = self.recvall(512)
		data=data.split("\n")
		for i in range(0,len(data)-1):
			self.nodes.append(data[i].split(":"))
		print self.nodes

	def rx_file(self,data):
		pwd=os.getcwd()
		lines=data.split("\n")
		name=inp_search_token_value(lines, "#file_name")
		size=int(inp_search_token_value(lines, "#file_size"))
		target=inp_search_token_value(lines, "#target")
		#print target,name
		target=target+name
		print "write to",target
		if target.startswith(pwd):
			packet_len=int(int(size)/int(512)+1)*512

			data = self.recvall(packet_len)
			if len(data)!=packet_len:
				print "packet mismatch",len(data),packet_len

			my_dir=os.path.dirname(target)

			if os.path.isdir(my_dir)==False:
				os.makedirs(my_dir)

			f = open(target, "wb")
			f.write(data)
			f.close()


	def send_command(self,header):
		buf=bytearray(512)
		for i in range(0,len(header)):
			buf[i]=header[i]
		buf=encrypt(buf)
		self.socket.sendall(buf)

	def copy_src_to_cluster(self):
		if self.cluster==True:
			path=inp_get_token_value("server.inp","#path_to_src")
			self.send_dir(path,"src")
			path=inp_get_token_value("server.inp","#path_to_libs")
			self.send_dir(path,"src")


	def cluster_get_data(self):
		if self.cluster==True:
			header="gpvdmgetdata\n"

			self.send_command(header)

	def cluster_get_info(self):
		if self.cluster==True:
			header="gpvdmsendnodelist\n"

			self.send_command(header)

	def cluster_quit(self):
		if self.cluster==True:
			header="gpvdmquit\n"

			self.send_command(header)


	def killall(self):
		if self.cluster==True:
			header="gpvdmkillall\n"

			self.send_command(header)

		else:
			print "stop jobs"
			if running_on_linux()==True:
				exe_name=os.path.basename(get_exe_command())
				cmd = 'killall '+exe_name
				ret= os.system(cmd)

		self.stop()


	def sleep(self):
		if self.cluster==True:
			header="gpvdmsleep"

			self.send_command(header)

	def poweroff(self):
		if self.cluster==True:
			header="gpvdmpoweroff"

			self.send_command(header)


	def wait_lock(self):
		print "Waiting for cluster..."
		while(self.mylock==True):
			sleep(0.1)

	def add_remote_job(self,job_orig_path):
		header="gpvdmaddjob\n#target\n"+job_orig_path+"\n#end"

		self.send_command(header)

	def cluster_clean(self):
		header="gpvdmclean\n"

		self.send_command(header)

	def listen(self):
		#print "thread !!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		self.running=True

		while(1):
			print "waiting for next command"
			understood=False
			data = self.recvall(512)
			
			#print "command=",data,len(data)
			if data.startswith("gpvdmfile"):
				self.rx_file(data)
				understood=True

			if data.startswith("gpvdmpercent"):
				lines=data.split("\n")
				percent=float(inp_search_token_value(lines, "#percent"))
				self.progress_window.set_fraction(percent/100.0)
				understood=True

			if data.startswith("gpvdmjobfinished"):
				lines=data.split("\n")
				name=inp_search_token_value(lines, "#job_name")
				self.label.set_text(gui_print_path("Finished:  ",name,60))
				understood=True

			if data.startswith("gpvdmfinished"):
				self.stop()
				understood=True

			if data.startswith("gpvdmheadquit"):
				self.stop()
				print "Server quit!"
				understood=True

			if data.startswith("gpvdmnodelist"):
				self.process_node_list(data)
				understood=True

			if understood==False:
				print "Command ",data, "not understood"
				sys.exit()
