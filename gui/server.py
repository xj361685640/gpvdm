#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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
from util import str2bool
from inp import inp_get_token_value
import threading
import multiprocessing
import time
from cal_path import get_image_file_path
from cal_path import get_exe_name

from time import sleep
from win_lin import running_on_linux
import subprocess
from util import gui_print_path
from progress import progress_class

from cal_path import get_exe_command
from sim_warnings import sim_warnings
from inp_util import inp_search_token_value
from stat import *
from encrypt import encrypt
from encrypt import decrypt
from encrypt import encrypt_load
import i18n
_ = i18n.language.gettext
from cluster import cluster


from status_icon import status_icon_init
from status_icon import status_icon_run
from status_icon import status_icon_stop

import codecs

from help import help_window
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import QWidget

from server_io import server_find_simulations_to_run

from workbook import gen_workbook
import time

my_server=False



class node:
	ip=""
	load=""
	cpus=""


class server(QWidget,cluster):
	
	sim_finished = pyqtSignal()
		
	def __init__(self):
		QWidget.__init__(self)
		self.running=False
		self.enable_gui=False
		self.callback_when_done=False
		self.display=False
		self.fit_update=None
		self.excel_workbook_gen_error=False
		status_icon_init()
		self.gui_update_time= time.time()

	def init(self,sim_dir):
		self.terminate_on_finish=False
		self.mylock=False
		self.cpus=multiprocessing.cpu_count()
		self.jobs=[]
		self.args=[]
		self.status=[]
		self.jobs_running=0
		self.jobs_run=0
		self.sim_dir=sim_dir
		self.cluster_init()
		#self.cluster=str2bool(inp_get_token_value("server.inp","#cluster"))
		self.finished_jobs=[]


	def set_terminal(self,terminal):
		self.terminal=terminal
	
	def set_display_function(self,display):
		self.display=display

	def set_fit_update_function(self,fit_update):
		self.fit_update=fit_update

	def gui_sim_start(self):
		help_window().hide()
		self.progress_window.start()
		status_icon_run(self.cluster)
		self.extern_gui_sim_start()

	def set_callback_when_done(self,proc):
		self.callback_when_done=proc

	def gui_sim_stop(self):
		text=self.check_warnings()
		self.progress_window.stop()
		help_window().show()
		status_icon_stop(self.cluster)

		help_window().help_set_help(["plot.png",_("<big><b>Simulation finished!</b></big><br>Click on the plot icon to plot the results")])

		if len(text)!=0:
			self.dialog=sim_warnings(text)

		if 	self.callback_when_done!=False:
			self.callback_when_done()
			self.callback_when_done=False

		if self.excel_workbook_gen_error==True:
			help_window().help_append(["warning.png",_("<big><b>Excel workbook error</b></big><br>I can't write new data to the file data.xlsx, I think you have are viewing it using another program.  Please close data.xlsx to enable me to write new data to it.")])

		self.sim_finished.emit()
		
	def setup_gui(self,extern_gui_sim_start):
		self.enable_gui=True
		self.extern_gui_sim_start=extern_gui_sim_start
		self.progress_window=progress_class()


	def add_job(self,path,arg):
		if self.cluster==False:
			self.jobs.append(path)
			self.args.append(arg)
			self.status.append(0)
		else:
			self.add_remote_job(path)
			self.send_dir(path,"")



	def start(self):
		self.excel_workbook_gen_error=False
		self.finished_jobs=[]
		if self.enable_gui==True:
			self.progress_window.show()
			self.gui_sim_start()

		self.running=True
		self.run_jobs()


	def print_jobs(self):
		for i in range(0, len(self.jobs)):
			print(self.jobs[i],self.arg[i],self.status[i])

		#if self.cluster==True:
		#	s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		#	port = 8888;
		#	s.sendto("print_jobs", (self.server_ip, port))
		#	s.close()


	def run_jobs(self):
		if self.cluster==True:
			self.cluster_run_jobs()


		else:
			if (len(self.jobs)==0):
				return
			for i in range(0, len(self.jobs)):
				if (self.jobs_running<self.cpus):
					if self.status[i]==0:
						self.status[i]=1
						for r in range(0,len(self.jobs)):
							print(self.jobs[i],self.args[i])
							
						print("Running job",self.jobs[i],self.args[i])
						if self.enable_gui==True:
							self.progress_window.set_text(_("Running job ")+self.jobs[i])
						self.jobs_running=self.jobs_running+1
#						if running_on_linux()==True:

						cmd="cd "+self.jobs[i]+";"
						cmd=cmd+get_exe_command()+" --lock "+"lock"+str(i)+" "+self.args[i]+" --gui --html &"
						print("command="+cmd)
						if self.enable_gui==True:
							self.terminal.run(self.jobs[i],get_exe_command()+" --lock "+"lock"+str(i)+" "+self.args[i]+" --gui --html")
						else:
							print(cmd)
							os.system(cmd)

#						else:
#							cmd=get_exe_command()+" --lock "+"lock"+str(i)+" "+self.args[i]+" --gui --html &\n"
#							print(cmd,self.jobs[i],self.args[i])
#							subprocess.Popen(cmd,cwd=self.jobs[i])
							#os.system(cmd)

							#sys.exit()

	def check_warnings(self):
		message=""
		problem_found=False
		print(len(self.jobs))

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
						#print("whoo",lines[l])
						found=found+lines[l]+"\n"
						problem_found=True
				if len(found)!=0:
					message=message+self.jobs[i]+":\n"+found+"\n"
				else:
					message=message+self.jobs[i]+":OK\n\n"
		if problem_found==False:
			message=""

		return message

	def force_stop(self):
		if running_on_linux()==True:
			cmd = 'killall '+get_exe_name()
			os.system(cmd)
			print(cmd)
		else:
			cmd="taskkill /im "+get_exe_name()
			print(cmd)
			os.system(cmd)

		self.stop()

	def stop(self):
		self.progress_window.set_fraction(0.0)

		self.gui_sim_stop()

		self.jobs=[]
		self.args=[]
		self.status=[]
		self.jobs_running=0
		self.jobs_run=0
		self.running=False

		if self.display!=False:

			self.display()
		print(_("I have shut down the server."))


	def simple_run(self):
		ls=os.listdir(self.sim_dir)
		for i in range(0, len(ls)):
			if ls[i][:4]=="lock" and ls[i][-4:]==".dat":
				del_file=os.path.join(self.sim_dir,ls[i])
				print("delete file:",del_file)
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
			data=codecs.decode(data_in, 'hex')
			data=data.decode('ascii')

			if data.startswith("lock"):
				if len(self.jobs)==0:
					print(_("I did not think I was running any jobs"))
					self.stop()
				else:
					if self.finished_jobs.count(data)==0:
						job=int(data[4:])
						self.finished_jobs.append(data)
						if str2bool(inp_get_token_value("dump.inp","#dump_workbook"))==True:
							if gen_workbook(self.jobs[job],os.path.join(self.jobs[job],"data.xlsx"))==False:
								self.excel_workbook_gen_error=self.excel_workbook_gen_error or True
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
					#self.progress_window.progress.set_pulse_step(0.01)
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
			elif (data.startswith("fit_run")):
				elapsed_time = time.time() - self.gui_update_time
				if elapsed_time>5:

					self.gui_update_time=time.time()
				
					if self.fit_update!=None:
						self.fit_update()

def server_init():
	global my_server
	my_server=server()

def server_get():
	global my_server
	return my_server

