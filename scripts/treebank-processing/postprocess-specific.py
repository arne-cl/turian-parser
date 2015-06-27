#!/usr/bin/python
#
#######################################################################
#
#
#   postprocess-specific.py
#
#   USAGE: ./postprocess-specific.py [--dont-reverse-Stransform] treebank.gld treebank.jmx < treebank.parsed
#
#   Postprocess trees.
#   Postprocess a tree against the original treebank tree, such that
#   EVALB can compare the parsed tree with the original tree.
#
#   WRITEME: Describe more specifically.
#
#
#   $Id: postprocess-specific.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################


from variables import *
import parsetree

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
	reverse_Stransform = True
	if len(sys.argv) == 4:
		assert(sys.argv[1]) == "--dont-reverse-Stransform"
		reverse_Stransform = False
		sys.argv = sys.argv[1:]
		debug(1, "NOT reversing Stransform.")

	assert(len(sys.argv) == 3)
	debug(1, "Opening files:\n\t%s\n\t%s\n" % (sys.argv[1], sys.argv[2]))
	treebank_file = open(sys.argv[1])
	jmx_file = open(sys.argv[2])

	sentence = 0
	for l in sys.stdin:
		sentence += 1
		if not l: assert(0)
		if l == "\n":
			lt = treebank_file.readline()
			lj = jmx_file.readline()
			print
			continue

		tree = parsetree.read_tree(l)
		assert tree != None

		if duplicate_top_item:
			assert(0)
			# Add a second TOP label, s.t. we can raise punctuation
			# above the first TOP label
			node = parsetree.Node()
			node.isleaf = 0
			node.label = "TOP"
			node.children = [tree]
			tree = parsetree.refresh(node)
#		else:
#			tree = parsetree.refresh(tree)

		lt = treebank_file.readline()
		treebank_tree = parsetree.read_tree(lt)
		assert treebank_tree != None
		treebank_tree = parsetree.reverse_regularize(treebank_tree)
		treebank_leaves = [(n.headword, n.headtag) for n in treebank_tree.leaves()]
		del treebank_tree

		lj = jmx_file.readline()
		jmx_tree = parsetree.read_tree(lj)
		assert jmx_tree != None
		jmx_tree = parsetree.reverse_regularize(jmx_tree)
		jmx_leaves = [(n.headword, n.headtag) for n in jmx_tree.leaves()]
		del jmx_tree

		tree = parsetree.reverse_regularize(tree)
		treestr = tree.to_string()
		tree = parsetree.reverse_regularize(tree)
		assert treestr == tree.to_string()

		parsetree.postprocess(tree, origleaves=jmx_leaves, treebank_leaves=treebank_leaves, reverse_Stransform=reverse_Stransform)
		print tree.to_string()

		if sentence % 100 == 0:
			debug(1, "Sentence #%d done" % sentence)
		else:
			debug(2, "Sentence #%d done" % sentence)

	assert(not treebank_file.readline())
	assert(not jmx_file.readline())
	treebank_file.close()
	jmx_file.close()

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
