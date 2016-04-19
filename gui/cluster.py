#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
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


########################################################################
#
# Clustering code for gpvdm
#
########################################################################
#
# This is some code I wrote to make gpvdm run as part of a cluster,
# it is useful for doing frequency domain calculations where lots
# of simulations are needed.  The code is very unstable/buggy/insecure,
# if you want to use it, I recommend getting the latest copy from
# github.  Then to enable it you will have to turn the value
# #enable_cluster in the file sim.gpvdm/ver.inp from "no" to "yes".
# Start gpvdm with the "--client" option on each node, and with the
# "--server" option on the head node.  Use at your own risk!
# 
# The server code sends data to the clients and tells them what to do,
# the server then reports back to the gpvdm interface once jobs are
# finished.  You will also have to set the "#cluster" to "1" and set
# the IP of the server in the sim.gpvdm/server.inp file.
#
# There is also *no* security on the networking code so I recommend
# running it somewhere where there is no route to the internet.
#
#########################################################################

def print_cluster_warning():
	print "Warning you have started gpvdm with clustering enabled!!!"
	print "Did you really want to do this?"
	print "#"
	print "########################################################################"
	print "#"
	print "# Clustering code for gpvdm"
	print "#"
	print "########################################################################"
	print "#"
	print "# This is some code I wrote to make gpvdm run as part of a cluster,"
	print "# it is useful for doing frequency domain calculations where lots"
	print "# of simulations are needed.  The code is very unstable/buggy/insecure,"
	print "# if you want to use it, I recommend getting the latest copy from"
	print "# github (as you are going to have to hack it around to make it work."
	print "# on your system/network) Then to enable it you will have to turn the value"
	print "# #enable_cluster in the file sim.gpvdm/ver.inp from \"no\" to \"yes\"."
	print "# Start gpvdm with the \"--client\" option on each node, and with the"
	print "# \"--server\" option on the head node.  Use at your own risk!"
	print "# "
	print "# The server code sends data to the clients and tells them what to do,"
	print "# the server then reports back to the gpvdm interface once jobs are"
	print "# finished.  You will also have to set the \"#cluster\" to \"1\" and set"
	print "# the IP of the server in the sim.gpvdm/server.inp file for the interface"
	print "# to connect to the server."
	print "#"
	print "# There is also *no* security on the networking code so I recommend"
	print "# running it somewhere where there is no route to the internet. The clients"
	print "# can execute code, delete files etc etc.. as you would"
	print "# expect from any clustering software.  Therefore if you want to run it"
	print "# run it on a system with *no internet access*."
	print "#"
	print "#########################################################################"

