#!/usr/bin/python2.4
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import sys, os, re, string, tempfile, shutil, os.path, random
from variables import *

random.seed(0)

# Evaluate a parser and return its LFMS
def evaluate_parser(parser):
	tmpdir = tempfile.mkdtemp()
	debug(1, "Created tempdir: %s" % tmpdir)

	shutil.copyfile(params, os.path.join(tmpdir, "parameters"))
	# Copy the parser hypotheses to the tmpdir
	for lbl in parser:
		hyp = all_dloss[lbl][parser[lbl]]
		shutil.copyfile(hyp, os.path.join(tmpdir, hyp))
	r = os.chdir(tmpdir)
	assert not r
	r = os.system("%s 2> /dev/null" % os.path.join(scripts_dir, "fix-checkpoints.py"))
	assert not r

	debug(1, "About to parse...")
	r = os.system("%s %s parameters 0 > parse-devel.out 2> /dev/null" % (os.path.join(system_dir, "code/build/Linux/parser"), parsefile["devel"]))
	assert not r
	debug(1, "...done parsing")

	r = os.system("%s devel < parse-devel.out > parse-devel 2> /dev/null" % os.path.join(scripts_dir, "postprocess.py"))
	assert not r

	r = os.system("/home/turian/parsing/tools/eval/EVALB/evalb -p /home/turian/parsing/tools/eval/EVALB/COLLINS15.prm %s parse-devel | %s > parse-devel.score" % (postprocess_gold["devel"], os.path.join(scripts_dir, "eda/fms.pl")))
	assert not r

	f = open("parse-devel.score")
	f.readline()
	fms = float(string.split(f.readline(), ",")[0])
	debug(1, "FMS = %.2f" % fms)

	shutil.rmtree(tmpdir)
	debug(1, "Removed tempdir: %s" % tmpdir)

	return fms

hyp_score = {}
def add_score(lbl, dloss, fms):
	if lbl not in hyp_score:
		hyp_score[lbl] = {}
	if dloss not in hyp_score[lbl]:
		hyp_score[lbl][dloss] = (0, 0)
	(a, b) = hyp_score[lbl][dloss] 
	hyp_score[lbl][dloss] = (a+1, b+fms)

assert len(sys.argv) == 3
params = sys.argv[2]
initial_dloss = float(sys.argv[1])
initial_parser = {}	# parser[lbl] => dloss

all_dloss = {}

chkre = re.compile("^checkpoint.label-([A-Z]+)$")
for f in os.listdir("."):
	m = chkre.match(f)
	if m:
		lbl = m.group(1)
		assert lbl not in all_dloss
		all_dloss[lbl] = {}
		chk = open(f)
		for l in chk:
			(hyp, dloss) = string.split(l)
			dloss = float(dloss)
			all_dloss[lbl][dloss] = hyp
			add_score(lbl, dloss, 1.)

		# Find the dloss of this label given the initial mindloss
		dl = all_dloss[lbl].keys()
		dl.sort()
		for x in dl:
			if x > initial_dloss:
				initial_parser[lbl] = x
				break

scores = []
parser = initial_parser
wd = os.getcwd()
while 1:
	os.chdir(wd)
	fms = evaluate_parser(parser)
	scores.append((fms, parser))
	str = ""
	str += "%.2f" % fms
	print fms,
	for lbl in parser:
		print all_dloss[lbl][parser[lbl]],
		str += " (%s, %.2f)" % (lbl, parser[lbl])
	print
	sys.stdout.flush()
	debug(1, str)
	for lbl in parser:
		add_score(lbl, parser[lbl], fms)

	# Choose a new parser
	for lbl in parser:
		tot = 0.
		for dloss in hyp_score[lbl]:
			(a, b) = hyp_score[lbl][dloss]
			avg = b/a
			tot += avg
		choice = random.random() * tot

		tot = 0.
		for dloss in hyp_score[lbl]:
			(a, b) = hyp_score[lbl][dloss]
			avg = b/a
			tot += avg
			if tot > choice:
				parser[lbl] = dloss
				break

scores.sort()
print scores
