#
#   parsetree.py
#
#   Read parsetrees in the Penn Treebank format.
#
#   $Id: parsetree.py 1657 2006-06-04 03:03:05Z turian $
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from variables import *

import re
import string
import weakref

from types import *

import aux
import basenp
import compounds
import head
import postags
import vocab

class ParseError(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

# Data structure based upon Table B.1 of Collins (1999)
class Node:
	"A node in a partial parse"

	# Unique identifier. The leaves from left to right are number
	# 1 ... n (n = number of leaves) and then the internal nodes are
	# numbered in any order.
	uid = -1

	# 1 = leaf (POS tag with no children), 0 = non-terminal
	isleaf = -1
	
	# the non-terminal label
	# (POS tag for leaf labels)
	label = ""

	# modifiers to the label, such as "LOC"
	# FIXME: Read about these in the Penn treebank guide
	label_modifiers = []

	# the general class into which this non-terminal label falls
	# FIXME: compute this
	label_class = ""

	# the non-terminal label of the head-child of the edge (?)
	headlabel = ""

	# the head word
	headword = ""

	# the original head word, prior to preprocessing,
	# delexicalization, &tc.
	origheadword = ""

	# the part-of-speech tag of the head-word
	headtag = ""

	# the general class into which the POS tag falls
	# FIXME: compute this
	headtag_class = ""

	# parent of this node (a weakref)
	parent = None

	# list of the children of the node (in left to right order)
	children = []

	def __init__(self, word="", tag=""):
		if tag != "":
			# Constructor for a leaf
			assert word != ""
			assert tag in postags.function or tag in postags.content
			self.isleaf = 1
		else:
			# Unknown whether this is a leaf or not
			assert word == ""
			self.isleaf = -1

		self.uid = -1
		self.label = tag
		self.label_modifiers = []
		self.headword = word
		self.origheadword = word
		self.headlabel = ""
		self.headtag = tag
		self.children = []
		self.parent = None

	def __del__(self):
		if "children" in dir(self) and len(self.children) > 0:
			del self.children
#		for v in dir(self):
#			if v[:2] != "__" and not type(self.v) is NoneType:
#				del self.getattr(v)

	# The leftmost leaf of this node
	def left_leaf(self):
		if self.isleaf:
			return self.uid
		else:
			return self.children[0].left_leaf()


	# The rightmost leaf of this node
	def right_leaf(self):
		if self.isleaf:
			return self.uid
		else:
			return self.children[-1].right_leaf()

	# The span of this node, i.e. (leftmost leaf, rightmost leaf)
	def span(self):
		return (self.left_leaf(), self.right_leaf())

	def leaves(self):
		if self.isleaf:
			return [self]
		else:
			return reduce(lambda x, y: x + y, [child.leaves() for child in self.children], [])

	# FIXME: Make sure that this returns internal nodes in a top-down
	# order, for reverse_S_transform
	def internal_nodes(self):
		if self.isleaf:
			return []
		else:
			return reduce(lambda x, y: x + y, [child.internal_nodes() for child in self.children], [self])
	
	# For the given node, return a string in the treebank/EVALB format
	# (i.e. the format read by read_tree(), but with less whitespace)
	#
	# Optional depth parameter gives the maximum depth of descendents
	# to print.
	# (Default=-1, which means print to arbitrary depth.)
	def to_string(self, depth=-1):
		if self.isleaf:
			return "(%s %s)" % (self.headtag, self.headword)
		else:
			if depth == 0:
				return "(%s ...)" % self.label
			else:
				return "(%s %s)" % (self.label, string.join([c.to_string(depth-1) for c in self.children]))
	
	# Return a list of constituents, where (lbl, (l, r)) is in the list
	# if there is a constituent labelled lbl from leaf l through leaf r.
	# Skips TOP constituents if withTOP == False
	def constituents(self, withTOP=False):
		lst = []
		if not self.isleaf:
			for c in self.children:
				lst = lst + c.constituents(withTOP=withTOP)
			if withTOP or self.label != "TOP":
				lst = lst + [(self.label, self.span())]
		return lst

	# Return a list of the ancestors of this node,
	# starting with the parent and ending with the root.
	# We return a list of weakrefs.
	# FIXME: Return this as weakrefs or not?
	def ancestors(self):
		lst = []
		p = self.parent
		while p != None:
			lst.append(p)
			p = p().parent
		return lst

	# Return a list of the left siblings of this node,
	# in left-to-right order.
	# FIXME: Return this as weakrefs or not?
	def left_siblings(self):
		p = self.parent()
		assert p != None
		lst = []
		for c in p.children:
			if c == self:
				break
			lst.append(c)
		return lst

	# Return a list of the right siblings of this node,
	# in left-to-right order.
	# FIXME: Return this as weakrefs or not?
	def right_siblings(self):
		p = self.parent()
		assert p != None
		lst = []
		found = False
		for c in p.children:
			if c == self:
				found = True
			elif found:
				lst.append(c)
		assert(found)
		return lst
	
	# Given a reference to a hashmap, return a hashmap of
	# constituents, where (l, r) is a key in the hashmap if there
	# is a constituent from leaf l through leaf r.
	# The value of each key (l, r) is a list of labels, where the
	# first label is the lowest constituent in this span (and the last
	# label is the highest constituent in this span)
	def constituents_hash(self, hsh):
		if not self.isleaf:
			for c in self.children:
				c.constituents_hash(hsh)
			p = self.span()
			if p not in hsh:
				hsh[p] = []
			hsh[p].append(self.label)

	# Add backpointers from children to parents
	def add_backpointers(self):
		if self.isleaf:
			assert(len(self.children) == 0)
		else:
			assert(len(self.children) > 0)
			for c in self.children:
				c.parent = weakref.ref(self)
				c.add_backpointers()
			#if "children" not in dir(tree) or len(tree.children) <= 0:
			#	return
			#if tree.isleaf:
			#	return

	# Remove this node, both from memory and from my parent node's
	# children list (but leave my parent intact, even if it now has
	# no children)
	def remove(self):
		assert len(self.children) == 0
		p = self.parent()
		assert p != None
		origlen = len(p.children)
		p.children = [c for c in p.children if c != self]
		assert len(p.children)+1 == origlen
		del(self)

	# Prune any leaf node which label is in badlabels,
	# as well as any internal nodes that dominate only nodes that
	# have been pruned
	def prune_labels(self, badlabels):
		if self.isleaf:
			if self.label in badlabels:
				self.remove()
		else:
			assert self.label not in badlabels
			assert len(self.children) > 0
			for c in self.children:
				c.prune_labels(badlabels)
			if len(self.children) == 0:
				self.remove()

	# Remove leftmost items, if they are punctuation items
	def remove_leftmost_punctuation(self):
		if self.isleaf:
			if self.label in punctuation_tags:
				self.remove()
				return True
			else: return False
		else:
			while len(self.children) > 0 and \
				self.children[0].remove_leftmost_punctuation():
				1
			if len(self.children) == 0:
				self.remove()
				return True
			else: return False

	# Remove rightmost items, if they are punctuation items
	def remove_rightmost_punctuation(self):
		if self.isleaf:
			if self.label in punctuation_tags:
				self.remove()
				return True
			else: return False
		else:
			while len(self.children) > 0 and \
				self.children[-1].remove_rightmost_punctuation():
				1
			if len(self.children) == 0:
				self.remove()
				return True
			else: return False

	# Raise punctuation nodes, such that they are never the leftmost or
	# rightmost child of any node but the TOP (root) node.
	# CAUTION! Don't use this on goldstandard parses, or it will
	# change the PARSEVAL scores! [I no longer think this applies -jpt]
	def raise_punctuation(self):
		if self.isleaf: return

		# First, raise the punctuation for my children
		for c in self.children:
			c.raise_punctuation()

		# If this is the root node, we cannot raise the
		# punctuation any higher.
		if self.parent == None:
			assert self.label == "TOP"
			return

		# We move my punctuation nodes up to my parent
		p = self.parent()
		assert(p != None)

		# Find my current left siblings and right siblings
		for i in range(len(p.children)):
			if p.children[i] == self: break
		assert p.children[i] == self
		lc = p.children[:i]
		rc = p.children[i+1:]
		assert(len(lc)+1+len(rc) == len(p.children))

		# Move leftmost punctuation to my left siblings
		while len(self.children) > 0 and self.children[0].label in punctuation_tags:
			lc = lc + [self.children[0]]
			self.children = self.children[1:]

		# Move rightmost punctuation to my left siblings
		while len(self.children) > 0 and self.children[-1].label in punctuation_tags:
			rc = [self.children[-1]] + rc
			self.children = self.children[:-1]

		p.children = lc + [self] + rc

		# If *all* my children are now gone
		if len(self.children) == 0:
			debug(1, "WARNING: node with label %s dominates only punctuation (%s)" % (self.label, self))
			self.remove()

	# Remove topmost items that are part of a unary chain, and that
	# have the same label as another item in the unary chain.
	# TODO: Option to remove bottommost duplicates?
	def remove_productions_to_self(self):
		if self.isleaf: return

		# First, remove self productions in my children
		for c in self.children:
			c.remove_productions_to_self()

		# If this is the root node, we're done
		if self.label == "TOP": return

		# If this is part of a unary chain, find the labels therein
		if len(self.children) != 1: return
		labels = []
		n = self
		while len(n.children) == 1:
			n = n.children[0]
			labels.append(n.label)

		# Is this a duplicate label?
		if self.label in labels:
			assert not self.children[0].isleaf

			debug(1, "Removing unary production to self %s in chain: %s " % (self.label, string.join([self.label] + labels, " <- ")))

			# If so, then remove me and link my child and my parent
			newc = self.children[0]
			self.children = []
			newc.parent = self.parent
			p = self.parent()
			assert p
			for i in range(len(p.children)):
				if p.children[i] == self: break
			assert p.children[i] == self
			p.children[i] = newc
			del(self)

 
	# Is this a possessive NP? (MacIntyre rules)
	# True iff this is an NP with a rightmost POS.
	# NB Collins + Bikel allow any child to be a POS.
	# tgrep2 indicates that an NP with a non-rightmost POS tag is
	# typically *not* a possessive NP.
	# (Technically, MacIntyre insists that the POS dominate only
	# 's or ' or 'S, but we ignore this additional requirement.)
	# (from ftp://ftp.cis.upenn.edu/pub/treebank/doc/faq.cd2)
	def is_possessive_NP(self):
		if self.isleaf: return False
		if self.label != "NP" and self.label != "NPB": return False
		if self.children[-1].isleaf and self.children[-1].headtag == 'POS': return True
		return False

# Klein+Manning / Bikel version. Unsound.
#	# Is this a possessive NP?
#	# A possessive NP immediately dominates a terminal POS node.
#	def is_possessive_NP(self):
#		if self.isleaf: return False
#		if self.label != "NP" and self.label != "NPB": return False
#		for c in self.children:
#			if c.isleaf and c.headtag == 'POS': return True
#				return False



# Assign a uid to all the nodes in the tree, starting with
# the leaves
def number_nodes(tree):
	nodecnt = 0
	for l in tree.leaves():
		l.uid = nodecnt
		nodecnt += 1
	for l in tree.internal_nodes():
		l.uid = nodecnt
		nodecnt += 1
	return tree

# Refresh a (new) tree:
# 1. Prune nodes labelled -NONE-
# 2. Assign a uid to all nodes
# 3. Add backpointers from children to parents
# 4. Add head information to the nodes
# Return the tree and a uidmap from uids to nodes
def refresh(tree):
	# Add backpointers from children to parents
	tree.add_backpointers()

	# Prune nodes labelled -NONE-
	tree.prune_labels(["-NONE-"])

	# Assign a uid to all nodes
	tree = number_nodes(tree)

	# Add backpointers from children to parents
	tree.add_backpointers()

	# Add head information to the nodes
	head.refresh(tree)

	return tree

def make_node(toks):
	if toks[0] != "(":
		raise ParseError(toks[0])


	# If this is a leaf token...
	if toks[2] != "(":
		if toks[3] != ")":
			raise ParseError(toks[0:4])

		# Create a leaf node
		node = Node(tag=toks[1], word=toks[2])
		return (node, toks[4:])

	# Otherwise, parse all subleaves (arguments)
	else:
		node = Node()
		node.isleaf = 0
		if toks[1] != '-EMPTY-':
			toks[1] = string.split(toks[1], '=')
			toks[1] = string.split(toks[1][0], '-') + toks[1][1:]
			toks[1] = string.split(toks[1][0], '|') + toks[1][1:]
			node.label = toks[1][0]
			node.label_modifiers = toks[1][1:]
		else:
			assert(0)
			#node.label = toks[1]
		toks = toks[2:]

		node.children = []
		while toks[0] != ")":
			(newchild, toks) = make_node(toks)
			if newchild != None:
				node.children.append(newchild)

		if len(node.children) == 0:
			return (None, toks[1:])
		else:
			return (node, toks[1:])

def make_tree(toks):
	if len(toks) == 0:
		return (None, toks)

	(tree, toks) = make_node(toks)

	if tree == None:
		return (tree, toks)
	else:
		tree = refresh(tree)
		return (tree, toks)

def read_tree(l):
	# Add whitespace around parentheses, for easier tokenization
	l = string.replace(l, '(', ' ( ')
	l = string.replace(l, ')', ' ) ')
	l = string.strip(l)
	toks = string.split(l)
	if len(toks) == 0:
		raise ParseError((l, toks))
	(tree, toks) = make_tree(toks)
	if len(toks) != 0:
		#sys.stderr.write(l + "\n" + `tree` + "\n" + `toks` + "\n")
		raise ParseError((l, toks))

	return tree

# Dan's Chomsky-adjunction-like S transform.
#	(S L* NP C* VP R*) -> (S L* (S NP C* VP) R*)
# where:
#	L*, C*, and R* are zero or more nodes,
#	no item in C* is NP or VP,
#	and L* or R* is non-empty.
# WARNING: This transform must occur *after* we raise punctuation.
def S_transform(tree):
	# Keep applying this transform until we cannot apply it any more.
	done = 0
	while not done:
		done = 1
		for n in tree.internal_nodes():
			if n.label != "S": continue
			if len(n.children) <= 2: continue

			for i in range(len(n.children)-1):
				c1 = n.children[i]
				if c1.label == "NP": break
			if c1.label != "NP": continue

			for j in range(i+1, len(n.children)):
				c2 = n.children[j]
				if c2.label == "NP":
					i = j
					c1 = c2
				elif c2.label == "VP": break
			if c2.label != "VP": continue

			if i == 0 and j == len(n.children)-1: continue

			# Otherwise, we've found an S to be transformed.
			done = 0

			# Create the new child S.
			node = Node()
			node.isleaf = False
			node.label = "S"
			node.children = n.children[i:j+1]
			assert node.children[0].label == "NP"
			assert node.children[-1].label == "VP"
	
			# Perform the transform
#			print
#			print n.to_string(depth=1)
			n.children = c1.left_siblings() + [node] + c2.right_siblings()
			assert len(n.children) > 1
			n = refresh(n)
#			print n.to_string(depth=2)

			# Break out of the loop, since the range may have changed.
			# If there are others NP VP children to be transformed,
			# we'll get them in the next pass.
			break

	return tree

# Convert (S _+ (S NP _* VP) _+) -> (S _+ NP _* VP _+)
# where "_+" is one or more nodes.
# FIXME: Not deleting r after reverse transform can leak memory?
def reverse_S_transform(tree):
	for n in tree.internal_nodes():
		if n.label != "S": continue
		if len(n.children) < 2: continue
		if n.children[0].label != "NP": continue
		if n.children[-1].label != "VP": continue
		p = n.parent()
		assert p != None
		if p.label != 'S': continue
	
		# Otherwise, we've found an S to be reverse transformed.
		p.children = n.left_siblings() + n.children + n.right_siblings()
		p = refresh(p)

	return tree

## A modification of the above Chomsky-adjunction-like S transform.
##	(S _* NP VP) -> (S _* (SPRIME NP VP))
## where "_*" is zero or more nodes.
## WARNING: This transform must occur *after* we raise punctuation.
#def SPRIME_transform(tree):
#	for n in tree.internal_nodes():
#		if n.label != "S": continue
#		if len(n.children) <= 1: continue
#		if n.children[-2].label != "NP": continue
#		if n.children[-1].label != "VP": continue
#
#		# Otherwise, we've found an S to be transformed,
#		# so create the new child SPRIME.
#		node = Node()
#		node.isleaf = False
#		node.label = "SPRIME"
#		node.children = n.children[-2:]
#
#		# Perform the transform
##		print
##		print n.to_string(depth=1)
#		n.children = n.children[:-2] + [node]
#		n = refresh(n)
##		print n.to_string(depth=2)
#
#	return tree
#
## Convert (S _* (SPRIME NP VP)) -> (S _* NP VP)
## where "_*" is one or more nodes.
## FIXME: Not deleting r after reverse transform can leak memory?
#def reverse_SPRIME_transform(tree):
#	for n in tree.internal_nodes():
#		if n.label != "S": continue
#		if len(n.children) < 1: continue
#		r = n.children[-1]
#		if r.label != "SPRIME": continue
#		if len(r.children) != 2: continue
#		if r.children[0].label != "NP": continue
#		if r.children[1].label != "VP": continue
#
#		# Otherwise, we've found an S to be reverse transformed.
#		n.children = n.children[:-1] + r.children
#		n = refresh(n)
#
#	return tree

# Preprocess a tree, *after* tagging:
# 1. Convert PRT to ADVP
# 2. Remove quotation marks
# 3. Raise punctuation
# 4. Convert base NPs to NPB
# 5. Remove unary projections to self
# 6. S transform.
# 7. SPRIME transform.
# 8. Lowercase headwords
# 9. Delexicalize words
# 10. Refresh the tree
def preprocess(tree, delexicalize=True):
	if convert_PRT_to_ADVP:
		for n in tree.internal_nodes():
			if n.label == 'PRT':
				n.label = 'ADVP'

	if remove_quotation_marks:
		tree.prune_labels(["``", "''"])

	if remove_outermost_punctuation:
		tree.remove_leftmost_punctuation()
		tree.remove_rightmost_punctuation()

	if raise_punctuation:
		tree.raise_punctuation()

	# Noise that can be removed reliably should be removed as early as possible.
	if remove_unary_projections_to_self:
		tree.remove_productions_to_self()

	# This should occur *after* we remove unary projections to self.
	if add_basal_nps:
		tree = basenp.transform(tree)

	# This transform must occur *after* we raise punctuation.
	if use_S_transform:
		assert not add_basal_nps
		tree = S_transform(tree)

		# Sanity check
		for n in tree.internal_nodes():
			if n.label != "S": continue
			if len(n.children) < 2: continue
			if n.children[0].label == "NP" and n.children[-1].label == "VP":
				assert len([c for c in n.children[1:-1] if c.label == "NP"])==0
				assert len([c for c in n.children[1:-1] if c.label == "VP"])==0

		# Sanity check that the transform is stable.
		treestr = tree.to_string()
		tree = S_transform(tree)
		assert tree.to_string() == treestr

	# This transform must occur *after* we raise punctuation.
	if use_SPRIME_transform:
		assert not add_basal_nps
		tree = SPRIME_transform(tree)

#		# Sanity check that the transform is stable.
#		treestr = tree.to_string()
#		tree = SPRIME_transform(tree)
#		assert tree.to_string() == treestr

	if lowercase_vocabulary:
		for n in tree.leaves():
			n.headword = string.lower(n.headword)

	if delexicalize:
		# Delexicalize infrequent words (words not in the vocabulary)
		vocab.read()
		for n in tree.leaves():
			if n.headword not in vocab.vocab_to_idx:
				n.headword = '*rare*'

	tree = refresh(tree)

	return tree

# Regularize a tree, *prior* to tagging:
# 1. Flatten closed class compounds
# 2. Add AUX tags.
def regularize(tree):
	if flatten_closed_class_compounds:
		tree = compounds.preprocess_closed_class_compounds(tree)
		tree = refresh(tree)

	if add_AUX:
		tree = aux.convert(tree)

	return tree

# Reverse regularize a tree, *prior* to postprocessing:
# 1. Reverse flatten closed class compounds
def reverse_regularize(tree):
	if flatten_closed_class_compounds:
		tree = compounds.postprocess_closed_class_compounds(tree)
		tree = refresh(tree)

	return tree

# Postprocess a tree, given the original leaves of this tree, as well
# as the leaves of corresponding the treebank tree
# (Leaves are represented as a list of pairs (word, tag))
# This postprocessed tree can evaluated against the treebank tree
# using EVALB
#
# Steps:
# 0) Reverse the S transform or the SPRIME transform.
# 0.5) Convert NPBs to NPs
# 1) Reinsert leftmost and rightmost punctuation from the original leaf
# set, skipping quotation marks.
# 2) Reinsert quotation marks from the original leaf set, which were
# pruned and do not appear in the current tree.
# 3) Restore the original headwords (those prior to lowercasining and
# delexicalization)
# 4) Raise the newly inserted (quotation) punctuation
# 5) Refresh the tree
# 6) Finally, retag the tree using the treebank tags
#
#
def postprocess(tree, origleaves=None, treebank_leaves=None, reverse_Stransform=True):
	assert origleaves != None
	assert treebank_leaves != None

	if use_S_transform and reverse_Stransform:
		tree = reverse_S_transform(tree)

#		# Sanity check that the reverse transform is stable.
#		treestr = tree.to_string()
#		tree = reverse_S_transform(tree)
#		if tree.to_string() != treestr
#		assert tree.to_string() == treestr

	if use_SPRIME_transform and reverse_Stransform:
		tree = reverse_SPRIME_transform(tree)

#		# Sanity check that no SPRIME remains
#		for n in tree.internal_nodes():
#			assert n.label != "SPRIME"
#
#		# Sanity check that the reverse transform is stable.
#		treestr = tree.to_string()
#		tree = reverse_SPRIME_transform(tree)
#		assert tree.to_string() == treestr

#	if remove_unary_projections_to_self:
#		tree.remove_productions_to_self()

	if add_basal_nps:
		tree = basenp.untransform(tree)

	if remove_outermost_punctuation:
		# Reinsert leftmost punctuation tags
		leftmost_nodes = []
		p = tree.leaves()[0].parent()
		assert p != None
		for i in range(len(origleaves)):
			n = origleaves[i]
			# If we're at the first item of the parse tree...
			if n[1] == tree.leaves()[0].headtag or \
			(n[0] == tree.leaves()[0].headword and n[1] not in punctuation_tags) or \
			(lowercase_vocabulary and string.lower(n[0]) == tree.leaves()[0].headword and n[1] not in punctuation_tags):
				# ...then stop inserting leftmost punctuation
				assert n[1] not in punctuation_tags
				break

			# If we're at a quotation mark and we had removed
			# quotation marks, then skip it for now (we'll
			# reinsert the quotation marks laters)
			if n[1] in ["``", "''"] and remove_quotation_marks:
				continue

			# Otherwise, this should be a punctuation tag,
			# so reinsert it
			assert n[1] in punctuation_tags
			newnode = Node(word=n[0], tag=n[1])
			newnode.parent = weakref.ref(p)
			leftmost_nodes.append(newnode)
		# Add new nodes to the left of the parser's first item
		p.children = leftmost_nodes + p.children


		# If we've reinserted every single item in the original leaves
		if i == len(origleaves):
			assert len(tree.leaves()) == 0
			# Just crash, this case shouldn't really occur
			assert 0
		else: assert len(tree.leaves()) != 0

	
		# Reinsert rightmost punctuation tags
		rightmost_nodes = []
		p = tree.leaves()[-1].parent()
		assert p != None
		for i in range(len(origleaves)-1, 0, -1):
			n = origleaves[i]
			# If we're at the final item of the parse tree...
			if n[1] == tree.leaves()[-1].headtag or \
			(n[0] == tree.leaves()[-1].headword and n[1] not in punctuation_tags) or \
			(lowercase_vocabulary and string.lower(n[0]) == tree.leaves()[-1].headword and n[1] not in punctuation_tags):
				# ...then stop inserting rightmost punctuation
				assert n[1] not in punctuation_tags
				break

			# If we're at a quotation mark and we had removed
			# quotation marks, then skip it for now (we'll
			# reinsert the quotation marks laters)
			if n[1] in ["``", "''"] and remove_quotation_marks:
				continue

			# Otherwise, this should be a punctuation tag,
			# so reinsert it
			assert n[1] in punctuation_tags
			newnode = Node(word=n[0], tag=n[1])
			newnode.parent = weakref.ref(p)
			rightmost_nodes.append(newnode)
		rightmost_nodes.reverse()
		# Add new nodes to the right of the parser's final item
		p.children = p.children + rightmost_nodes

	# Reinsert all but rightmost quotation marks
	i = 0
	for n in tree.leaves():
		if remove_quotation_marks:
			# Reinsert quotation marks
			while origleaves[i][1] in ["``", "''"]:
				newnode = Node(word=origleaves[i][0], tag=origleaves[i][1])
				p = n.parent()
				assert p != None
				newnode.parent = p

				for j in range(len(p.children)):
					if p.children[j] == n: break
				assert p.children[j] == n
				p.children = p.children[:j] + [newnode] + p.children[j:]
				i += 1

#		assert(n.origheadword == origleaves[i][0])
		# Restore the original head word of this node
		n.headword = origleaves[i][0]
		i += 1

	# Reinsert rightmost quotation marks
	n = tree.leaves()[-1]
	p = n.parent()
	assert p != None
	for j in range(len(p.children)):
		if p.children[j] == n: break
	assert p.children[j] == n
	assert j == len(p.children)-1
	while i != len(origleaves) and origleaves[i][1] in ["``", "''"]:
		newnode = Node(word=origleaves[i][0], tag=origleaves[i][1])
		newnode.parent = p
		p.children = p.children + [newnode]
		i += 1
	#print i, len(origleaves)
	#print [(n.headword, n.headtag) for n in tree.leaves()]
	#print origleaves
	assert(i == len(origleaves))

	# Raise the newly inserted (quotation) punctuation
	if raise_punctuation:
		tree.raise_punctuation()

	# Refresh the tree
	tree = refresh(tree)

	# Finally, retag using the treebank tags
	i = 0
	assert len(tree.leaves()) == len(treebank_leaves)
	for n in tree.leaves():
		# FIXME: Want case sensitive?
		assert string.lower(treebank_leaves[i][0]) == string.lower(n.headword)
		n.headword = treebank_leaves[i][0]
		n.headtag = treebank_leaves[i][1]
		i += 1

	return tree

# For a list of nodes, find the lowest common ancestor of all the nodes.
# Return this lowest common ancestor as a weakref.
# FIXME: Faster? (e.g. use hash, not list)
def lowest_common_ancestor(nodes):
	assert(len(nodes) > 1)

	alst = []
	for n in nodes:
		alst.append(n.ancestors())

	for n in alst[0]:
		found = True;
		for a in alst[1:]:
			if n not in a:
				found = False
				break
		if found:
			for a in alst: assert n in a
			return n
	assert 0

# Normalize a tree for PARSEVAL scoring
def normalize(tree):
	for n in tree.internal_nodes():
		if n.label == 'PRT': n.label = 'ADVP'

	tree.prune_labels(["``", "''"])
	tree.remove_leftmost_punctuation()
	tree.remove_rightmost_punctuation()
	tree.raise_punctuation()
	tree = refresh(tree)
	return tree
