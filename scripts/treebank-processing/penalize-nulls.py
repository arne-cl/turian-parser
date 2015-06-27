#!/usr/bin/python
#
#######################################################################
#
#
#   penalize-nulls.py
#
#   USAGE: ./penalize-nulls.py goldfile < testfile
#
#   Read every tree in the testfile, and for any blank line or '(null)'
#   pretend that the parser just output a TOP bracketing the entire
#   sentence.
#
#
#   $Id: penalize-nulls.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################


from variables import *
import parsetree

import sys

if mysys == "Linux" and not profile:
	debug(1, "Linux detected... using psyco")
	import psyco
	psyco.full(memory=4096)

#	psyco.full()

#	psyco.log()
#	psyco.full(memory=128)
#	psyco.profile(0.05, memory=1024)
#	psyco.profile(0.2)

elif not profile:
	debug(1, "Uname gives %s... NOT using psyco" % (mysys))
else:
	debug(1, "Profiling...")
	import hotshot, hotshot.stats

def leaves_string(tree):
	str = ""
	for l in tree.leaves():
		str += " (%s %s)" % (l.label, l.headword)
	return "(TOP %s)" % string.strip(str)

def main():
	assert(len(sys.argv) == 2)
	debug(1, "Opening files:\n\t%s\n" % (sys.argv[1]))
	gold_file = open(sys.argv[1])

	sentence = 0
	for lt in sys.stdin:
		sentence += 1

		lg = gold_file.readline()
		gold_tree = parsetree.read_tree(lg)

		if lt == "\n" or lt == "(null)\n":
			goldstr = leaves_string(gold_tree)
			sys.stderr.write("FOUND NULL! Sentence #%d\n" % sentence)
			sys.stderr.write("Output: %s\n" % goldstr)
			print goldstr
		else:
			test_tree = parsetree.read_tree(lt)
			teststr = "(%s)" % ([l.headword for l in test_tree.leaves()])
			goldstr = "(%s)" % ([l.headword for l in test_tree.leaves()])
			assert(goldstr == teststr)
			print lt,

		if sentence % 100 == 0:
			debug(1, "Sentence #%d done" % sentence)
		else:
			debug(2, "Sentence #%d done" % sentence)

	assert(not gold_file.readline())
	gold_file.close()

if profile:
	#prof = hotshot.Profile("oracle.prof", lineevents=1)
	prof = hotshot.Profile("oracle.prof")
	prof.runcall(main)
	prof.close()
	stats = hotshot.stats.load("oracle.prof")
	stats.strip_dirs()
	stats.sort_stats('time', 'calls')
	stats.print_stats(20)
else:
	main()
