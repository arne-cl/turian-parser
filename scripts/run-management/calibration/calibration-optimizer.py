#!/usr/bin/python2.4
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import sys, os, re, string, tempfile, shutil, os.path, random
from variables import *

#random.seed(0)
random.seed(2)
wd = os.getcwd()

# Evaluate a parser and return its LFMS
def evaluate_parser(parser):
	tmpdir = tempfile.mkdtemp()
	debug(2, "Created tempdir: %s" % tmpdir)

	shutil.copyfile(params, os.path.join(tmpdir, "parameters"))
	# Copy the parser hypotheses to the tmpdir
	for lbl in parser:
		hyp = all_dloss[lbl][parser[lbl]]
		shutil.copyfile(hyp, os.path.join(tmpdir, hyp))
	r = os.chdir(tmpdir)
	#assert not r
	if r:
		shutil.rmtree(tmpdir)
		return 0

	r = os.system("%s 2> /dev/null" % os.path.join(scripts_dir, "fix-checkpoints.py"))
	#assert not r
	if r:
		sys.stderr.write("FAILED: %s 2> /dev/null" % os.path.join(scripts_dir, "fix-checkpoints.py"))
		shutil.rmtree(tmpdir)
		return 0

	debug(2, "About to parse...")
	r = os.system("%s %s parameters 0 > parse-devel.out 2> parse-devel.err" % (os.path.join(system_dir, "code/build/Linux/parser"), parsefile["devel"]))
	#assert not r
	if r:
		sys.stderr.write("FAILED: %s %s parameters 0 > parse-devel.out 2> parse-devel.err\n" % (os.path.join(system_dir, "code/build/Linux/parser"), parsefile["devel"]))
		shutil.rmtree(tmpdir)
		return 0
	debug(2, "...done parsing")

	r = os.system("%s devel < parse-devel.out > parse-devel 2> /dev/null" % os.path.join(scripts_dir, "postprocess.py"))
	#assert not r
	if r:
		sys.stderr.write("FAILED: %s devel < parse-devel.out > parse-devel 2> /dev/null\n" % os.path.join(scripts_dir, "postprocess.py"))
		shutil.rmtree(tmpdir)
		return 0

	r = os.system("/home/turian/parsing/tools/eval/EVALB/evalb -p /home/turian/parsing/tools/eval/EVALB/COLLINS15.prm %s parse-devel | %s > parse-devel.score" % (postprocess_gold["devel"], os.path.join(scripts_dir, "eda/fms.pl")))
	#assert not r
	if r:
		sys.stderr.write("FAILED: /home/turian/parsing/tools/eval/EVALB/evalb -p /home/turian/parsing/tools/eval/EVALB/COLLINS15.prm %s parse-devel | %s > parse-devel.score\n" % (postprocess_gold["devel"], os.path.join(scripts_dir, "eda/fms.pl")))
		shutil.rmtree(tmpdir)
		return 0

	f = open("parse-devel.score")
	f.readline()
	fms = float(string.split(f.readline(), ",")[0])
	#debug(1, "FMS = %.2f" % fms)

	shutil.rmtree(tmpdir)
	debug(2, "Removed tempdir: %s" % tmpdir)

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

def debug_out_parser(fms, parser):
	str = ""
	str += "%.2f" % fms
	for lbl in parser:
		str += " (%s, %.2f)" % (lbl, parser[lbl])
	return str

def print_parser(parser, fms):
	print fms,
	for lbl in parser:
		print all_dloss[lbl][parser[lbl]],
	print
	sys.stdout.flush()

def use_parser(parser):
	os.chdir(wd)
	fms = evaluate_parser(parser)
	scores.append((fms, parser))
	debug(2, debug_out_parser(fms, parser))
	for lbl in parser:
		add_score(lbl, parser[lbl], fms)
	return fms


freq = {
"ADJP": 0.021609,
"ADVP": 0.0364083,
"CONJP": 0.000107241,
"FRAG": 0.0020054,
"INTJ": 0.000557653,
"LST": 0.000160861,
"NAC": 0.000139413,
"NP": 0.357198,   # w/o NPB
"NPB": 0,
"NX": 0.000589825,
"PP": 0.0881842,
"PRN": 0.00154427,
"QP": 0.0127724,
"RRC": 2.14482e-05,
"S": 0.140453,
"SBAR": 0.0185205,
"SBARQ": 0.00102951,
"SINV": 0.00170513,
"SQ": 0.00164079,
"TOP": 0.109107,
"UCP": 0.000353895,
"VP": 0.199125,
"WHADJP": 9.65168e-05,
"WHADVP": 0.00163006,
"WHNP": 0.00412877,
"WHPP": 8.57927e-05,
"X": 0.000825755,
}

def choose_label():
	tot = 0.
	vary_freq = {}
	for l in freq:
		# Add some skew
		#vary_freq[l] = freq[l] + random.random()*0.05
		vary_freq[l] = freq[l] + random.random()*0.02
		tot += vary_freq[l]
	choice = random.random() * tot
	tot = 0.
	for l in freq:
		tot += vary_freq[l]
		if tot > choice: return l

scores = []
parser = initial_parser
bestfms = use_parser(parser)
debug(1, debug_out_parser(bestfms, parser))
sys.stderr.flush()
print_parser(parser, bestfms)
tabu = []
while 1:
	# Choose a new parser
#	for lbl in parser:
		lbl = choose_label()
		while lbl in tabu:
			lbl = choose_label()
		tabu = ([lbl] + tabu)[:5]
		old_dloss = parser[lbl]
		debug(1, "Varying label = %s (current = %.2f)" % (lbl, old_dloss))
		fmsval = []
		fmsval.append((bestfms, parser[lbl]))

		dlosses = [k for k in all_dloss[lbl].keys() if k != parser[lbl]]
		random.shuffle(dlosses)
		if freq[lbl] < 0.01:
			dlosses = dlosses[:5]
		else:
			dlosses = dlosses[:10]
		for d in dlosses:
			parser[lbl] = d
			fmsval.append((use_parser(parser), d))
			debug(1, "Score for %s with dloss=%.2f = %.2f" % (lbl, d, fmsval[-1][0]))
			sys.stderr.flush()
		fmsval.sort()
		fmsval.reverse()
		#if fmsval[0][0] > bestfms: (bestfms, parser[lbl]) = fmsval[0]
		#else: parser[lbl] = old_dloss
		(bestfms, parser[lbl]) = fmsval[0]	# tiebreak in favor of high dloss
		if parser[lbl] == old_dloss:
			debug(1, "%s stays at %.2f (LFMS=%.2f)" % (lbl, parser[lbl], bestfms))
		else:
			debug(1, "%s <- %.2f (LFMS=%.2f)" % (lbl, parser[lbl], bestfms))
			print_parser(parser, bestfms)
		debug(1, debug_out_parser(bestfms, parser))
		sys.stderr.flush()

scores.sort()
print scores
