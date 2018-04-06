#! /usr/bin/env python3

"""An extension that ensures that given features are present."""
import os
import sys

def log(text):
	f=open("out.dat", "a")
	f.write(text+"\n")
	f.close()
