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

import i18n
_ = i18n.language.gettext

def server_find_simulations_to_run(commands,search_path):
	for root, dirs, files in os.walk(search_path):
		for my_file in files:
			if my_file.endswith("sim.gpvdm")==True:
#				full_name=os.path.join(root, my_file)
				commands.append(root)

class server:
	def __init__(self):
		self.running=False
		self.thread_data=[""]
		self.enable_gui=False

		self.statusicon = gtk.StatusIcon()
		self.statusicon.set_from_stock(gtk.STOCK_YES)
		#self.statusicon.connect("popup-menu", self.right_click_event)
		self.statusicon.set_tooltip("gpvdm")

	def connect(self):
		if self.socket==False:

			self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.socket.connect((self.server_ip, 50002))
			self.cluster=True

			buf=bytearray(512)
			header="gpvdmregistermaster\n"
			for i in range(0,len(header)):
				buf[i]=header[i]

			self.socket.send(buf)


			if self.running==False:
				self.mylock=False
				self.thread = threading.Thread(target = self.listen)
				self.thread.daemon = True
				self.thread.start()

			print "conected to cluster"
		else:
			self.socket.close()
			self.thread.stop()
			self.cluster=False
			print "not conected to cluster"

	def init(self,sim_dir):
		self.terminate_on_finish=False
		self.mylock=False
		self.cpus=multiprocessing.cpu_count()
		self.jobs=[]
		self.status=[]
		self.jobs_running=0
		self.jobs_run=0
		self.sim_dir=sim_dir
		#self.cluster=str2bool(inp_get_token_value("server.inp","#cluster"))
		self.server_ip=inp_get_token_value("server.inp","#server_ip");
		self.finished_jobs=[]
		self.socket = False
		self.cluster=False

	def set_terminal(self,terminal):
		self.terminal=terminal

	def gui_sim_start(self):
		self.progress_window.start()
		self.statusicon.set_from_stock(gtk.STOCK_NO)
		self.extern_gui_sim_start()

	def gui_sim_stop(self):
		text=self.check_warnings()
		self.progress_window.stop()
		self.statusicon.set_from_stock(gtk.STOCK_YES)
		self.extern_gui_sim_stop("Finished simulation")
		my_help_class.help_set_help(["plot.png",_("<big><b>Simulation finished!</b></big>\nClick on the plot icon to plot the results")])
		print text
		if len(text)!=0:
			dialog=sim_warnings()
			dialog.init(text)
