#
#   state.py
#
#   Handle partial-parses (states)
#
#   $Id: state.py 1657 2006-06-04 03:03:05Z turian $
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from variables import *

import head
import parsetree
import query
import vocab

uidmap = (None, None)
state = None

parser = uidmap[0]
treebank = uidmap[1]

constits = None

allow_inconsistent = False

# Compute the length of the state, excluding punctuation items and TOP
def length():
	tot = 0
	for uid in state:
		if parser[uid].label not in punctuation_tags and parser[uid].label != 'TOP':
			tot += 1
	return tot

def clear():
	global uidmap, state
	global parser, treebank
	global correct_structure_action

	if treebank != None:
		for uid in treebank:
			if treebank[uid] != None:
				treebank[uid].__del__
	if parser != None:
		for uid in parser:
			if parser[uid] != None:
				parser[uid].__del__

	uidmap = ({}, {})
	state = []
	parser = uidmap[0]
	treebank = uidmap[1]
	correct_structure_action = None

# Create and return the initial state from a tree
def initialize(newmap):
	clear()
	uids = newmap.keys()
	uids.sort()
	for uid in uids:
		if newmap[uid].isleaf:
			treebank_leaf = newmap[uid]
			parser_leaf = parsetree.Node()
			parser_leaf.uid = treebank_leaf.uid
			parser_leaf.isleaf = treebank_leaf.isleaf
			parser_leaf.headword = treebank_leaf.headword
			parser_leaf.origheadword = treebank_leaf.origheadword

			# Give the parser the POS labels
			parser_leaf.label = treebank_leaf.label
			# Add the treebank leaf label to the parser state
			parser_leaf.headlabel = ''
			parser_leaf.headtag = treebank_leaf.headtag

			parser[uid] = parser_leaf
			treebank[uid] = treebank_leaf

			# Make sure this is a terminal label
			assert vocab.label_to_idx[treebank[uid].headtag][1] == 1

			state.append(uid)
		else:
			treebank_node = newmap[uid]
			parser[uid] = None
			treebank[uid] = treebank_node

def actions():
	actions = []

	# Find available actions
	for i in range(len(state)):
		r = range(i+1, min(len(state)+1, i+max_span_length+1))

		# Add the correct actions of length > max_span_length, if we do not allow
		# inconsistent state
		if not allow_inconsistent and treebank[state[i]].parent() != None \
			and len(treebank[state[i]].parent().children) > max_span_length:
			if i+len(treebank[state[i]].parent().children) <= len(state):
				r.append(i+len(treebank[state[i]].parent().children))

		# Find possible incorrect structure actions
		for j in r:
			if allow_inconsistent:
				top = False
				if raise_punctuation:
					# Disallow actions with punctuation as the
					# leftmost or rightmost item
					if parser[state[i]].label in punctuation_tags:
						assert(parser[state[i]].isleaf)
						continue
					elif parser[state[j-1]].label in punctuation_tags:
						assert(parser[state[j-1]].isleaf)
						continue

				# Create a fake label (since we allow inconsistent states)
				actions.append(('foo', (state[i:j],state[0:i],state[j:])))
				continue

			# FIXME: Don't hardcode the "no label" label
			label = 'no_label'

			# If there may be a label for this span...
			if treebank[state[i]].parent() != None:
				parent = treebank[state[i]].parent()

				if len(parent.children) == j-i:
					label = parent.label
					# See if this span actually has a correct label
					for k in range(j-i):
						if state[i+k] != parent.children[k].uid:
							# No, it doesn't
							# FIXME: Don't hardcode the "no label" label
							label = 'no_label'

			if raise_punctuation:
				# Disallow actions with punctuation as
				# the leftmost or rightmost item
				if parser[state[i]].label in punctuation_tags:
					assert(parser[state[i]].isleaf)
					assert(label == 'no_label')
					continue
				if parser[state[j-1]].label in punctuation_tags:
					assert(parser[state[j-1]].isleaf)
					assert(label == 'no_label')
					continue

			actions.append((label, (state[i:j], state[0:i], state[j:])))

	assert(len(actions) != 0)
	return actions

# Remove an item from the span, replacing it in the state by its children
def remove(node):
	global state

	assert not node.isleaf

	found = 0
	new_state = []
	for n in state:
		if n == node.uid:
			found = 1
			for c in node.children:
				new_state.append(c.uid)
				c.parent = None
		else:
			new_state.append(n)
	assert found
	
	state = new_state
	debug(3, "New (previous) state: %s" % `new_state`)

	node.children = []
	del(parser[node.uid])
	del(node)
	

# Perform a single action
def perform(action):
	global state

	debug(3, "Performing action %s" % (`action`))
	(lbl, (span, lcontext, rcontext)) = action

	if allow_inconsistent:
		uid = len(parser)
	else:
		assert(action[0])
		uid = treebank[span[0]].parent().uid
		assert uid != None
		assert uid != -1
		assert lbl == treebank[span[0]].parent().label

	node = parsetree.Node()
	node.uid = uid
	node.isleaf = 0
	node.label = lbl

	for child in span:
		parser[child].parent = weakref.ref(node)
	node.children = [parser[c] for c in span]

	parser[uid] = node

	# Try to bubble head information to this node
	head.bubble(parser[uid])

	new_state = []
	found = 0
	for n in state:
		if n in span:
			assert found != 2
			found = 1
		elif found == 1:
			new_state.append(uid)
			new_state.append(n)
			found = 2
		else:
			new_state.append(n)
	if found == 1:
		new_state.append(uid)
		found = 2
	state = new_state
	debug(3, "New state: %s" % `new_state`)

	return node

# Determine if the state is a goal state (i.e. contains nothing but
# punctuation and one TOP item)
def is_complete():
	punct = 0
	topcnt = 0
	for lbl in [parser[uid].label for uid in state]:
		if lbl in punctuation_tags:
			punct += 1
		elif lbl == 'TOP':
			topcnt += 1
	if topcnt == 1 and punct + topcnt == len(state):
		return True
	else: return False
