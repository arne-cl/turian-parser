#!/usr/bin/python
#!/usr/bin/python2.4
#
#######################################################################
#
#
#   regularize.py
#
#   USAGE: ./regularize.py < treefile > treefile.reg
#
#   Read in a treebank and do some regularization:
#   * Flatten closed class compounds
#
#   The training treebank should be regularized *prior* to JMX tagging.
#
#   $Id: regularize.py 1657 2006-06-04 03:03:05Z turian $
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

def main():
	assert len(sys.argv) == 1

	sentence = 0
	for l in sys.stdin:
		sentence += 1

		if not l: assert(0)

		tree = parsetree.read_tree(l)
		tree = parsetree.regularize(tree)
		assert tree != None

		# SANITY CHECK:
		# Ensure that the cleaned output is "stable", i.e. that
		# this script will produce identical output if we pipe
		# cleaned output from this script back into it.
		assert tree.to_string() == parsetree.read_tree(tree.to_string()).to_string()
		print tree.to_string()

		if sentence % 100 == 0:
			debug(1, "Sentence #%d done" % sentence)
		else:
			debug(2, "Sentence #%d done" % sentence)

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
