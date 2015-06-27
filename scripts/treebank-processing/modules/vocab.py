#
#   vocab.py
#
#   Determine the relevant vocabulary (all words of sufficient frequency
#   in the training data). Also, assign indices to POS and constituent
#   labels.
#
#   $Id: vocab.py 1657 2006-06-04 03:03:05Z turian $
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from variables import *

import compounds
import postags

have_read = False

vocab_count = {}
vocab_total = 0

label_count = ({}, {})

vocab_to_idx = {}
label_to_idx = {}
idx_to_label = {}

vocab_funccount = {}

def init():
	global vocab_count, vocab_total
	global label_count
	global vocab_to_idx, label_to_idx

	vocab_count = {}
	vocab_total = 0
	label_count = ({}, {})
	vocab_to_idx = {}
	label_to_idx = {}

	# Add the "no label" label
	# FIXME: Don't hardcode the "no label" label
	label_count[0]['no_label'] = 0

	# Add all POS tags
	for w in postags.function.keys() + postags.content.keys():
		if w == '-NONE-': continue
		label_count[1][w] = 0

def add(tree):
	global vocab_count, vocab_total
	global label_count
	global vocab_funccount

	for node in tree.internal_nodes() + tree.leaves():
		if node.label not in label_count[node.isleaf]:
			label_count[node.isleaf][node.label] = 0
		label_count[node.isleaf][node.label] += 1

	# Filter out duplicate words in the same tree
	used_words = {}
	for node in tree.leaves():
		assert node.isleaf
		w = node.headword
		if w in used_words: continue
		used_words[w] = 1
		vocab_count[w] = vocab_count.get(w, 0) + 1
		vocab_total += 1
		if tree.headtag in postags.function:
			vocab_funccount[w] = vocab_funccount.get(w, 0) + 1

def write():
	# Remove infrequent words
	lst = [(vocab_count[word], word) for word in vocab_count if vocab_count[word] >= minfreq]
	# If we only want function words in the vocab...
	if only_function_tags:
		# ...then keep only words that occur more frequently with a function tag than with a content tag.
		lst = [(cnt, word) for (cnt, word) in lst if vocab_funccount.get(word, 0)*2 > vocab_count.get(word, 0) or word in extra_function_words]
	lst.sort()
	lst.reverse()
	lst = ['*rare*'] + [p[1] for p in lst]

	# Make sure we have all compounds in the vocabulary:
	if flatten_closed_class_compounds:
		for w in compounds.compound_words:
			if w not in lst: lst.append(w)

#	print lst
	debug(1, "Writing vocabulary to: %s" % vocabulary_file)
	fil = open(vocabulary_file, "wt")

	idx = 1
	for w in lst:
		fil.write("%d %s\n" % (idx, w))
		idx += 1
	fil.close

	# FIXME: Don't hardcode these values
	lst = [(0, label_count[0][lbl], lbl) for lbl in label_count[0] if lbl != 'no_label' and lbl != 'TOP' and lbl != "SPRIME" and lbl != "NPB"]
	lst += [(-1, label_count[1][lbl], lbl) for lbl in label_count[1]]
	if use_SPRIME_transform: lst.append((0, 0, "SPRIME"))
	#if add_basal_nps: lst.append((0, 999999, "NPB"))
	lst.append((0, 999999, "NPB"))
	lst.sort()
	lst.reverse()
	lst = [(0, 0, "no_label"), (0, 0, "TOP")] + lst
	debug(1, "Writing labels to: %s" % label_file)
	fil = open(label_file, "wt")
	idx = 0
	for l in lst:
		fil.write("%d %d %d %s\n" % (idx, -l[0], l[1], l[2]))
		idx += 1
	fil.close

def read():
	global have_read
	if have_read:
		return

	global vocab_total, vocab_count
	init()

	debug(1, "Reading vocabulary from: %s" % vocabulary_file)
	fil = open(vocabulary_file, "rt")
	vocab_to_idx[""] = 0
	vocab_to_idx[None] = 0
	for l in fil.readlines():
		(idx, word) = string.split(l)
		idx = int(idx)

		# FIXME: Don't hardcode this
		if word == "TOTALFREQ":
			continue

		assert idx != 0
		vocab_to_idx[word] = idx
	fil.close()

	debug(1, "Reading labels from: %s" % label_file)
	fil = open(label_file, "rt")
	label_to_idx[""] = (0, -1)
	label_to_idx[None] = (0, -1)
	for l in fil.readlines():
		(idx, isleaf, cnt, lbl) = string.split(l)
		idx = int(idx)
		assert (idx != 0 and lbl != 'no_label') or (idx == 0 and lbl == 'no_label')
		isleaf = int(isleaf)
		cnt = int(cnt)

		label_to_idx[lbl] = (idx, isleaf)
		idx_to_label[idx] = lbl
	fil.close()

	have_read = True
