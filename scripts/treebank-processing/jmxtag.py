#!/usr/bin/python -u
#
#!/usr/local/bin/python2.3 -u
#
#######################################################################
#
#
#   jmxtag.py
#
#   Read in the treebank and use JMX to tag it.
#
#   USAGE: ./jxmtag.py projectdir < treebank
#   where projectdir is the appropriate JMX training data/project directory.
#
#   $Id: jmxtag.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################


from variables import *

import parsetree
#import vocab
import posclass
import postags

import os
import sys

jmxin = None
jmxout = None

#vocab.init()

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
	assert(len(sys.argv) == 2)

	# Open the JMX pos tagger
	global jmxin, jmxout
	(jmxin, jmxout) = os.popen2(string.replace(jmxcmd, "PROJECTDIR", sys.argv[1]))

	for l in sys.stdin:
		if not l: assert(0)

		tree = parsetree.read_tree(l)
		if tree == None:
			debug(1, "Skipping empty tree")
			continue

		tree.prune_labels(["-NONE-"])

		# Run the JMX pos tagger on this tree.
		jmxstr = string.join([n.headword for n in tree.leaves()]) + "\n"
		jmxin.write(jmxstr)
		jmxin.flush()
		jmxtoks = string.split(jmxout.readline())
		assert len(jmxtoks) == len(tree.leaves())
		#print string.join(["%s_%s" % (n.headword, n.headtag) for n in tree.leaves()]) + "\n", string.join(jmxtoks, " ")
		#print
		# Change the POS tags at the leaves to match the output
		# of the JMX pos tagger
		for n in tree.leaves():
			(word, jmxtag) = string.split(jmxtoks[0], "_")
			assert n.headword == word

#			if remove_quotation_marks and jmxtag in ["``", "''"]:
#				if n.headtag != jmxtag:
#					print jmxstr
#				assert n.headtag == jmxtag
#			elif raise_punctuation and jmxtag in punctuation_tags:
#				if n.headtag != jmxtag:
#					print jmxstr
#				assert n.headtag == jmxtag
			n.headtag = jmxtag
			jmxtoks = jmxtoks[1:]

		sys.stdout.write("%s\n" % tree.to_string())

	jmxin.close()
	jmxout.close()

if profile:
	#prof = hotshot.Profile("preprocess.prof", lineevents=1)
	prof = hotshot.Profile("preprocess2.prof")
	prof.runcall(main)
	prof.close()
	stats = hotshot.stats.load("preprocess2.prof")
	stats.strip_dirs()
	stats.sort_stats('time', 'calls')
	stats.print_stats(20)
else:
	main()
