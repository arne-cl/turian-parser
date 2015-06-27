#
#   head.py
#
#   Lexicalize (determine head information)
#
#   "The head-rules used by the parser.
#   [parent non-terminal] is the non-terminal on the lefthand-side of
#   a rule.
#   [direction] specifies whether search starts from the left or right
#   end of the rule.
#   [priority list] gives a priority ranking, with priority decreasing
#   when moving down the list."
#   from Collins (1999, pg. 240)
#
#   For modifications to Collins's rules, search for MODIFICATION
#
#   $Id: head.py 1657 2006-06-04 03:03:05Z turian $
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from variables import *

import copy

# head_rules[parent non-terminal] = (direction, priority list)
# MODIFICATION: AUX and AUXG were added after verb terminals.
head_rules = {
"ADJP": ("left", ["NNS", "QP", "NN", "$", "ADVP", "JJ", "VBN", "VBG", "AUX", "AUXG", "ADJP", "JJR", "NP", "JJS", "DT", "FW", "RBR", "RBS", "SBAR", "RB"]),
"ADVP": ("right", ["RB", "RBR", "RBS", "FW", "ADVP", "TO", "CD", "JJR", "JJ", "IN", "NP", "JJS", "NN"]),
"CONJP": ("right", ["CC", "RB", "IN"]),
"FRAG": ("right", []),
"INTJ": ("left", []),
"LST": ("right", ["LS", ":"]),
"NAC": ("left", ["NN", "NNS", "NNP", "NNPS", "NP", "NAC", "EX", "$", "CD", "QP", "PRP", "VBG", "AUX", "AUXG", "JJ", "JJS", "JJR", "ADJP", "FW"]),
"PP": ("right", ["IN", "TO", "VBG", "VBN", "AUX", "AUXG", "RP", "FW"]),
"PRN": ("left", []),
"PRT": ("right", ["RP"]),
"QP": ("left", ["$", "IN", "NNS", "NN", "JJ", "RB", "DT", "CD", "NCD", "QP", "JJR", "JJS"]),
"RRC": ("right", ["VP", "NP", "ADVP", "ADJP", "PP"]),
"S": ("left", ["TO", "IN", "VP", "S", "SBAR", "ADJP", "UCP", "NP"]),
"SBAR": ("left", ["WHNP", "WHPP", "WHADVP", "WHADJP", "IN", "DT", "S", "SQ", "SINV", "SBAR", "FRAG"]),
"SBARQ": ("left", ["SQ", "S", "SINV", "SBARQ", "FRAG"]),
"SINV": ("left", ["VBZ", "VBD", "VBP", "VB", "MD", "AUX", "AUXG", "VP", "S", "SINV", "ADJP", "NP"]),
"SQ": ("left", ["VBZ", "VBD", "VBP", "VB", "MD", "AUX", "AUXG", "VP", "SQ"]),
"UCP": ("right", []),
"VP": ("left", ["TO", "VBD", "VBN", "MD", "VBZ", "VB", "VBG", "VBP", "AUX", "AUXG", "VP", "ADJP", "NN", "NNS", "NP"]),
"WHADJP": ("left", ["CC", "WRB", "JJ", "ADJP"]),
"WHADVP": ("right", ["CC", "WRB"]),
"WHNP": ("left", ["WDT", "WP", "WP$", "WHADJP", "WHPP", "WHNP"]),
"WHPP": ("right", ["IN", "TO", "FW"])
}

head_map = {}

def init():
	for parent in head_rules:
		(direction, priority_list) = head_rules[parent]
		head_map[parent] = {}
		cnt = 0
		for pos in priority_list:
			head_map[parent][pos] = cnt
			cnt += 1

def get_priority(parent, pos):
	if len(head_map) == 0:
		init()
	return head_map[parent].get(pos, len(head_map[parent]))

