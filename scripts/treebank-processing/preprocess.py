#!/usr/bin/python
#
#######################################################################
#
#
#   preprocess.py
#
#   USAGE: ./preprocess.py < treefile.orig > treefile
#
#   TODO:
#   * Try to preprocess twice, to verify consistency.
#
#   Preprocess trees.
#   This is required before use the C++ parser on them.
#   Some of the following steps can be enabled/disabled by modifying
#   variables.py.:
#   1. Convert PRT to ADVP
#   2. Remove quotation marks
#   3. Raise punctuation
#   4. Remove unary projections to self
#   5. Potentially add basal NPs
#   6. Lowercase headwords
#   7. Delexicalize words with content tags
#   8. Delexicalize infrequent words
#   9. Add a duplicate TOP item
#   10. Verify that all labels are in data/labels.txt
#   11. Verify that all head words are in data/vocabulary.txt
#   12. Output the tree.
#
#
#   $Id: preprocess.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################


from variables import *

import parsetree
import vocab
vocab.read()

import sys

if mysys == "Linux" and not profile:
	try:
		debug(1, "Linux detected... using psyco")
		import psyco
		psyco.full(memory=4096)

#	psyco.full()

#	psyco.log()
#	psyco.full(memory=128)
#	psyco.profile(0.05, memory=1024)
#	psyco.profile(0.2)

	except:
		debug(1, "Could not import psyco")


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
#		# Skip blank lines
#		if not string.strip(l):
#			print
#			continue

		tree = parsetree.read_tree(l)
		assert tree != None

		# Sanity check that the tree's already been regularized.
		treestr = tree.to_string()
		tree = parsetree.regularize(tree)
		assert tree.to_string() == treestr

		if duplicate_top_item:
			# Add a second TOP label, s.t. we can raise punctuation
			# above the first TOP label
			node = parsetree.Node()
			node.isleaf = 0
			node.label = "TOP"
			node.children = [tree]
			tree = parsetree.refresh(node)
			tree = parsetree.preprocess(tree)
		else:
			tree = parsetree.preprocess(tree)

		for n in tree.leaves():
			# Make sure that the headtag is a terminal label (POS tag)
			assert vocab.label_to_idx[n.headtag][1] == 1
			# Make sure that the headword is in the vocabulary
			assert vocab.vocab_to_idx[n.headword] > 0

		for n in tree.internal_nodes():
			# Make sure that the label is a constituent label
			assert vocab.label_to_idx[n.label][1] == 0

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
