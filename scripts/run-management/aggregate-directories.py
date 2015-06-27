#!/usr/bin/python2.4
#!/usr/bin/python
#
#######################################################################
#
#
#   aggregate-directories.py
#
#   USAGE: ./aggregate-directories.py newdir olddir1 olddir2 [oldir3 ...]
#
#   Read in the classifiers in old directories, choose the most trained
#   version of each classifier, and copy them to the new directory.
#
#   $Id: aggregate-directories.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import sys, os, os.path, string, re
from os.path import isdir, isfile, join

if len(sys.argv) < 4:
	sys.stderr.write("Incorrect call.\nUSAGE: ./aggregate-directories.py newdir olddir1 olddir2 [oldir3 ...]\n")
	assert 0

from debug import *
set_debug_level(99)

newdir = sys.argv[1]
olddirs = sys.argv[2:]

assert not os.access(newdir, os.F_OK)
for olddir in olddirs: assert isdir(olddir)

all_labels = {}

checkpoint_re = re.compile("checkpoint.label-([A-Z]+)$")
def read_checkpoint(dir, file):
	assert isdir(dir)
	file = join(dir, file)
	assert isfile(file)

	match = checkpoint_re.search(file)
	assert match
	lbl = match.group(1)
	all_labels[lbl] = True

	hypothesis_re = re.compile("^(hypothesis.label-%s.l1_penalty-)([0-9\.\-\+e]+) ([0-9\.\-\+e]+)$" % lbl)

	f = open(file, "rt")
	hypotheses = []
	for l in f:
		match2 = hypothesis_re.search(l)
		assert match2
		assert float(match2.group(2)) == float(match2.group(3))
		l1 = float(match2.group(2))
		hypothesis = join(dir, match2.group(1) + match2.group(2))
		hypotheses.append((l1, hypothesis))
	f.close()
	return hypotheses

debug(1, "Writing to new directory: %s" % newdir)
debug(1, "\told directories:\n\t\t\t%s" % string.join(olddirs, "\n\t\t\t"))

labels = {}

label_best_l1 = {}
label_best_file = {}
label_best_dir = {}

for olddir in olddirs:
	assert olddir not in labels
	labels[olddir] = {}
	for file in os.listdir(olddir):
		match = checkpoint_re.search(file)
		if match:
			lbl = match.group(1)
			hypotheses = read_checkpoint(olddir, file)
			hypotheses.sort()

			if len(hypotheses) > 1:
				assert hypotheses[0][0] < hypotheses[1][0]

			best_l1 = hypotheses[0][0]
			best_file = hypotheses[0][1]
			labels[olddir][lbl] = (best_l1, best_file)
			if lbl not in label_best_l1 or best_l1 < label_best_l1[lbl]:
				label_best_l1[lbl] = best_l1
				label_best_file[lbl] = best_file
				label_best_dir[lbl] = olddir

	if not len(labels[olddir]):
		sys.stderr.write("\n\nFATAL: No checkpoints found in '%s'" % olddir)
		assert 0

import tempfile

import re
end_re = re.compile("END[\r\n]*")

master_checkpoint = {}
for lbl in all_labels:
	all = []
	for dir in labels:
		if not lbl in labels[dir]: continue

		l1 = labels[dir][lbl][0]
		file = labels[dir][lbl][1]
		if l1 == label_best_l1[lbl]:
			if file == label_best_file[lbl]: continue
#			print "diff %s %s" % (join(label_best_dir[lbl], "checkpoint.label-%s" % lbl), join(dir, "checkpoint.label-%s" % lbl))
			if not os.system("diff %s %s > /dev/null 2>&1" % (join(label_best_dir[lbl], "checkpoint.label-%s" % lbl), join(dir, "checkpoint.label-%s" % lbl))):
				debug(1, "WARNING: Checkpoints differ:\n\t%s\n\t%s" % (join(label_best_dir[lbl], "checkpoint.label-%s" % lbl), join(dir, "checkpoint.label-%s" % lbl)))
				debug(1, "\tAttempting sort...")
				(id1, fn1) = tempfile.mkstemp()
				(id2, fn2) = tempfile.mkstemp()
				os.system("sort %s > %s" % (join(label_best_dir[lbl], "checkpoint.label-%s" % lbl), fn1))
				os.system("sort %s > %s" % (join(dir, "checkpoint.label-%s" % lbl), fn2))
				assert not os.system("diff %s %s" % (fn1, fn2))
				os.remove(fn1)
				os.remove(fn2)
				debug(1, "\t...sort resulted in successful comparison")

#			print "diff %s %s" % (label_best_file[lbl], file)
#			assert not os.system("diff %s %s" % (label_best_file[lbl], file))
			# See which hypothesis is longer
			f1 = open(label_best_file[lbl], "rt")
			f2 = open(file, "rt")
			(f1txt, f1rep) = end_re.subn("", string.join(f1.readlines()))
			(f2txt, f2rep) = end_re.subn("", string.join(f2.readlines()))
			assert f1rep <= 1 and f2rep <= 1

			f1good = False
			f2good = False
			# Is f2txt a subset of f1txt?
			if string.find(f1txt, f2txt) == 0: f1good = True
			# Is f1txt a subset of f2txt?
			if string.find(f2txt, f1txt) == 0: f2good = True
#                        print label_best_file[lbl], file
			if not (f1good or f2good): print label_best_file[lbl], file, f1txt, f2txt
			assert f1good or f2good
			if f1good and f2good:
				debug(1, "WARNING: Best %s hypotheses equal:\n\t%s\n\t%s" % (lbl, label_best_file[lbl], file))
			elif f2good: 
				debug(1, "WARNING: Replacing %s hypothesis:\n\tNEW BEST: %s\n\tOLD BEST: %s" % (lbl, file, label_best_file[lbl]))
				label_best_file[lbl] = file
				label_best_dir[lbl] = dir
			else:
				assert f1good
				debug(1, "WARNING: Keeping best %s hypothesis:\n\tBEST: %s\n\tTRAINED LESS: %s" % (lbl, label_best_file[lbl], file))

	master_checkpoint[lbl] = (label_best_dir[lbl], "checkpoint.label-%s" % lbl)

os.mkdir(newdir)
assert isdir(newdir)
newdir = join(newdir, "workdata")
os.mkdir(newdir)
assert isdir(newdir)

debug(1, "")
debug(1, "MASTER CHECKPOINTS:")
for lbl in all_labels:
	(dir, chk) = master_checkpoint[lbl]
	debug(1, "\t%s (l1=%f)" % (join(dir, chk), label_best_l1[lbl]))

	assert not os.system("cp %s %s" % (join(dir, chk), newdir))

	for (l1, file) in read_checkpoint(dir, chk):
		assert not os.system("cp %s %s" % (file, newdir))
