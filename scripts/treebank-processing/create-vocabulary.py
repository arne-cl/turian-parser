#!/usr/bin/python
#!/usr/bin/python2.2
#
#!/usr/local/bin/python2.3
#
#######################################################################
#
#
#   create-vocabulary.py
#
#   Creates the labels.txt and vocabulary.txt files from treebank in stdin.
#
#   [WRITEME: describe methodology and options]
#
#   $Id: create-vocabulary.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################


from variables import *

import head
import parsetree
import postags
import query
import vocab

import math

import random
random.seed(0)

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
	vocab.init()

	sentence = 0
	for l in sys.stdin:
		sentence += 1
#		if not l: assert(0)
		# Skip blank lines
		if not string.strip(l): continue

		tree = parsetree.read_tree(l)
		assert tree != None
		if remove_quotation_marks:
			tree.prune_labels(["``", "''"])

		for n in tree.leaves():
			if lowercase_vocabulary:
				n.headword = string.lower(n.headword)
		tree = parsetree.refresh(tree)

		vocab.add(tree)
		del tree

		if sentence % 1000 == 0:
			debug(1, "Sentence #%d done" % sentence)
		elif sentence % 100 == 0:
			debug(2, "Sentence #%d done" % sentence)

	vocab.write()

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
