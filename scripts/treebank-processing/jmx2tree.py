#!/usr/bin/python -u
#
#######################################################################
#
#
#   jmx2tree.py
#
#   Convert JMX-style POS-tagged sentences to completely flat Penn
#   Treebank-style trees.
#
#   USAGE: ./jxm2tree.py < jmx.txt > jmx-trees.txt
#
#   $Id: jmx2tree.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################


import string,sys

for l in sys.stdin:
	toks = string.split(l)
	lst = []
	for t in toks:
		(word, tag) = string.split(t, "_")
		lst.append("(%s %s)" % (tag, word))
	str = "(TOP %s)" % string.join(lst, " ")
	print str
