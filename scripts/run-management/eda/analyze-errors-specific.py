#!/usr/bin/python
#
#######################################################################
#
#
#   analyze-errors-specific.py
#
#   USAGE: ./analyze-errors-specific.py goldfile parsefile
#
#   Given a postprocessed treebank, create a breakdown of PARSEVAL scores
#   by constituent type, sorted by those type with the largest share of
#   the error.
#
#   Types of constituents we are analyzing:
#   * All constituents
#   * Constituents broken down by label
#   * Constituents broken down by number of children
#   * Constituents broken down by label, number of children
#
#   WRITEME: Describe more specifically.
#
#
#   $Id: analyze-errors-specific.py 1657 2006-06-04 03:03:05Z turian $
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
	gold_filename = sys.argv[1]
	parse_filename = sys.argv[2]
	debug(1, "Opening files:\n\t%s\n\t%s\n" % (gold_filename, parse_filename))
	gold_file = open(gold_filename)
	parse_file = open(parse_filename)

	sentence = 0

	all_types = {}

	test_constits = {}
	gold_constits = {}

	test_constits_totals = {}
	gold_constits_totals = {}

	for l in parse_file:
		sentence += 1
		if not l: assert(0)

		test_tree = parsetree.read_tree(l)
		assert test_tree != None
		test_tree = parsetree.normalize(test_tree)
		test_leaves = [(n.headword, n.headtag) for n in test_tree.leaves()]

		lt = gold_file.readline()
		gold_tree = parsetree.read_tree(lt)
		assert gold_tree != None
		gold_tree = parsetree.normalize(gold_tree)
		gold_leaves = [(n.headword, n.headtag) for n in gold_tree.leaves()]

		# Make sure we're comparing the same sentences
		assert test_leaves == gold_leaves

		for n in test_tree.internal_nodes():
			if n.label == "TOP": continue

			s = n.span()
			c = "Sentence #%d: %s @ [%d, %d]" % (sentence, n.label, s[0], s[1])

			# Types of constituents we are analyzing:
			#	* All constituents
			#	* Constituents broken down by label
			#	* Constituents broken down by number of children
			#	* Constituents broken down by label, number of children
#			types = ["all", "label %s" % n.label, "%d children" % len(n.children), "label %s with %d children" % (n.label, len(n.children))]
			types = ["all", "label %s" % n.label, "%d children" % len(n.children)]

			for t in types:
				all_types[t] = 1
				if t not in test_constits:
					test_constits[t] = {}
					test_constits_totals[t] = 0
				if c not in test_constits[t]: test_constits[t][c] = 0
				test_constits_totals[t] += 1
				test_constits[t][c] += 1

		for n in gold_tree.internal_nodes():
			if n.label == "TOP": continue

			s = n.span()
			c = "Sentence #%d: %s @ [%d, %d]" % (sentence, n.label, s[0], s[1])

			# Types of constituents we are analyzing:
			#	* All constituents
			#	* Constituents broken down by label
			#	* Constituents broken down by number of children
			#	* Constituents broken down by label, number of children
