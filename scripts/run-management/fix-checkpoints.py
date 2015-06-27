#!/usr/bin/python
#
#
#######################################################################
#
#
#   fix-checkpoints.py
#
#   USAGE: ./fix-checkpoints.py [-f]
#   [in the hypotheses working directory]
#
#   Regenerate checkpoint files.
#   Skip any hypothesis that has fewer trees than it should.
#   (e.g. an hypothesis with no trees)
#
#   If the -f parameter is given, we *delete* hypotheses that have
#   fewer trees than they should.
#
#   $Id: fix-checkpoints.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import os, re, sys

if len(sys.argv) == 2:
	assert sys.argv[1] == '-f'
	delete = True
else:
	assert len(sys.argv) == 1
	delete = False

hyps = {}

hypre = re.compile("hypothesis.label-([A-Z]+).l1_penalty-([0-9\.e\+\-]+)$")
for f in os.listdir("."):
	res = hypre.match(f)
	if res:
		lbl = res.group(1)
		dloss = float(res.group(2))
		trees = 0
		fil = open(f, "rt")
		for l in fil:
			if l == "Tree\n": trees += 1
		fil.close()

		if lbl not in hyps: hyps[lbl] = []
		hyps[lbl].append((dloss, f, trees))

for lbl in hyps:
	filename = "checkpoint.label-%s" % lbl
	sys.stderr.write("Writing to '%s'\n" % filename)
	chk = open(filename, "wt")

	lastcnt = 0
	hyps[lbl].sort()
	hyps[lbl].reverse()
	for (dloss, filename, cnt) in hyps[lbl]:
		if cnt < lastcnt:
			if delete:
				sys.stderr.write("REMOVING '%s' (%d trees should be more than %d)\n" % (filename, cnt, lastcnt))
				os.remove(filename)
			else:
				sys.stderr.write("SKIPPING '%s' (%d trees should be more than %d)\n" % (filename, cnt, lastcnt))
			continue
		lastcnt = cnt
#		chk.write("%s dloss %g\n" % (filename, dloss))
		chk.write("%s %g\n" % (filename, dloss))
	chk.close()
