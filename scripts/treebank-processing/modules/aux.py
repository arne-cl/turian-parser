#
#   aux.py
#
#   Convert helper verbs to AUX or AUXG
#   Based upon rules by Don Blaheta
#   http://www.cs.brown.edu/~dpb/tbfix/aux
#
#   $Id: aux.py 1657 2006-06-04 03:03:05Z turian $
#
#
#######################################################################
# Copyright (c) 2004-2006, New York University. All rights reserved
#######################################################################

from variables import *

import parsetree

import string

# The first column is the original POS tag.
# The second column is the word.
# The third column in the new POS tag.
# Entries commented out were commented out by Blaheta.
helper_verbs = [
	("VB", "be", "AUX"),
	("VB", "were", "AUX"),
	("VBP", "am", "AUX"),
	("VBP", "'m", "AUX"),
	("VBP", "are", "AUX"),
	("VBP", "'re", "AUX"),
	("VBZ", "is", "AUX"),
	("VBZ", "'s", "AUX"),
	("VBD", "was", "AUX"),
	("VBD", "were", "AUX"),
	("VBN", "been", "AUX"),
	("VBG", "being", "AUXG"),
	("VB", "have", "AUX"),
	("VB", "had", "AUX"),
	("VBP", "have", "AUX"),
	("VBP", "'ve", "AUX"),
	("VBZ", "has", "AUX"),
	("VBD", "had", "AUX"),
	("VBD", "'d", "AUX"),
	("VBN", "had", "AUX"),
	("VBG", "having", "AUXG"),
	("VB", "do", "AUX"),
	("VBP", "do", "AUX"),
	("VBZ", "does", "AUX"),
	("VBD", "did", "AUX"),
	("VBN", "done", "AUX"),
#	("VBG", "doing", "AUXG"),
	("VB", "need", "AUX"),
	("VBP", "need", "AUX"),
#	("VBZ", "needs", "AUX"),
#	("VBD", "needed", "AUX"),
#	("VBN", "needed", "AUX"),
#	("VBG", "needing", "AUXG"),
]

def convert(tree):
	leaves = tree.leaves()
	for n in leaves:
		for (origpos, word, newpos) in helper_verbs:
			if string.lower(n.headword) != word:
				continue
			if n.label != origpos:
				continue
			n.label = newpos
			n.headtag = newpos

	return tree
