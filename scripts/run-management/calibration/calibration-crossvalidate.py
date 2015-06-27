#!/usr/bin/python2.4
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import sys, os, re, string, tempfile, shutil, os.path, random
from variables import *

wd = os.getcwd()

parsefile = "/home/turian/parsing/20050609-FIXED-training-data.lesioning-from-less-context-as-baseline/runs/baseline/data/ptb22/ptb-section22.parse.jmx"
postprocess_gold = "/home/turian/parsing/20050609-FIXED-training-data.lesioning-from-less-context-as-baseline/runs/baseline/data/ptb22/ptb-section22.parse-postprocess.gld"
postprocess_jmx = "/home/turian/parsing/20050609-FIXED-training-data.lesioning-from-less-context-as-baseline/runs/baseline/data/ptb22/ptb-section22.parse-postprocess.jmx"

# Evaluate a parser and return its LFMS
def evaluate_parser(parser):
	tmpdir = tempfile.mkdtemp()
	debug(1, "Created tempdir: %s" % tmpdir)

	os.chdir(wd)
	shutil.copyfile(params, os.path.join(tmpdir, "parameters"))
	# Copy the parser hypotheses to the tmpdir
	for hyp in parser:
		shutil.copyfile(hyp, os.path.join(tmpdir, hyp))

	r = os.chdir(tmpdir)
	#assert not r
	if r:
		shutil.rmtree(tmpdir)
		return 0

	r = os.system("%s 2> /dev/null" % os.path.join(scripts_dir, "fix-checkpoints.py"))
	#assert not r
	if r:
		shutil.rmtree(tmpdir)
		return 0

	debug(1, "About to parse...")
	r = os.system("%s %s parameters 0 > parse-ptb22.out 2> /dev/null" % (os.path.join(system_dir, "code/build/Linux/parser"), parsefile))
	#assert not r
	if r:
		shutil.rmtree(tmpdir)
		return 0
	debug(1, "...done parsing")

	os.chdir(wd)
	r = os.system("%s %s %s < %s/parse-ptb22.out > %s/parse-ptb22 2> /dev/null" % (os.path.join(scripts_dir, "postprocess-specific.py"), postprocess_gold, postprocess_jmx, tmpdir, tmpdir))
	#assert not r
	if r:
		shutil.rmtree(tmpdir)
		return 0
	os.chdir(tmpdir)

	r = os.system("/home/turian/parsing/tools/eval/EVALB/evalb -p /home/turian/parsing/tools/eval/EVALB/COLLINS15.prm %s parse-ptb22 | %s > parse-ptb22.score" % (postprocess_gold, os.path.join(scripts_dir, "eda/fms.pl")))
	#assert not r
	if r:
		shutil.rmtree(tmpdir)
		return 0

	f = open("parse-ptb22.score")
	f.readline()
	fms = float(string.split(f.readline(), ",")[0])
	debug(1, "FMS = %.2f" % fms)

	shutil.rmtree(tmpdir)
	debug(1, "Removed tempdir: %s" % tmpdir)

	return fms

assert len(sys.argv) == 2
params = sys.argv[1]

bestfms = -1
bestparsers = []
for l in sys.stdin:
	hyps = string.split(l)
	parser = hyps[1:]
	fms = evaluate_parser(parser)
	if bestfms < fms:
		debug(1, "New best FMS <- %.2f" % fms)
		bestfms = fms
		bestparsers = []
	if bestfms <= fms:
		bestparsers.append(parser)
		debug(1, "FMS = %.2f, inserted: %s" % (fms, string.join(parser)))

debug(1, "")
debug(1, "%d best parser(s) have LFMS %s" % (len(bestparsers), fms))
for p in bestparsers:
	print string.join(p)
