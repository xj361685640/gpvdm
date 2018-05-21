import os
import sys
import pyinotify
import re
import time

class ModHandler(pyinotify.ProcessEvent):
	def __init__(self,file_name):
		self.fname=file_name
		self.p=0
		try:
			self.f=open(self.fname,"r", encoding="latin-1")
			sys.stdout.write(self.f.read())
			sys.stdout.flush()
			self.p = self.f.tell()
			self.f.close()
		except:
			pass

	def process_IN_MODIFY(self, evt):
		try:
			self.f=open(self.fname,"r", encoding="latin-1")
			self.f.seek(0,2)
			size = self.f.tell()
			if self.p>size:
				self.p=0
			self.f.seek(self.p)
			r=self.f.read()
			pos=r.rfind('\n')
			if pos!=-1:
				r=r[:pos]
				self.f.seek(self.p+len(r))
				r=re.sub(r'[^\x00-\x7F]+',' ', r)
				print(r, end='', flush=True)
				#sys.stdout.write(">"+r+"<")
				#sys.stdout.flush()
				self.p = self.f.tell()
			self.f.close()
		except:
			pass

def tail(fname):
	handler = ModHandler(fname)
	wm = pyinotify.WatchManager()
	notifier = pyinotify.ThreadedNotifier(wm, handler)
	notifier.daemon=True
	wdd = wm.add_watch(fname, pyinotify.ALL_EVENTS)

	notifier.start()

	while(1):
		time.sleep(1.0) 
		if os.path.isfile(fname)==True:
			if (time.time() - os.stat(fname).st_mtime)>2:
				break
		else:
			break

#	input("Press Enter to continue...")
	print("")
	notifier.stop()

