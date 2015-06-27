#!/usr/bin/python
#
#
#######################################################################
#
#
#   refresh-links.py
#
#   USAGE: ./refresh-links.py
#
#   Run this after copying a parser directory to make sure that links
#   aren't pointing to the old directory.
#
#   1. Point all variables.py to the one in scripts/
#   2. Point all variables.pm to the one in scripts/
#   3. Remove all variables.pyc
#
#
#   $Id: refresh-links.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from variables import *

import sys, popen2, string, os

sys.stderr.write("\n\nBASEDIR = %s\n\n" % (system_dir))

goodfile = wrap(scripts_dir + "/variables.py")
if not os.access(goodfile, os.R_OK):
	sys.stderr.write("WARNING: Could not find %s\n" % (goodfile))

# Point all variables.py to the one in scripts/
(findout, findin) = popen2.popen2("find %s -name variables.py" % (system_dir))
for l in findout.readlines():
	f = wrap(l)
	if f != goodfile:
		sys.stderr.write("Linking %s to scripts/variables.py\n" % f)
		os.system("rm %s ; ln -s %s %s" % (f, goodfile, f))
findin.close()
findout.close()


goodfile = wrap(scripts_dir + "/variables.pm")
if not os.access(goodfile, os.R_OK):
	sys.stderr.write("WARNING: Could not find %s\n" % (goodfile))

# Point all variables.pm to the one in scripts/
(findout, findin) = popen2.popen2("find %s -name variables.pm" % (system_dir))
for l in findout.readlines():
	f = wrap(l)
	if f != goodfile:
		sys.stderr.write("Linking %s to scripts/variables.pm\n" % f)
		os.system("rm %s ; ln -s %s %s" % (f, goodfile, f))
findin.close()
findout.close()


# Remove all variables.pyc
(findout, findin) = popen2.popen2("find %s -name variables.pyc" % (system_dir))
for l in findout.readlines():
	f = wrap(l)
	sys.stderr.write("Removing %s\n" % f)
	os.system("rm %s" % f)
findin.close()
findout.close()

sys.stderr.write("\n\nDON'T FORGET to update code/parameters.example\n\n")
