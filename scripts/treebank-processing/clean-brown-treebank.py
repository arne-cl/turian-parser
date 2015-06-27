#!/usr/bin/python2.4
#
#!/usr/bin/python
#
#######################################################################
#
#
#   clean-brown-treebank.py
#
#   USAGE: ./clean-brown-treebank.py < treefile.orig > treefile.clean
#
#   Read in a Penn I style treebank (i.e. Brown corpus) that's been
#   preprocessed using ./scripts/joinlines-brown.pl, and
#   do some minor "cleanup":
#   * Skip any tree that just contains punctuation.
#   * Remove all internal nodes with labels that are unknown
#   (not in the constituent list), typically errant POS tags like
#   "AUX".
#   * Remove NULLs
#   * Remove -, =, : and suffixes from labels. [WRITEME: what are these called?]
#   The cleaned treebank is output. Note that this cleanup routine is
#   "stable", insofar as this script will produce identical output if we
#   pipe cleaned output from this script back into it.
#   If an input line is blank, we skip it and do *not* output it.
#
#   Cleaning a treebank will not affect its PARSEVAL score.
#
#
#   $Id: clean-brown-treebank.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################


from variables import *
import parsetree

constits = ["TOP", "NP", "VP", "S", "PP", "SBAR", "ADVP", "ADJP", "QP", "WHNP", "PRN", "PRT", "WHADVP", "SINV", "NX", "UCP", "FRAG", "NAC", "WHPP", "SQ", "CONJP", "SBARQ", "X", "INTJ", "WHADJP", "RRC", "LST"]
unknown_constits = {}
 
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

#		if not l: assert(0)
		# Skip blank lines
		if not string.strip(l):
#			print
			continue

		tree = parsetree.read_tree(l)
		assert tree != None

		# SANITY CHECK:
		# Ensure that the cleaned output is "stable", i.e. that
		# this script will produce identical output if we pipe
		# cleaned output from this script back into it.
		assert tree.to_string() == parsetree.read_tree(tree.to_string()).to_string()

		# Remove all internal nodes with labels that are unknown
		# (not in the constituent list).
		for n in tree.internal_nodes():
			if n.label not in constits:
		                p = n.parent()
				assert p != None
				p.children = n.left_siblings() + n.children + n.right_siblings()
				tree = parsetree.refresh(tree)
				if n.label not in unknown_constits:
					unknown_constits[n.label] = 1
					sys.stderr.write("Stripping unknown label: %s\n" % n.label)

		to_print = False
		for n in tree.leaves():
			if n.headword not in [":", ",", ".", "``", "''", "?", "!"]:
				to_print = True
				break

		if to_print: print tree.to_string()
		else: sys.stderr.write("Skipping all punctuation tree\n")

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
