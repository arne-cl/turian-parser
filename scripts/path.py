#!/usr/bin/python
#
#
#######################################################################
#
#
#   path.py
#
#   Slick functions for manipulating paths.
#
#   $Id: path.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

# Add the modules dir to the PYTHONPATH
import sys, string
sys.path.append("~/common/python_modules/")

from host import *
host = get_host()

import os.path
from os.path import isdir, commonprefix

base = None

def set_base(d):
	global base
	assert isdir(d)
	base = d

# Shorten the path of p by chopping off the working-directory, if present.
def shorten(p):
	assert base
	if commonprefix([p, base]) == base: return p[len(base)+1:]
	else: return p 

# Make a directory name uniform
# Super useful for user-entered directories
def wrap(dir):
	if host == 'coop' or host == 'chicken':
		return string.strip(string.replace(os.path.realpath(dir), "/clusterRoot", ""))
	else:
		assert host == 'linux' or host == 'sun'
		return string.strip(os.path.realpath(dir))
