#! /usr/bin/env python3
import os


import platform

def get_os():
	p=platform.release()
	if p.count("fc")!=0:
		return "fedora"

	return None
