#!/usr/bin/python
#
#######################################################################
#
#   DEPRECATED IN FAVOR OF skip-long-sentences.pl, WHICH USES EVALB
#
#######################################################################
#
#   skip-long-sentences.py
#
#   USAGE: ./skip-long-sentences.py < treefile.orig > treefile
#
#   Read in trees, and skip (output a blank line for) those with more
#   words than variables.max_words_per_sentence
#
#   $Id: skip-long-sentences.py 1657 2006-06-04 03:03:05Z turian $
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
	skip_sentence = 0
	for l in sys.stdin:
		sentence += 1
		if not l: assert(0)

		tree = parsetree.read_tree(l)
		assert tree != None
#		print string.strip(tree.to_string())
#		print string.strip(l)
#		assert string.strip(tree.to_string()) == string.strip(l)

		origleaves = [(n.headword, n.headtag) for n in tree.leaves()]
		if len(origleaves) > max_words_per_sentence:
			skip_sentence += 1
#			print
		else:
			print string.strip(l)

		if sentence % 100 == 0:
			debug(1, "Sentence #%d done" % sentence)
		else:
			debug(2, "Sentence #%d done" % sentence)

	keep_sentence = sentence - skip_sentence
	debug(1, "Kept %.2f%% (%d of %d) sentences with at most %d words" % (100.*keep_sentence/sentence, keep_sentence, sentence, max_words_per_sentence))
	debug(1, "i.e. skipped %.2f%% (%d of %d) sentences with more that %d words" % (100.*skip_sentence/sentence, skip_sentence, sentence, max_words_per_sentence))

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
