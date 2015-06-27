#
#   compounds.py
#
#   Preprocess and postprocess closed class compounds.
#
#   Based upon a GLARF transform from Adam Meyers.
#   For more information, cf. documentation/compound-rules.txt
#
#   This is case insenstive.
#
#   $Id: compounds.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from variables import *

import parsetree

import string

compounds = [
	(["a", "heck", "of"], "DT",	"(*P* *L* (NP (DT a) (NN heck)) (PP (IN of) *R*))"),
	(["a", "lot", "of"], "DT",	"(*P* *L* (NP (DT a) (NN lot)) (PP (IN of) *R*))"),
	(["according", "to"], "IN",	"(*P* *L* (VBG according) (PP (TO to) *R*))"),
	(["ahead", "of"], "IN",		"(*P* *L* (RB ahead) (PP (IN of) *R*))"),
	(["as", "for"], "IN",		"(*P* *L* (IN as) (PP (IN for) *R*))"),
	(["as", "of"], "IN",		"(*P* *L* (IN as) (PP (IN of) *R*))"),
	(["out", "of"], "IN",		"(*P* *L* (IN out) (PP (IN of) *R*))"),
	(["due", "to"], "IN",		"(*P* *L* (JJ due) (PP (TO to) *R*))"),
	(["next", "to"], "IN",		"(*P* *L* (JJ next) (PP (TO to) *R*))"),
	(["such", "as"], "IN",		"(*P* *L* (JJ such) (IN as) *R*)"),
	(["fewer", "than"], "RB",	"(*P* *L* (JJR fewer) (IN than) *R*)"),
	(["more", "than"], "RB",	"(*P* *L* (JJR more) (IN than) *R*)"),
	(["greater", "than"], "RB",	"(*P* (ADJP *L* (JJR greater)) (PP (IN than) *R*))"),
	(["as", "little", "as"], "RB",	"(*P* *L* (RB as) (JJ little) (IN as) *R*)"),
	(["as", "few", "as"], "RB",	"(*P* *L* (RB as) (JJ few) (IN as) *R*)"),
	(["as", "many", "as"], "RB",	"(*P* *L* (RB as) (JJ many) (IN as) *R*)"),
	(["as", "much", "as"], "RB",	"(*P* *L* (RB as) (JJ much) (IN as) *R*)"),
	(["as", "well", "as"], "CC",	"(*P* *L* (CONJP (RB as) (RB well) (IN as)) *R*)"),
	(["in", "favor", "of"], "IN",	"(*P* *L* (IN in) (NP (NP (NN favor)) (PP (IN of) *R*)))"),
	(["up", "to"], "IN",		"(*P* *L* (IN up) (TO to) *R*)"),
	(["sort", "of"], "RB",		"(*P* *L* (ADVP (NN sort) (IN of)) *R*)"),
]

compound_words = [string.join(words, "+") for (words, pos, structure) in compounds]

# \param tree A parsetree (type Node)
def preprocess_closed_class_compounds(tree):
	assert flatten_closed_class_compounds

	found = True
	while found:
		found = False
		leaves = tree.leaves()
		for i in range(len(leaves)):
			for (words, pos, structure) in compounds:
				l = len(words)
				if i + l >= len(leaves):
					continue
				match = True
				for j in range(l):
					if string.lower(leaves[i+j].headword) != words[j]:
						match = False
						break
				if not match: continue
	
#				print tree.to_string()
#				print words
				p = parsetree.lowest_common_ancestor(leaves[i+0:i+l])
	
				left_siblings = []
				right_siblings = []
	
				# Collect, for left siblings for the new node,
				# the left siblings of all ancestors of the leftmost word,
				# from the child of p down to the leftmost word.
				leftmost_ancestors = leaves[i+0].ancestors()
				leftmost_ancestors.reverse()
				found = False
				for a in leftmost_ancestors:
					if a == p: found = True
					elif found == True: left_siblings += a().left_siblings()
				assert found
				left_siblings += leaves[i+0].left_siblings()
	
				# Collect, for right siblings for the new node,
				# the right siblings of all ancestors of the rightmost word,
				# from rightmost word up through the child of p.
				right_siblings += leaves[i+l-1].right_siblings()
				for a in leaves[i+l-1].ancestors():
					if a == p: break
					right_siblings += a().right_siblings()
				assert a == p
	
				leafcnt = len(words)
				for n in left_siblings + right_siblings:
					leafcnt += len(n.leaves())
				assert(len(p().leaves()) == leafcnt)
	
				# Exception for "sort of", in the case that p is an NP
				# (this finds the adjectival usage of "sort of", as opposed to
				# the default adverbial usage).
				if words == ["sort", "of"] and p().label == "NP":
					compound = parsetree.Node(tag="JJ", word=string.join(words, "+"))
				else:
					compound = parsetree.Node(tag=pos, word=string.join(words, "+"))
				node = parsetree.Node()
				node.isleaf = 0
				node.label = p().label
				node.children = left_siblings + [compound] + right_siblings
	
#				print [l.to_string() for l in p().leaves()]
#				print [n.to_string() for n in left_siblings], [n.to_string() for n in right_siblings]
#				print len(p().leaves()), leafcnt
#				if len(left_siblings) > len(leaves[i+0].left_siblings()):
#				print words, p().to_string(), node.to_string()
	
#				print p().to_string()
				newchildren = p().left_siblings() + [node] + p().right_siblings()
				assert p().parent != None
				p().parent().children = newchildren
#				sys.stdout.flush()

				found = True
				break
			if found: break
		if found: tree = parsetree.refresh(tree)
	return tree

# \param tree A parsetree (type Node)
# WRITEME
def postprocess_closed_class_compounds(tree):
	assert flatten_closed_class_compounds
	leaves = tree.leaves()
	for n in leaves:
		for (words, pos, structure) in compounds:
			if string.lower(n.headword) != string.join(words, "+"):
				continue

			# Exception for "sort of", in the case that p is an NP
			# (this finds the adjectival usage of "sort of", as opposed to
			# the default adverbial usage).
			if words == ["sort", "of"] and n.parent().label == "NP":
				structure = "(*P* (NP *L* (NN sort)) (PP (IN of) *R*))"

			structure = string.replace(structure, "*P*", n.parent().label)
			structure = string.replace(structure, "*L*", string.join([l.to_string() for l in n.left_siblings()]))
			structure = string.replace(structure, "*R*", string.join([r.to_string() for r in n.right_siblings()]))
			p = n.parent()
			assert p != None
#			print p.to_string()
			newnode = parsetree.read_tree(structure)
			p.children = newnode.children
			tree = parsetree.refresh(tree)
#			print p.to_string()

	return tree