#			types = ["all", "label %s" % n.label, "%d children" % len(n.children), "label %s with %d children" % (n.label, len(n.children))]
			types = ["all", "label %s" % n.label, "%d children" % len(n.children)]

			for t in types:
				all_types[t] = 1
				if t not in gold_constits:
					gold_constits[t] = {}
					gold_constits_totals[t] = 0
				if c not in gold_constits[t]: gold_constits[t][c] = 0
				gold_constits_totals[t] += 1
				gold_constits[t][c] += 1

		if sentence % 100 == 0:
			debug(1, "Sentence #%d done" % sentence)
		else:
			debug(2, "Sentence #%d done" % sentence)

	gsum = 0
	tsum = 0
	msum = 0
	# FIXME: Don't hardcode this
	for i in range(128):
		t = "%d children" % i
		if t in gold_constits_totals:
			gsum += gold_constits_totals[t]
		if t in test_constits_totals:
			tsum += test_constits_totals[t]
	assert gsum == gold_constits_totals["all"]
	assert tsum == test_constits_totals["all"]

	alltot = test_constits_totals["all"] + gold_constits_totals["all"]
	print "Total constituents in test + gold: %d" % alltot

	all_error_fms = 0.
	nonall_error_fms = 0.

	sorted_types = []
	for t in all_types:
		if t not in test_constits:
			test_constits[t] = {}
			test_constits_totals[t] = 0
		if t not in gold_constits:
			gold_constits[t] = {}
			gold_constits_totals[t] = 0

		tot = test_constits_totals[t] + gold_constits_totals[t]

		str = ""
		str += "\n"
		str += "Breakdown type: %s\n" % t
		str += "comprising %.2f%% (%d/%d) of all constituents\n" % (100.*tot/alltot, tot, alltot)

		testmatch = 0
		goldmatch = 0
		testtot = 0
		goldtot = 0

		allkeys = {}
		for k in test_constits[t].keys() + gold_constits[t].keys():
			allkeys[k] = True
		for c in allkeys:
			testmatch += min(test_constits[t].get(c,0), gold_constits["all"].get(c,0))
			goldmatch += min(test_constits["all"].get(c,0), gold_constits[t].get(c,0))
			goldtot += gold_constits[t].get(c,0)
			testtot += test_constits[t].get(c,0)

		assert goldtot == gold_constits_totals[t]
		assert testtot == test_constits_totals[t]

		# BUG: These error FMS are all skewed!
		# To see this, observe the "VP with 2 children" has more attributed error than just "VP"

		#error_fms = 1. * (testtot + goldtot - 2 * match) / alltot
		error_fms = 1. * (testtot + goldtot - goldmatch - testmatch) / alltot
		str += "overall error incurred by this constituent type = %.3f%% (%d/%d)\n" % (100.*error_fms, testtot + goldtot - goldmatch - testmatch, alltot)
		#errprc = 1. * (testtot - match) / test_constits_totals["all"]
		#errrcl = 1. * (goldtot - match) / gold_constits_totals["all"]
		##errprc = 1. * (test_constits_totals["all"] - testtot + match) / test_constits_totals["all"]
		##errrcl = 1. * (gold_constits_totals["all"] - goldtot + match) / gold_constits_totals["all"]
		#if errprc == 0 or errrcl == 0: error_fms = 0
		#else: error_fms = 1-2*errrcl*errprc/(errrcl+errprc)
		#str += "overall error incurred by this constituent type = %.3f%%\n" % (100.*error_fms)
		#str += "overall PRC error incurred by this constituent type = %.3f%% (%d/%d)\n" % (100.*errprc,testtot - match, test_constits_totals["all"])
		#str += "overall RCL error incurred by this constituent type = %.3f%% (%d/%d)\n" % (100.*errrcl,goldtot - match, gold_constits_totals["all"])

		if t == "all":
			all_error_fms += error_fms
		else:
			nonall_error_fms += error_fms

		if testtot == 0: lprc = 0
		else: lprc = 1. * testmatch / testtot

		if goldtot == 0: lrc = 0
		else: lrcl = 1. * goldmatch / goldtot

		if lprc == 0 or lrcl == 0: lfms = 0
		else: lfms = 2*lrcl*lprc/(lrcl+lprc)
		str += "LFMS = %.3f%%\n" % (100. * lfms)
		str += "LPRC = %.3f%% (%d/%d)\n" % (100. * lprc, testmatch, testtot)
		str += "LRCL = %.3f%% (%d/%d)\n" % (100. * lrcl, goldmatch, goldtot)

		sorted_types.append((error_fms, str))

	sorted_types.sort()
	sorted_types.reverse()
	for (error_fms, str) in sorted_types:
		print str

	assert(not gold_file.readline())
	gold_file.close()
	parse_file.close()

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
