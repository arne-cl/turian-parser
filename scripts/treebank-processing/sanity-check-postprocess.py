#!/usr/bin/python
#
#######################################################################
#
#
#   sanity-check-postprocess.py
#
#   Sanity check that we can read in a parse tree, preprocess it, and
#   postprocess it without any issues.
#   [WRITEME: More details]
#
#   $Id: sanity-check-postprocess.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################


from variables import *

import parsetree

import math

import random
random.seed(0)

mode = "parse"
discard40 = False

min_conf = None
sum_loss = None
action_cnt = None

origtreestr = None

tree = None
treebank_leaves = None
origleaves = None

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

constits = None
sentence = None
step = None

def main():
	global mode
	global sentence
	global constits
	global tree
	global treebank_leaves
	global origleaves
	global discard40

	assert len(sys.argv) == 3

	debug(1, "Opening treebank: %s" % sys.argv[1])
	debug(1, "Opening parser output: %s" % sys.argv[2])
	treebank_file = open(sys.argv[1])
	parser_file = open(sys.argv[2])

	sentence = 0
	for l in parser_file:
		sentence += 1
		if not l: assert(0)

		lt = treebank_file.readline()
		treebank_tree = parsetree.read_tree(lt)
		assert treebank_tree != None
		treebank_leaves = [(n.headword, n.headtag) for n in treebank_tree.leaves()]
		del treebank_tree

		tree = parsetree.read_tree(l)
		assert tree != None
		origleaves = [(n.headword, n.headtag) for n in tree.leaves()]

#		print lt, l

		# Get another copy of this tree
		faketree = parsetree.read_tree(l)
		faketree = parsetree.regularize(faketree)
		if duplicate_top_item:
			# Add a second TOP label, s.t. we can raise punctuation
			# above the first TOP label
			node = parsetree.Node()
			node.isleaf = 0
			node.label = "TOP"
			node.children = [faketree]
			faketree = parsetree.refresh(node)
		else:
			faketree = parsetree.refresh(tree)

		# Preprocess and postprocess the tree, to get the
		# "ideal" parse.
		# (although there may be some differences between
		# this and the treebank parse, insofar as the
		# former will have not duplicate unary projections
		# to self, and because tagging might change whether
		# some terminals are punctuation or not.)
		faketree = parsetree.preprocess(faketree)
		fakestrpre = faketree.to_string()
		faketree = parsetree.reverse_regularize(faketree)
		faketree = parsetree.postprocess(faketree, origleaves=origleaves, treebank_leaves=origleaves)
		fakestrpost = faketree.to_string()
		assert(`[(n.headword, n.headtag) for n in faketree.leaves()]` == `origleaves`)

		faketree = parsetree.preprocess(faketree)
		assert faketree.to_string() == fakestrpre
		faketree = parsetree.postprocess(faketree, origleaves=origleaves, treebank_leaves=origleaves)
		assert faketree.to_string() == fakestrpost
		assert(`[(n.headword, n.headtag) for n in faketree.leaves()]` == `origleaves`)



		# Get another copy of this tree
		faketree = parsetree.read_tree(l)
		if duplicate_top_item:
			# Add a second TOP label, s.t. we can raise punctuation
			# above the first TOP label
			node = parsetree.Node()
			node.isleaf = 0
			node.label = "TOP"
			node.children = [faketree]
			faketree = parsetree.refresh(node)
		else:
			faketree = parsetree.refresh(tree)

		# Preprocess and postprocess the tree, to get the
		# "ideal" parse.
		# (although there may be some differences between
		# this and the treebank parse, insofar as the
		# former will have not duplicate unary projections
		# to self, and because tagging might change whether
		# some terminals are punctuation or not.)
		faketree = parsetree.preprocess(faketree)
		faketree = parsetree.postprocess(faketree, origleaves=origleaves, treebank_leaves=treebank_leaves)
		assert(`[(n.headword, n.headtag) for n in faketree.leaves()]` == `treebank_leaves`)

		if sentence % 10 == 0:
			debug(1, "Sentence #%d done" % sentence)


	assert(not treebank_file.readline())
	treebank_file.close()
	parser_file.close()

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
