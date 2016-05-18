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
import hashlib
import i18n
_ = i18n.language.gettext

def strip_slash(tx_name):
	start=0
	for i in range(0,len(tx_name)):
		if tx_name[i]=='\\' or tx_name[i]=='/':
			start=start+1
		else:
			break

	return tx_name[start:]

class node:
	ip=""
	load=""
	cpus=""

class tx_struct:
	id=0
	src=""
	file_name=""
	size=0
	target=""
	stat=0
	compressed=False
	data=""

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

	def sync_dir(self,path,target):
		count=0
		banned=[]
		sums=""

		files=self.gen_dir_list(path)
		for fname in files:
			f = open(fname, 'rb')   
			bytes = f.read()
			size=len(bytes)
			f.close()

			m = hashlib.md5()
			m.update(bytes)
			bin=m.digest()
			key_hash=""
			for i in range(0,m.digest_size):
				key_hash=key_hash+format(ord(bin[i]), '02x')
			sums=sums+fname+"\n"+key_hash+"\n"


		sums=sums[0:len(sums)-1]
		#print "sending",sums
		size=len(sums)

		tx_size=((int(size)/int(512))+1)*512


		data=bytearray(tx_size)
		for i in range(0,size):
			data[i]=sums[i]

		
		head="gpvdm_sync_packet_one\n#size\n"+str(size)+"\n#target\n"+target+"\n#src\n"+path+"\n#end"

		start_len=len(head)

		head_buf=bytearray(512)
		for i in range(0,len(head)):
			head_buf[i]=head[i]

		buf=head_buf+data

		buf=encrypt(buf)
		self.socket.sendall(buf)


	def gen_dir_list(self,path):

		file_list=[]

		for root, dirs, files in os.walk(path):
			for name in files:

				fname=os.path.join(root, name)

				tx=True

				if name.count("snapshots")>0:
					tx=False

				if name.endswith(".pdf") or name.endswith(".png") or name.endswith(".dll"):
					tx=False

				if tx==True:
					fname=fname[len(path):]

					fname=strip_slash(fname)
					file_list.append(fname)

		#print file_list
		return file_list

	def send_dir(self,src,target):
		print "Sending dir",src,target
		files=self.gen_dir_list(src)
		self.send_files(target,src,files)

	def send_files(self,target,src,files):
		count=0
		banned=[]

		for fname in files:
			full_path=os.path.normpath(os.path.join(src,fname))

			stat=os.stat(full_path)[ST_MODE]
			#print full_path
			f = open(full_path, 'rb')   
			bytes = f.read()
			size=len(bytes)
			f.close()

			expand=((int(size)/int(512))+1)*512-size
			bytes+= "\0" * expand


			#don't send strings starting in /
			tx_name=strip_slash(fname)

			buf=bytearray(512)
			if target=="":
				target=src

			#print "tx_name=",tx_name
			header="gpvdmfile\n#file_name\n"+tx_name+"\n#file_size\n"+str(size)+"\n#target\n"+target+"\n#stat\n"+str(stat)+"\n#end"
			for i in range(0,len(header)):
				buf[i]=header[i]

			buf=buf+bytes
			buf=encrypt(buf)
			self.socket.sendall(buf)
			count=count+1
			print "sending",full_path
		print "total=",count

	def process_node_list(self,data):
		self.nodes=[]
		data = self.recvall(512)
		data=data.split("\n")
		for i in range(0,len(data)-1):
			self.nodes.append(data[i].split(":"))
		print self.nodes

	def process_job_list(self,data):
		ret=self.rx_packet(data)
		print ret.data

	def process_sync_packet_two(self,data):
		lines=data.split("\n")
		target=inp_search_token_value(lines, "#target")
		src=inp_search_token_value(lines, "#src")
		size=int(inp_search_token_value(lines, "#size"))

		packet_len=int(int(size)/int(512)+1)*512
		data = self.recvall(packet_len)
		data = data[0:size]
		lines=data.split("\n")
		pos=0
		copy_list=[]
		#print lines
		for i in range(0,len(lines)):
			fname=lines[pos]
			pos=pos+1
			if fname!="":
				copy_list.append(fname)

		self.send_files(target,src,copy_list)

	def process_sync_packet_one(self,data):
		print data


	def rx_packet(self,data):
		ret=tx_struct()

		lines=data.split("\n")
		ret.file_name=inp_search_token_value(lines, "#file_name")
		ret.size=int(inp_search_token_value(lines, "#file_size"))
		ret.target=inp_search_token_value(lines, "#target")


		if ret.size!=0:
			packet_len=int(int(ret.size)/int(512)+1)*512
			ret.data = self.recvall(packet_len)
			ret.data=ret.data[0:ret.size]

		return ret

	def rx_file(self,data):
		print data
		pwd=os.getcwd()
		lines=data.split("\n")
		name=inp_search_token_value(lines, "#file_name")
		size=int(inp_search_token_value(lines, "#file_size"))
		target=inp_search_token_value(lines, "#target")
		#print target,name
		target=target+name

		if size!=0:
			packet_len=int(int(size)/int(512)+1)*512
			data = self.recvall(packet_len)

			if target.startswith(pwd):
				print "write to",target,len(data),packet_len
				if len(data)!=packet_len:
					print "packet mismatch",len(data),packet_len

				my_dir=os.path.dirname(target)

				if os.path.isdir(my_dir)==False:
					os.makedirs(my_dir)

				f = open(target, "wb")
				f.write(data[0:size])
				f.close()
			else:
				print "not writing target",pwd,target
		else:
				f = open(target, "wb")
				f.close()


	def send_command(self,header):
		buf=bytearray(512)
		for i in range(0,len(header)):
			buf[i]=header[i]
		buf=encrypt(buf)
		self.socket.sendall(buf)

	def copy_src_to_cluster_fast(self):
		if self.cluster==True:
			path=inp_get_token_value("server.inp","#path_to_src")
			self.sync_dir(path,"src")
			path=inp_get_token_value("server.inp","#path_to_libs")
			self.sync_dir(path,"src")
		
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
		self.send_command("gpvdm_master_clean\n")


	def cluster_list_jobs(self):
		self.send_command("gpvdm_send_job_list\n")

	def listen(self):
		#print "thread !!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		self.running=True

		while(1):
			print "waiting for next command"
			understood=False
			data = self.recvall(512)
			
			if data==None:
				break

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

			if data.startswith("gpvdm_sync_packet_two"):
				self.process_sync_packet_two(data)
				understood=True

			if data.startswith("gpvdm_sync_packet_one"):
				self.process_sync_packet_one(data)
				understood=True

			if data.startswith("gpvdm_job_list"):
				self.process_job_list(data)
				understood=True

			if understood==False:
				print "Command ",data, "not understood"
				
