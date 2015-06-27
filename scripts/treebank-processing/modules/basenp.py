#
#   basenp.py
#
#   Preprocess and postprocess basal NPs.
#
#   cf. Collins or Bikel.
#
#   $Id: basenp.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from variables import *

import parsetree
import head

def transform_node(n):
	if n.isleaf: return n
	if n.label != 'NP': return n

	# Does this node dominate no other NPs except possessive NPs?
	found = False
	good = True
	for n2 in n.internal_nodes():
		if n == n2: found = True
		else:
			if (n.label == "NP" or n.label == "NPB") and not n.is_possessive_NP():
				good = False
				break
	if not good: return n

	# Yes, so it is a basal NPs.
	assert found
	n.label = "NPB"

	assert not add_basal_np_extra_np_level
#	if add_basal_np_extra_np_level:
#		p = n.parent()
#		assert p != None
#		if (p.label != "NP") or \
#				(p.label == "NP" and p.is_coordinated_phrase()) or \
#				(p.label == "NP" and p.children[head.get("NP", [c.label for c in p.children])] != n):
#			node = parsetree.Node()
#			node.isleaf = 0
#			node.label = "NP"
#			node.children = [n]
#			p.children = n.left_siblings() + [node] + n.right_siblings()
#			p = parsetree.refresh(p)
#			n = node
#
#			assert(len(n.children) == 1)
#			npb = n.children[0]
#
#			# MODIFICATION: I've never seen the following occur
#			# in training data, so I'm just commenting it out.
##			# NPB nodes that have sentential nodes as their
##			# rightmost child are "repaired"
##			if npb.children[-1].label in sentential_nodes:
##				n.children.append(npb.children[-1])
##				npb.children = npb.children[:-1]
##				p = parsetree.refresh(p)
##				print p.to_string()

	return n

def transform(tree):
	assert add_basal_nps

	# Do a bottom-up transformation of the tree
	if not tree.isleaf:
		tree.children = [transform(c) for c in tree.children]
		tree = parsetree.refresh(tree)
	tree = transform_node(tree)

	return tree

def untransform(tree):
	assert add_basal_nps

	for n in tree.internal_nodes():
		if n.label == "NPB":
#			print n.parent().parent().to_string()
			n.label = "NP"
			assert not add_basal_np_extra_np_level
#			# If we added an extra NP level, remove it.
#			p = n.parent()
#			if add_basal_np_extra_np_level and len(p.children) == 1:
#				assert(p.children[0] == n)
#				assert(p.label == "NP")
#				p.parent().children = p.left_siblings() + [n] + p.right_siblings()
#				p = parsetree.refresh(p.parent())
##				print p.to_string()
	return tree
