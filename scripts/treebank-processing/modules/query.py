#
#   query.py
#
#   Pass queries (examples) to classifiers, and get their predictions
#
#   $Id: query.py 1657 2006-06-04 03:03:05Z turian $
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

import string
import copy

from debug import *
from variables import *

import vocab
import posclass

item_cache = {}
item_cnt = 0

itemstr = ""
newitemstr = ""

debug_items = False

def new_item_string():
	global newitemstr
	str = newitemstr
	newitemstr = ""
	return str

def item_string(item, uidmap):
	global item_cnt

	if item == -1:
		return ""

	n = uidmap[item]

	key = "%d,%d,%d,%d,%d,%d" % (n.isleaf, vocab.label_to_idx[n.label][0], vocab.label_to_idx[n.headlabel][0], vocab.vocab_to_idx[n.headword], vocab.label_to_idx[n.headtag][0], posclass.to_idx[posclass.from_tag[n.headtag]])
	if key in item_cache:
		return ""

	item_cnt += 1
	item_cache[key] = item_cnt

	str = "item %d isleaf=%d label=%d headlabel=%d headword=%d headtag=%d headtagclass=%d\n" % (item_cnt, n.isleaf, vocab.label_to_idx[n.label][0], vocab.label_to_idx[n.headlabel][0], vocab.vocab_to_idx[n.headword], vocab.label_to_idx[n.headtag][0], posclass.to_idx[posclass.from_tag[n.headtag]])
	if debug_items:
		debug(1, "\n%s" % string.strip(str))

	global itemstr
	global newitemstr
	itemstr += str
	newitemstr += str

def item_int(item, uidmap):
	if item == -1:
		return 0

	n = uidmap[item]
	key = "%d,%d,%d,%d,%d,%d" % (n.isleaf, vocab.label_to_idx[n.label][0], vocab.label_to_idx[n.headlabel][0], vocab.vocab_to_idx[n.headword], vocab.label_to_idx[n.headtag][0], posclass.to_idx[posclass.from_tag[n.headtag]])
	return item_cache[key]

# Given a list of items and a map from item UIDs to actual nodes,
# return a query string (in span format)
def to_string((items, lcontext, rcontext), uidmap, correct=None, count=1, with_weight=True, state=None):
	all_items = items + lcontext[0:example_context] + rcontext[0:example_context]
	for i in all_items:
		item_string(i, uidmap)

	while len(items) < max_span_length:
		items = items + [-1]
	while len(lcontext) < example_context:
		lcontext = [-1] + lcontext
	while len(rcontext) < example_context:
		rcontext = rcontext + [-1]

	lc = copy.copy(lcontext)
	lc.reverse()
	all_items_str = ""
	all_items_str += string.join([`item_int(i, uidmap)` for i in items], ' ') + " "
	all_items_str += string.join([`item_int(i, uidmap)` for i in lc[0:example_context]], ' ') + " "
	all_items_str += string.join([`item_int(i, uidmap)` for i in rcontext[0:example_context]], ' ')

	if state != None:
		state_str = "state=%d " % state
	else:
		state_str = ""

	if correct != None:
		if correct == 1 or correct == 0:
			correct_idx = correct
		else:
			# Make sure this is a non-terminal label
			assert vocab.label_to_idx[correct][1] == 0
			correct_idx = vocab.label_to_idx[correct][0]

		assert(correct_idx >= 0 and correct_idx <= labels)

		if with_weight:
			return "%sweight=%.16g correct=%d | %s |" % (state_str, 1./count, correct_idx, all_items_str)
		else:
			return "%s%d %s" % (state_str, correct_idx, all_items_str)
	else:
		return "%s0 %s" % (state_str, all_items_str)
