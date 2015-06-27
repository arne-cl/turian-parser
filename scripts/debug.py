#
#   debug.py
#
#   Debug messages.
#
#   $Id: debug.py 1657 2006-06-04 03:03:05Z turian $
#
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import realtime
import sys

def set_debug_level(x):
	global debug_level
	debug_level = x

def error(msg):
	sys.stderr.write("ERROR occurred at %s:\n%s\n" % (realtime.elapsed_str(), msg))

def debug(lvl, msg):
	if lvl <= debug_level:
		sys.stderr.write("%s: %s\n" % (realtime.elapsed_str(), msg))
