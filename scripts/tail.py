import os
import sys
import re
import time

def tail(fname):
	while(os.path.isfile(fname)==False):
		print("wait for "+fname)

	f=open(fname,"r", encoding="latin-1")
	sys.stdout.write(f.read())
	sys.stdout.flush()
	p = f.tell()
	f.close()

	mod_time=os.stat(fname).st_mtime
	last_mod_time=mod_time
	while(1):
		mod_time=os.stat(fname).st_mtime
		if last_mod_time!=mod_time:
			f=open(fname,"r", encoding="latin-1")
			f.seek(0,2)
			size = f.tell()
			if p>size:
				p=0
			f.seek(p)
			r=f.read()
			pos=r.rfind('\n')
			if pos!=-1:
				r=r[:pos]
				f.seek(p+len(r))
				r=re.sub(r'[^\x00-\x7F]+',' ', r)
				print(r, end='', flush=True)
				p = f.tell()
			f.close()

		time.sleep(0.05) 
		if os.path.isfile(fname)==True:
			if (time.time() - mod_time)>2:
				print("tail finished..")
				break
		else:
			break

		last_mod_time=mod_time

#	input("Press Enter to continue...")
	print("")