#			response=dialog.run()
			dialog.destroy()


	def setup_gui(self,extern_gui_sim_start,extern_gui_sim_stop):
		self.enable_gui=True
		self.extern_gui_sim_start=extern_gui_sim_start
		self.extern_gui_sim_stop=extern_gui_sim_stop
		self.progress_window=progress_class()
		self.progress_window.init()


	def add_job(self,path):
		if self.cluster==False:
			self.jobs.append(path)
			self.status.append(0)
		else:
			self.add_remote_job(path)
			self.send_dir(path)
			#s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
			#port = 8888;
			#s.sendto("addjobs#"+command, (self.server_ip, port))
			#s.close()

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

	def start(self):
		self.finished_jobs=[]
		if self.enable_gui==True:
			self.progress_window.show()
			self.gui_sim_start()
		#self.statusicon.set_from_stock(gtk.STOCK_NO)
		self.running=True
		self.run_jobs()

	def process_node_list(self,data):
		data = self.socket.recv(512)
		print data

	def rx_file(self,data):
		pwd=os.getcwd()
		lines=data.split("\n")
		name=inp_search_token_value(lines, "#file_name")
		size=int(inp_search_token_value(lines, "#file_size"))
		target=inp_search_token_value(lines, "#target")
		print target,name
		target=target+name
		print "write to",target
		if target.startswith(pwd):
			written=0
			LENGTH=512
			f = open(target, "wb")

			while(1):
				data = self.socket.recv(512)


				#if (left<LENGTH):
				#	write_block_size=left
				#else:
				#	write_block_size=LENGTH

				f.write(data)
				written=written+512;

				if written>=size:
					break;
			f.close()
		else:
			print "target mismatch",pwd,target
		
	def listen(self):
		#print "thread !!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		self.running=True

		while(1):
			#print "wait"
			data = self.socket.recv(512)
			#print "command=",data,len(data)
			if data.startswith("gpvdmfile"):
				self.rx_file(data)

			if data.startswith("gpvdmpercent"):
				lines=data.split("\n")
				percent=float(inp_search_token_value(lines, "#percent"))
				self.progress_window.set_fraction(percent/100.0)

			if data.startswith("gpvdmjobfinished"):
				lines=data.split("\n")
				name=inp_search_token_value(lines, "#job_name")
				self.label.set_text(gui_print_path("Finished:  ",name,60))
			if data.startswith("gpvdmfinished"):
				self.stop()
				sys.exit()

			if data.startswith("gpvdmheadquit"):
				self.stop()
				print "Server quit!"

			if data.startswith("gpvdmnodelist"):
				self.process_node_list(data)

	def set_wait_bit(self):
		self.opp_finished=False


	def print_jobs(self):
		for i in range(0, len(self.jobs)):
			print self.jobs[i],self.status[i]

		#if self.cluster==True:
		#	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		#	port = 8888;
		#	s.sendto("print_jobs", (self.server_ip, port))
		#	s.close()

	def cluster_get_data(self):
		if self.cluster==True:
			buf=bytearray(512)
			header="gpvdmgetdata\n"
			for i in range(0,len(header)):
				buf[i]=header[i]

			print header
			self.socket.send(buf)

	def cluster_get_info(self):
		if self.cluster==True:
			buf=bytearray(512)
			header="gpvdmsendnodelist\n"
			for i in range(0,len(header)):
				buf[i]=header[i]

			print header
			self.socket.send(buf)

	def killall(self):
		if self.cluster==True:
			buf=bytearray(512)
			header="gpvdmkillall\n"
			for i in range(0,len(header)):
				buf[i]=header[i]

			self.socket.send(buf)



		else:
			print "stop jobs"
			if running_on_linux()==True:
				exe_name=os.path.basename(get_exe_command())
				cmd = 'killall '+exe_name
				ret= os.system(cmd)

		self.stop()


	def sleep(self):
		if self.cluster==True:
			buf=bytearray(512)
			header="gpvdmsleep"
			for i in range(0,len(header)):
				buf[i]=header[i]

			self.socket.send(buf)

	def poweroff(self):
		if self.cluster==True:
			buf=bytearray(512)
			header="gpvdmpoweroff"
			for i in range(0,len(header)):
				buf[i]=header[i]

			self.socket.send(buf)


	def wait_lock(self):
		print "Waiting for cluster..."
		while(self.mylock==True):
			sleep(0.1)

	def add_remote_job(self,job_orig_path):
		buf=bytearray(512)
		header="gpvdmaddjob\n#target\n"+job_orig_path+"\n#end"
		for i in range(0,len(header)):
			buf[i]=header[i]

		self.socket.send(buf)

	def send_dir(self,path):

		for root, dirs, files in os.walk(path):
			for name in files:
				fname=os.path.join(root, name)

				print "sending", fname
				f = open(fname, 'rb')   
				bytes = f.read()
				size=len(bytes)
				f.close()

				#build header
				tx_name= os.path.normpath(fname[len(path)+1:])

				buf=bytearray(512)
				header="gpvdmfile\n#file_name\n"+tx_name+"\n#file_size\n"+str(size)+"\n#target\n"+path+"\n#end"
				for i in range(0,len(header)):
					buf[i]=header[i]

				self.socket.send(buf)

				expand=((int(size)/int(512))+1)*512-size
				bytes+= "\0" * expand
				self.socket.send(bytes)


	def run_jobs(self):
		print ">>>>>>>>>>>>>>>>>>>>>>>>>",self.cluster

		if self.cluster==True:
			buf=bytearray(512)
			header="gpvdmrunjobs"
			for i in range(0,len(header)):
				buf[i]=header[i]

			self.socket.send(buf)

		else:
			if (len(self.jobs)==0):
				return
			for i in range(0, len(self.jobs)):
				if (self.jobs_running<self.cpus):
					if self.status[i]==0:
						self.status[i]=1
						print "Running job",self.jobs[i]
						if self.enable_gui==True:
							self.progress_window.set_text("Running job"+self.jobs[i])
						self.jobs_running=self.jobs_running+1
						if running_on_linux()==True:
							cmd="cd "+self.jobs[i]+";"
							cmd=cmd+get_exe_command()+" --lock "+"lock"+str(i)+" &\n"
							print "command="+cmd
							if self.enable_gui==True:
								self.terminal.feed_child(cmd)
							else:
								print cmd
								os.system(cmd)

						else:
							cmd=get_exe_command()+" --lock "+"lock"+str(i)+" &\n"
							print cmd,self.jobs[i]
							subprocess.Popen(cmd,cwd=self.jobs[i])
							#os.system(cmd)

							#sys.exit()
	def check_warnings(self):
		message=""
		problem_found=False
		for i in range(0,len(self.jobs)):
			log_file=os.path.join(self.jobs[i],"log.dat")
			if os.path.isfile(log_file):
				f = open(log_file, "r")
				lines = f.readlines()
				f.close()
				found=""
				for l in range(0, len(lines)):
					lines[l]=lines[l].rstrip()
					if lines[l].startswith("error:") or lines[l].startswith("warning:"):
						found=found+lines[l]+"\n"
						problem_found=True
				if len(found)!=0:
					message=message+self.jobs[i]+":\n"+found+"\n"
				else:
					message=message+self.jobs[i]+":OK\n\n"
		if problem_found==False:
			message=""

		return message


	def stop(self):

		self.progress_window.set_fraction(0.0)
		self.running=False

		self.gui_sim_stop()

		self.jobs=[]
		self.status=[]
		self.jobs_running=0
		self.jobs_run=0
		print _("I have shut down the server.")


	def simple_run(self):
		ls=os.listdir(self.sim_dir)
		for i in range(0, len(ls)):
			if ls[i][:4]=="lock" and ls[i][-4:]==".dat":
				del_file=os.path.join(self.sim_dir,ls[i])
				print "delete file:",del_file
				os.remove(del_file)
		self.run_jobs()

		while(1):
			ls=os.listdir(self.sim_dir)
			for i in range(0, len(ls)):
				if ls[i][:4]=="lock" and ls[i][-4:]==".dat":
					lock_file=ls[i]
					os.remove(os.path.join(self.sim_dir,lock_file))
					self.jobs_run=self.jobs_run+1
					self.jobs_running=self.jobs_running-1
			self.run_jobs()
			time.sleep(0.1)

			if self.jobs_run==len(self.jobs):
				break

	def callback_dbus(self,data_in):
		if data_in.startswith("hex"):
			data_in=data_in[3:]
			data=data_in.decode("hex")
			#print "dbus:",data
			if data.startswith("lock"):
				if str(data)>4:
					test=data[:4]
					if test=="lock":
						if self.finished_jobs.count(data)==0:
							self.finished_jobs.append(data)
#							rest=data[4:]
							self.jobs_run=self.jobs_run+1
							self.jobs_running=self.jobs_running-1
							self.progress_window.set_fraction(float(self.jobs_run)/float(len(self.jobs)))
							self.run_jobs()
							if (self.jobs_run==len(self.jobs)):
								self.stop()

			elif (data=="pulse"):
				if len(self.jobs)==1:
					splitup=data.split(":")
					if len(splitup)>1:
						text=data.split(":")[1]
						self.progress_window.set_text(text)
					self.progress_window.progress.set_pulse_step(0.01)
					self.progress_window.pulse()
			elif (data.startswith("percent")):
				if len(self.jobs)==1:
					splitup=data.split(":")
					if len(splitup)>1:
						frac=float(data.split(":")[1])
						self.progress_window.set_fraction(frac)
			elif (data.startswith("text")):
				if len(self.jobs)==1:
					splitup=data.split(":")
					if len(splitup)>1:
						self.progress_window.set_text(data.split(":")[1])




