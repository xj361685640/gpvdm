#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
#
#	https://www.gpvdm.com
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

#from copying import copying
from cal_path import get_exe_command
#from global_objects import global_object_get
#from help import my_help_class
from sim_warnings import sim_warnings
from inp_util import inp_search_token_value
from stat import *
from encrypt import encrypt
from encrypt import decrypt
from encrypt import encrypt_load
import hashlib
import i18n
_ = i18n.language.gettext
import zlib
from cal_path import get_src_path
from progress import progress_class

from gui_enable import gui_get

if gui_get()==True:
	from PyQt5.QtCore import pyqtSignal

from cal_path import get_sim_path
from cal_path import get_exe_name

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
	dir_name=""
	command=""
	file_name=""
	size=0
	target=""
	stat=0
	compressed=False
	data=""
	exe_name=""
	zip=0
	uzipsize=0
	cpus=-1

class cluster:
	if gui_get()==True:
		load_update = pyqtSignal()
	
	def cluster_init(self):
		self.socket = False
		self.cluster=False
		self.nodes=[]
		self.server_ip=inp_get_token_value(os.path.join(get_sim_path(),"server.inp"),"#server_ip")
		self.cluster_jobs = []

	def connect(self):
		if self.cluster==False:
			encrypt_load()
			print("conecting to:",self.server_ip)
			self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			port=int(inp_get_token_value(os.path.join(get_sim_path(),"server.inp"),"#port"))
			try:
				self.socket.connect((self.server_ip, port))
			except:
				print("Failed to connect to ",self.server_ip)
				return False

			self.cluster=True

			data=tx_struct()
			data.id="gpvdmregistermaster"
			self.tx_packet(data)

			if self.running==False:
				self.mylock=False
				self.thread = threading.Thread(target = self.listen)
				self.thread.daemon = True
				self.thread.start()

			print("conected to cluster")
		else:
			self.socket.close()
			try:
				self.thread.stop()
			except:
				print("error stopping thread",sys.exc_info()[0])

			self.cluster=False
			print("not conected to cluster")

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
		print("cache clear")

	def recvall(self,n):
		data = bytearray() 
		while len(data) < n:
			packet = self.socket.recv(n - len(data))
			if not packet:
				print("not packet")
				return None
			data += packet
			#print(100.0*len(data)/n)
		data=decrypt(data)
		return data

	def cluster_make(self):
		data=tx_struct()
		data.id="gpvdmheadexe"
		data.dir_name="src"
		data.command=inp_get_token_value(os.path.join(get_sim_path(),"server.inp"),"#make_command")
		self.tx_packet(data)

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
		#print("sending",sums0
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

		banned_types=[".pdf",".png",".dll",".o",".so",".so",".a",".dat",".aprox",".ods",".xls",".xlsx",".log",".pptx",".dig",".old",".bak",".opj",".csv"]
		banned_dirs=["equilibrium","man_src","images","snapshots", "plot","pub","gui","debian","desktop"]

		file_list=[]

		for root, dirs, files in os.walk(path):
			for name in files:
					
				fname=os.path.join(root, name)

				tx=True

				ext=os.path.splitext(name)
				if len(ext)>0:
					ext=ext[1]
					if ext in banned_types:
						tx=False

				if tx==True:
					fname=fname[len(path):]

					fname=strip_slash(fname)
					file_list.append(fname)

		f=[]
		for i in range(0,len(file_list)):
			if file_list[i].endswith("gpvdm_gui_config.inp"):
				banned_dirs.append(os.path.dirname(file_list[i]))

		for i in range(0,len(file_list)):
			add=True
			for ii in range(0,len(banned_dirs)):
				if file_list[i].startswith(banned_dirs[ii]+os.path.sep)==True:
					add=False
					break

			if add==True:
				f.append(file_list[i])
		return f

	def send_dir(self,src,target):
		print("Sending dir",src,target)
		files=self.gen_dir_list(src)
		self.send_files(target,src,files)

	def tx_packet(self,data):
		
		#If the user wants to tx a string conver it into byte sfirst
		if type(data.data)==str:
			dat=str.encode(data.data)
		else:
			dat=data.data

		if data.zip==True:
			dat = zlib.compress(dat)

		tx_size=len(dat)
		if tx_size!=0:
			expand=((int)(tx_size/512)+1)*512-tx_size
			expand=int(expand)
			zeros=bytearray(expand)
			dat += zeros

		header=data.id+"\n"

		if data.file_name!="":
			header=header+"#file_name\n"+data.file_name+"\n"

		header=header+"#size\n"+str(tx_size)+"\n"

		if data.target!="":
			header=header+"#target\n"+data.target+"\n"

		header=header+"#stat\n"+str(data.stat)+"\n"

		if data.dir_name!="":
			header=header+"#dir_name\n"+data.dir_name+"\n"

		if data.exe_name!="":
			header=header+"#exe_name\n"+data.exe_name+"\n"

		if data.command!="":
			header=header+"#command\n"+data.command+"\n"

		if data.cpus!=-1:
			header=header+"#cpus\n"+str(data.cpus)+"\n"

		if data.zip==True:
			header=header+"#zip\n"+"1"+"\n#uzipsize\n"+str(data.uzipsize)+"\n"

		header=header+"#end"

		buf=bytearray(512)

		for i in range(0,len(header)):
			buf[i]=ord(header[i])

		buf += dat
		#buf=buf+bytes
		print("I am encrypting",len(buf),data.id,len(buf),len(dat))
		buf=encrypt(buf)
		print("I am sending",len(buf),data.id)
		self.socket.sendall(buf)

	def send_files(self,target,src,files):
		count=0
		banned=[]

		for fname in files:
			data=tx_struct()
			full_path=os.path.normpath(os.path.join(src,fname))

			data.stat=os.stat(full_path)[ST_MODE]

			f = open(full_path, 'rb')   
			bytes = f.read()
			f.close()
			orig_size=len(bytes)

			print("tx file:",full_path)

			if target=="":
				data.target=src
			else:
				data.target=target

			data.id="gpvdmfile"
			data.uzipsize=len(bytes)
			data.data=bytes
			data.zip=True
			data.file_name=strip_slash(fname)
			count=count+1
			self.tx_packet(data)
			#if count>2:
			#	break

		print("total=",count)

	def process_node_list(self,data):
		self.nodes=[]
		data = self.recvall(512)
		data=data.decode("utf-8") 
		data=data.split("\n")
		for i in range(0,len(data)-1):
			self.nodes.append(data[i].split(":"))
		#print(self.nodes)

	def process_job_list(self,data):
		ret=self.rx_packet(data)
		lines=ret.data.decode("utf-8").split("\n") 
		self.cluster_jobs=[]
		for line in lines:
			act=line.split()
			if len(act)==9:
				self.cluster_jobs.append([act[0], act[1], act[2], act[3],act[4], act[5], act[6], act[7]])
			else:
				print(line)

		print(ret.data,"jim")

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
		#print(lines)
		for i in range(0,len(lines)):
			fname=lines[pos]
			pos=pos+1
			if fname!="":
				copy_list.append(fname)

		self.send_files(target,src,copy_list)

	def process_sync_packet_one(self,data):
		print(data)


	def rx_packet(self,data):
		ret=tx_struct()

		lines=data[0:512].decode("utf-8").split("\n")
		ret.file_name=inp_search_token_value(lines, "#file_name")
		ret.size=int(inp_search_token_value(lines, "#size"))
		ret.target=inp_search_token_value(lines, "#target")
		ret.zip=int(inp_search_token_value(lines, "#zip"))
		ret.uzipsize=int(inp_search_token_value(lines, "#uzipsize"))

		print(ret.file_name,ret.size,ret.uzipsize,len(data))

		if ret.size!=0:
			packet_len=int(int(ret.size)/int(512)+1)*512
			ret.data = self.recvall(packet_len)
			if len(ret.data)!=packet_len:
				print("packet len does not match size",len(ret.data),packet_len)
			ret.data=ret.data[0:ret.size]
			if ret.zip==1:
				ret.data = zlib.decompress(ret.data)
				ret.size=len(ret.data)

		return ret

	def rx_file(self,data):
		pwd=get_sim_path()
		ret=self.rx_packet(data)

		target=ret.target+ret.file_name

		if target.startswith(pwd):
			my_dir=os.path.dirname(target)

			if os.path.isdir(my_dir)==False:
				os.makedirs(my_dir)

			if ret.size>0:

				print("write:",target)
				f = open(target, "wb")
				f.write(ret.data[0:ret.size])
				f.close()
			else:
				f = open(target, "wb")
				f.close()
		else:
			print("not writing target",pwd,target)

	def set_cluster_loads(self,ip,loads):

		packet=""
		for i in range(0,len(ip)):
			packet=packet+ip[i]+"\n"+str(loads[i])+"\n"

		packet=packet[:-1]
		data=tx_struct()
		data.id="gpvdm_set_max_loads"
		data.data=packet
		data.size=len(packet)
		self.tx_packet(data)

	def copy_src_to_cluster_fast(self):
		if self.cluster==True:
			path=get_src_path()
			if path==None:
				return
			self.sync_dir(path,"src")
			path=inp_get_token_value(os.path.join(get_sim_path(),"server.inp"),"#path_to_libs")
			self.sync_dir(path,"src")
		
	def copy_src_to_cluster(self):
		if self.cluster==True:
			path=get_src_path()
			if path==None:
				return
			self.send_dir(path,"src")
			path=inp_get_token_value(os.path.join(get_sim_path(),"server.inp"),"#path_to_libs")
			self.send_dir(path,"src")


	def cluster_get_data(self):
		if self.cluster==True:
			data=tx_struct()
			data.id="gpvdmgetdata"
			self.tx_packet(data)


	def cluster_get_info(self):
		if self.cluster==True:
			data=tx_struct()
			data.id="gpvdmsendnodelist"
			self.tx_packet(data)

	def cluster_quit(self):
		if self.cluster==True:
			data=tx_struct()
			data.id="gpvdmquit"
			self.tx_packet(data)


	def killall(self):
		if self.cluster==True:
			data=tx_struct()

			data.id="gpvdm_stop_all_jobs"
			self.tx_packet(data)

			data.id="gpvdmkillall"
			self.tx_packet(data)

			sleep(1)

			data.id="gpvdm_delete_all_jobs"
			self.tx_packet(data)

		else:
			print("stop jobs")
			if running_on_linux()==True:
				cmd = 'killall '+get_exe_name()
				os.system(cmd)
				print(cmd)
			else:
				cmd="taskkill /im "+get_exe_name()
				print(cmd)
				os.system(cmd)

		self.stop()

	def cluster_run_jobs(self):
		exe_name=inp_get_token_value(os.path.join(get_sim_path(),"server.inp"),"#exe_name")
		data=tx_struct()
		data.id="gpvdmrunjobs"
		data.exe_name=exe_name
		self.tx_packet(data)


	def sleep(self):
		if self.cluster==True:
			data=tx_struct()
			data.id="gpvdmsleep"
			self.tx_packet(data)


	def poweroff(self):
		if self.cluster==True:
			data=tx_struct()
			data.id="gpvdmpoweroff"
			self.tx_packet(data)

	def add_remote_job(self,job_orig_path):
		data=tx_struct()
		data.id="gpvdmaddjob"
		data.target=job_orig_path
		data.cpus=int(inp_get_token_value(os.path.join(get_sim_path(),"server.inp"),"#server_cpus"))
		if data.cpus==0:
			data.cpus=1
		self.tx_packet(data)

	def cluster_clean(self):
		data=tx_struct()
		data.id="gpvdm_master_clean"
		self.tx_packet(data)


	def cluster_list_jobs(self):
		data=tx_struct()
		data.id="gpvdm_send_job_list"
		self.tx_packet(data)

	def listen(self):
		#print("thread !!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
		self.running=True

		while(1):
			understood=False
			data = self.recvall(512)
			
			if data==None:
				break

			#print("command=",data,len(data))
			if data.startswith(str.encode("gpvdmfile")):
				self.rx_file(data)
				understood=True

			if data.startswith(str.encode("gpvdmpercent")):
				lines=data.split("\n")
				percent=float(inp_search_token_value(lines, "#percent"))
				self.progress_window.set_fraction(percent/100.0)
				understood=True

			if data.startswith(str.encode("gpvdmjobfinished")):
				lines=data.split("\n")
				name=inp_search_token_value(lines, "#job_name")
				self.label.set_text(gui_print_path("Finished:  ",name,60))
				understood=True

			if data.startswith(str.encode("gpvdmfinished")):
				self.stop()
				understood=True

			if data.startswith(str.encode("gpvdmheadquit")):
				self.stop()
				print("Server quit!")
				understood=True

			if data.startswith(str.encode("gpvdmnodelist")):
				self.process_node_list(data)
				self.load_update.emit()
				understood=True

			if data.startswith(str.encode("gpvdm_sync_packet_two")):
				self.process_sync_packet_two(data)
				understood=True

			if data.startswith(str.encode("gpvdm_sync_packet_one")):
				self.process_sync_packet_one(data)
				understood=True

			if data.startswith(str.encode("gpvdm_job_list")):
				self.process_job_list(data)
				understood=True

			if data.startswith(str.encode("gpvdm_message")):
				d=data.decode('UTF-8')
				d=d.split("\n")
				message=inp_search_token_value(d, "#message")
				print("message:",message)
				understood=True

			if understood==False:
				print("Command ",data, "not understood")
				