def get_help(parent, heads):
	# NP rules from Collins (1999, pg. 238)
	# MODIFICATION: We also apply these rules to NX
	if parent == 'NP' or parent == 'NX':
		if heads[len(heads)-1] == 'POS':
			return len(heads)-1
		new_heads = []
		cnt = 0
		for pos in heads:
			new_heads.append((cnt, pos))
			cnt += 1
		heads = new_heads
		rev_heads = copy.copy(heads)
		rev_heads.reverse()
		for pos in rev_heads:
			if pos[1] in ["NN", "NNP", "NNPS", "NNS", "NX", "POS", "JJR"]:
				return pos[0]
			if pos[1] == None or pos[1] == "":
				return -1
		for pos in heads:
			if pos[1] == "NP":
				return pos[0]
			if pos[1] == None or pos[1] == "":
				return -1
		for pos in rev_heads:
			if pos[1] in ["$", "ADJP", "PRN"]:
				return pos[0]
			if pos[1] == None or pos[1] == "":
				return -1
		for pos in rev_heads:
			if pos[1] == "CD":
				return pos[0]
			if pos[1] == None or pos[1] == "":
				return -1
		for pos in rev_heads:
			if pos[1] in ["JJ", "JJS", "RB", "QP"]:
				return pos[0]
			if pos[1] == None or pos[1] == "":
				return -1
		return len(heads)-1
	else:
		if parent not in head_rules:
			debug(2, "UNKNOWN label for head: %s" % parent)
			return 0

		priority = [get_priority(parent, pos) for pos in heads]
		# If rules go right-to-left, then reverse the priority list
		if head_rules[parent][0] == 'right':
			priority.reverse()
		cnt = 0
		new_priority = []
		for pr in priority:
			new_priority.append((pr, cnt))
			cnt += 1
		new_priority.sort()
		#print new_priority
		idx = new_priority[0][1]
		# Unreverse the index for right-to-left
		if head_rules[parent][0] == 'right':
			idx = len(priority) - idx - 1
		return idx

# Given a parent and a list of the heads of the children, return the
# index of the child that contains the head.
def get(parent, heads):
	# FIXME!!!!!
	# There are smarter ways to handle incomplete heads
	if parent == "" or parent == None:
		assert(0)
		return -1
	for pos in heads:
		if pos == "" or pos == None:
			assert(0)
			return -1

	# get_help does most of the work
	idx = get_help(parent, heads)
	assert(idx != -1)

#	debug(2, "Head %s %s: %d" % (parent, `heads`, idx))

	# Check for the special case for coordination given in pg. 239
	# of Collins (1999)
	# MODIFICATION: Collins does not apply coordination head movement
	# to basal NPs. However, we do.
	# MODIFICATION: We treat ':' and ',' as coordinating conjunctions,
	# in addition to 'CC'
	# FIXME: Adam sez don't use ':' as a coordinating conjunction, only
	# use ',' as a coordinating conjunction when there is also a 'CC'.
	# FIXME: This special case should *not* apply to basal NPs and NXs.
	# (cf. Bikel '03 section 3.3)
	if idx > 1 and (heads[idx-1] == 'CC' or heads[idx-1] == ',' or heads[idx-1] == ':'):
		#print "COORD", heads[idx-2], "<-", heads[idx]
		return idx-2
	return idx

# Bubble up head information
def bubble(node):
	# If this node already has a head, we have no new information
	# to bubble up.
	if node.headtag != '' and node.headtag != None:
		return

#	debug(3, "%d head = ?" % (node.uid))
#	debug(2, "%d head = %s, %s" % (node.uid, node.label, `[c.headtag for c in node.children]`))

	# Find a head for this node
	hd = get(node.label, [c.label for c in node.children])

	# If no head was found, then just return
	if hd == -1:
		assert(0)
		return

	# Else, update the head information
	assert node.children[hd].label != None and node.children[hd].label != ''

	# Headlabel can only be a constituent label
	if not node.children[hd].isleaf:
		node.headlabel = node.children[hd].label

	if node.children[hd].headword != None and node.children[hd].headword != '':
	        node.headword = node.children[hd].headword
	        node.headtag = node.children[hd].headtag
		assert node.headtag != None and node.headtag != ''
	else:
		assert(0)
		return
	debug(3, "head %d gives (label %s, headlabel %s, headtag %s, headword %s)" % (node.uid, node.label, node.headlabel, node.headtag, node.headword))

#	debug(2, "%d head %d" % (node.uid, hd))
#	debug(2, "%d head of %s" % (node.uid, `[c.label for c in node.children]`))
#	debug(2, "%d head = %s" % (node.uid, `(node.headword, node.headtag)`))

	# and if we have a parent...
	if node.parent != None and node.parent() != None:
		# ...then bubble this information up
		bubble(node.parent())

# Refresh the head information for every node in a tree
def refresh(node):
	if node.isleaf:
		assert(node.headlabel == '' or node.headlabel == None)
		assert(node.headtag != '' and node.headtag != None)
		assert(node.headword != '' and node.headword != None)
		return

	# First, find the heads of my children
	map(refresh, node.children)

	# Now, find my head
	hd = get(node.label, [c.label for c in node.children])

	assert(hd != -1)
	assert node.children[hd].label != None and node.children[hd].label != ''

	# Headlabel can only be a constituent label
	if not node.children[hd].isleaf:
		node.headlabel = node.children[hd].label

	assert(node.children[hd].headword != None and node.children[hd].headword != '')
        node.headword = node.children[hd].headword
        node.headtag = node.children[hd].headtag
	assert node.headtag != None and node.headtag != ''
