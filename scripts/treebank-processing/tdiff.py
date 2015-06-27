#!/usr/bin/python2.4
#
#######################################################################
#
#
#   tdiff.py
#
#   USAGE: ./tdiff.py treebank_in treebank_out [difftxt]
#
#   Do a diff on each pair of trees in two treebanks.
#
#   More specifically, we find all non-terminal nodes in the out-tree
#   that are not present in the in-tree, and append difftxt (default:
#   "***") to this out-node's label. We then output the updated out-tree.
#
#   If the trees are identical, a warning is output.
#
#   We assume that *all* nodes in in-tree are present in out-tree.
#   We also assume that there are no unary projections to self.
#
#   TODO:
#	* Stronger tdiff, which does not assume that in-tree is a subset
#	of out-tree. [If we want to output only one tree, what do we do
#	when both trees contain a unary node that is node present in the
#	other tree?]
#
#   $Id: tdiff.py 1657 2006-06-04 03:03:05Z turian $
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

# Convert a node to a string: "(label, span)"
def node_txt(n):
	s = n.span()
	return "(%s, (%d, %d))" % (n.label, s[0], s[1])

def main():
	difftxt = "***"
	if len(sys.argv) == 4:
		difftxt = sys.argv[3]
		sys.argv = sys.argv[:3]

	assert(len(sys.argv) == 3)
	debug(1, "Using '%s' as difftxt." % difftxt)
	debug(1, "Opening files:\n\t%s\n\t%s\n" % (sys.argv[1], sys.argv[2]))

	in_file = open(sys.argv[1])
	out_file = open(sys.argv[2])

	sentence = 0
	for lin in in_file:
		sentence += 1

		lout = out_file.readline()

		assert lin
		assert lout

		in_tree = parsetree.read_tree(lin)
		out_tree = parsetree.read_tree(lout)

		assert in_tree != None
		assert out_tree != None

		# Find all constituents in the in_tree and in the out_tree
		in_nodes = {}
                for n in in_tree.internal_nodes():
			nt = node_txt(n)
			# Check that there are no unaries to self
			assert nt not in in_nodes
			in_nodes[nt] = True
		out_nodes = {}
                for n in out_tree.internal_nodes():
			nt = node_txt(n)
			# Check that there are no unaries to self
			assert nt not in out_nodes
			out_nodes[nt] = True

		assert(len(in_nodes) <= len(out_nodes))
		# Sanity check:
		# Make sure that every constituent in the in_tree
		# is also in the out_tree
		for n in in_nodes: assert n in out_nodes

		found_diff = False

		# Find all constituents in out_tree that are not
		# in in_tree
                for n in out_tree.internal_nodes():
			nt = node_txt(n)

			# If the constituent is not present in in_tree,
			# then add difftxt to this node's label
			if nt not in in_nodes:
				found_diff = True
				n.label += difftxt
			# Otherwise, remove this node from in_tree's
			# list, since we don't want to use it twice
			# [This should have no effect if there are no
			# unary projections to self]
			else: del in_nodes[nt]

		if not found_diff:
			assert lin == lout
			assert in_tree.to_string() == out_tree.to_string()
			debug(1, "WARNING: No diff found for sentence #%d: %s" % (sentence, lin))

		print out_tree.to_string()

#		if sentence % 100 == 0:
#			debug(1, "Sentence #%d done" % sentence)
#		else:
#			debug(2, "Sentence #%d done" % sentence)

	assert(not in_file.readline())
	assert(not out_file.readline())
	in_file.close()
	out_file.close()

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
