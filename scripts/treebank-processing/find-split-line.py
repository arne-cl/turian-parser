#!/usr/bin/python
#
#######################################################################
#
#
#   find-split-line.py
#
#   USAGE: ./find-split-line.py treefile splitfactor > treefile-split
#
#   Read in trees and count the number of decisions therein (i.e. number
#   of constituent). Read the trees again, outputting them as we go,
#   and stop when we've read at least splitfactor * number of decisions.
#
#   TODO: Rename me to partition-treebank.py
#
#   $Id: find-split-line.py 1657 2006-06-04 03:03:05Z turian $
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
	assert len(sys.argv) == 3

	sentence = 0
	f = open(sys.argv[1], "rt")
	total_actions = 0
	for l in f:
		sentence += 1
		if not l: assert(0)

		tree = parsetree.read_tree(l)
		assert tree != None
		assert string.strip(tree.to_string()) == string.strip(l)

		total_actions += len(tree.internal_nodes())
		if sentence % 100 == 0:
			debug(1, "Sentence #%d done (first pass)" % sentence)
		else:
			debug(2, "Sentence #%d done (first pass)" % sentence)
	f.close()

	keep_actions = float(sys.argv[2]) * total_actions
	debug(1, "Wish to keep %.2f actions out of %d" % (keep_actions, total_actions))

	split_sentence = 0
	f = open(sys.argv[1], "rt")
	split_actions = 0
	for l in f:
		split_sentence += 1
		if not l: assert(0)

		tree = parsetree.read_tree(l)
		assert tree != None
		assert string.strip(tree.to_string()) == string.strip(l)

		split_actions += len(tree.internal_nodes())
		print string.strip(l)
		if split_actions >= keep_actions:
			break
	f.close()

	debug(1, "Kept %.2f%% (%d of %d) sentences, %.2f%% (%d) actions versus %.2f%% (%.2f) desired)" % (100.*split_sentence/sentence, split_sentence, sentence, 100.*split_actions/total_actions, split_actions, 100.*float(sys.argv[2]), keep_actions))

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
