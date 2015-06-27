#
#   classifier.py
#
#   Communicate with classifiers, which make confidence predictions for
#   queries.
#
#   $Id: classifier.py 1657 2006-06-04 03:03:05Z turian $
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from debug import *
from variables import *

import query
import string
import state
import vocab

import math
import re

import popen2
classifier_in = None
classifier_out = None

baseline = False

good_cache = {}
last_cache = {}
example_cache = {}
def clear_cache():
	global last_cache
	global example_cache
	debug(1, "Good cache contains %d items, cache to be cleared contains %d items" % (len(good_cache), len(example_cache)))
	last_cache = example_cache
	example_cache = {}

def open(cmd):
	if cmd == "baseline" or cmd == "b":
		global baseline
		baseline = True
		debug(1, "Simulating baseline classifier")
		return

	global classifier_out, classifier_in
	(classifier_out, classifier_in) = popen2.popen2(cmd)
	debug(1, "popen'ed classifier: %s" % cmd)

def close():
	if not baseline:
		global classifier_out, classifier_in
		classifier_in.write("CRASH, BABY\n")
		classifier_in.flush()
		classifier_out.close()
		classifier_in.close()

#predict_re = re.compile("([0-9]+),\s*([0-9]+)\s+.*predict_label\s*=\s*([0-9]+)\s+confidence\s*=\s*([0-9\-\+e\.]+)")
predict_re = re.compile("predict_label\s*=\s*([0-9]+)\s+confidence\s*=\s*([0-9\-\+e\.]+)")

# Given actions, find the confidence for each
def predict(actions, correct_only=False):
	global example_cache
#	if correct_only:
#		# Discard incorrect actions
#		actions = [a for a in actions if a[0] == 1.]
	action_conf = []
	for a in actions:
		if not baseline:
			assert len(a[1][0]) > 0
			if len(a[1][0]) > max_span_length:
				action_conf.append((-99999999., a))
				continue

			if correct_only:
				origstr = "\n%s\n" % query.to_string(a[1], state.parser, correct=a[0], with_weight=False)
			else:
				origstr = "\n%s\n" % query.to_string(a[1], state.parser, with_weight=False)
			if origstr in good_cache:
				assert query.new_item_string() == ""
				for (conf, lbl) in good_cache[origstr]:
					action_conf.append((conf, (lbl, a[1])))
				continue
			if origstr in last_cache:
				assert query.new_item_string() == ""
				for (conf, lbl) in last_cache[origstr]:
					action_conf.append((conf, (lbl, a[1])))
				# Since we've seen this query twice in a row, move it to the
				# good cache
				good_cache[origstr] = last_cache[origstr]
				continue
			if origstr in example_cache:
				assert query.new_item_string() == ""
				for (conf, lbl) in example_cache[origstr]:
					action_conf.append((conf, (lbl, a[1])))
				continue

			newitems = query.new_item_string()
			str = string.strip(newitems + origstr)
			str = string.replace(str, "\n\n", "\n")
			classifier_in.write(str + "\n")
			classifier_in.flush()

#			sys.stderr.write(newitems)
#			if (len(a[1][0]) == 2 and len(a[1][1]) == 1):
#				sys.stderr.write(str)

			example_cache[origstr] = []
			# Read one confidence per label
			for i in range(labels):
#				print range(labels)
				predict = classifier_out.readline()
#				print i, labels
#				print str
#				print predict
#				print
				sys.stdout.flush()
				match = predict_re.match(predict)
				if match == None:
					sys.stderr.write("%s\n" % `predict`)
					sys.stderr.write("query: %s\n" % `str`)
					assert(0)
				assert(match)
				labelidx = int(match.group(1))
				assert(labelidx == i+1)
#				assert(labelidx != 0)
				if correct_only:
					assert(vocab.idx_to_label[labelidx] == a[0])
	
				assert(len(a) == 2)
				a = (vocab.idx_to_label[labelidx], a[1])
				prediction = float(match.group(2))
				action_conf.append((prediction, a))
				example_cache[origstr].append((prediction, a[0]))
#				if (len(a[1][0]) == 2 and len(a[1][1]) == 1):
#					sys.stderr.write(`prediction, a[0]` + "\n")
		else:
			prediction = 0.
			action_conf.append((prediction, a))
#			for i in range(labels):
#				action_conf.append((prediction, (vocab.idx_to_label[i+1], a[1])))
	return action_conf
