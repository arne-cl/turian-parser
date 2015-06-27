/*  $Id: head.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file head.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/head.H"

#include "universal/globals.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/AutoVector.H"
#include "universal/FastMap.H"

#include <sstream>
#include <string>
#include <list>

typedef enum { LEFT, RIGHT } direction_ty;
static FastMap<direction_ty> direction_map;
//static AutoVector<AutoVector<unsigned> > priority_map;
static vector<vector<unsigned> > priority_map;

static bool head_is_init = false;

static const unsigned head_rules_cnt = 25;
/**
 *  "The head-rules used by the parser."
 *  - [parent non-terminal] is the non-terminal on the lefthand-side of
 *  a rule.
 *  - [direction] specifies whether search starts from the left or right
 *  end of the rule.
 *  - [priority list] gives a priority ranking, with priority decreasing
 *  when moving down the list."
 *  (quoted from Collins 1999, pg. 240)
 *  MODIFICATION: AUX and AUXG were added after verb terminals.
 *  MODIFICATION: NPBs have the same priority as NPs
 */
static const string head_rules[head_rules_cnt][3] = {
	{"ADJP",	"L",	"NNS QP NN $ ADVP JJ VBN VBG AUX AUXG ADJP JJR NP JJS DT FW RBR RBS SBAR RB"},
	{"ADVP",	"R",	"RB RBR RBS FW ADVP TO CD JJR JJ IN NP JJS NN"},
	{"CONJP",	"R",	"CC RB IN"},
	{"FRAG",	"R",	""},
	{"INTJ",	"L",	""},
	{"LST",		"R",	"LS :"},
	{"NAC",		"L",	"NN NNS NNP NNPS NP NAC EX $ CD QP PRP VBG AUX AUXG JJ JJS JJR ADJP FW"},
	{"PP",		"R",	"IN TO VBG VBN AUX AUXG RP FW"},
	{"PRN",		"L",	""},
	{"PRT",		"R",	"RP"},
//	{"QP",		"L",	"$ IN NNS NN JJ RB DT CD NCD QP JJR JJS"},
// MODIFICATION: Removed "NCD"
	{"QP",		"L",	"$ IN NNS NN JJ RB DT CD QP JJR JJS"},
	{"RRC",		"R",	"VP NP ADVP ADJP PP"},
	{"S",		"L",	"TO IN VP S SBAR ADJP UCP NP"},
	{"SBAR",	"L",	"WHNP WHPP WHADVP WHADJP IN DT S SQ SINV SBAR FRAG"},
	{"SBARQ",	"L",	"SQ S SINV SBARQ FRAG"},
	{"SINV",	"L",	"VBZ VBD VBP VB MD AUX AUXG VP S SINV ADJP NP"},
	{"SQ",		"L",	"VBZ VBD VBP VB MD AUX AUXG VP SQ"},
	{"TOP",		"L",	""},	// MODIFICATION: Added this
	{"UCP",		"R",	""},
	{"VP",		"L",	"TO VBD VBN MD VBZ VB VBG VBP AUX AUXG VP ADJP NN NNS NP"},
	{"WHADJP",	"L",	"CC WRB JJ ADJP"},
	{"WHADVP",	"R",	"CC WRB"},
	{"WHNP",	"L",	"WDT WP WP$ WHADJP WHPP WHNP"},
	{"WHPP",	"R",	"IN TO FW"},
	{"X",		"L",	""},	// MODIFICATION: Added this
};

static void head_init() {
	assert(!head_is_init);

	direction_map.clear();

	priority_map.clear();
	vector<unsigned> tmp;
	for (LIST<Label>::const_iterator l = all_labels().begin(); l != all_labels().end(); l++)
		priority_map.push_back(tmp);

	for (unsigned i = 0; i < head_rules_cnt; i++) {
		Label parent = string_to_label(head_rules[i][0]);
		assert(is_constituent_label(parent));

		if (head_rules[i][1] == "L") {
			direction_map.insert(parent, LEFT);
		} else {
			assert(head_rules[i][1] == "R");
			direction_map.insert(parent, RIGHT);
		}

		unsigned cnt = 0, skipcnt = 0;
		istringstream sin(head_rules[i][2]);

		list<pair<Label, unsigned> > priority_submap;
		while (!sin.eof()) {
			string labelstr;
			sin >> labelstr >> ws;
			// Skip labels that are not in the vocabulary
			if (!is_label_string(labelstr)) {
				Debug::log(2) << "Skipping unknown label " << labelstr << " in head_init()\n";
				skipcnt++;
				continue;
			}
			Label l = string_to_label(labelstr);
			priority_submap.push_back(make_pair(l, cnt));
			// MODIFICATION: Give NPB equal priority as NP
			if (l == Label_NP())
				priority_submap.push_back(make_pair(Label_NPB(), cnt));
			cnt++;
		}
//		assert(priority_submap.size() == cnt);
		priority_map.at(parent) = vector<unsigned>(max_label(), cnt);
		list<pair<Label, unsigned> >::const_iterator l;
		for (l = priority_submap.begin(); l != priority_submap.end(); l++)
			priority_map.at(parent).at(l->first) = l->second;
	}
	direction_map.lock();
	assert(direction_map.size() == head_rules_cnt);

	head_is_init = true;
}

/// Find the priority of a label.
/// \param parent The label of the parent item.
/// \param child The label of the child item.
/// \return The priority of the child label with respect to the parent
/// label. Lower priority is better.
static unsigned head_priority(const Label parent, const Label child) {
	if (!head_is_init)
		head_init();
	assert(head_is_init);

//	Debug::log(9) << label_string(parent) << "\n";
//	assert(priority_map.exists(parent));
	assert(direction_map.exists(parent));

	return priority_map.at(parent).at(child);
}

/// Special NP head rules.
/// Based upon Collins (1999, pg. 238)
/// We also apply these rules to NX.
static unsigned noun_head_get(const vector<Label>& children) {
	assert(children.size() > 0);
	int last = children.size() - 1;

	if (children.at(last) == Label_POS())
		return last;

	for (int i = last; i >= 0; i--) {
		if (children.at(i) == Label_NN()) return i;
		if (children.at(i) == Label_NNP()) return i;
		if (children.at(i) == Label_NNPS()) return i;
		if (children.at(i) == Label_NNS()) return i;
		if (children.at(i) == Label_NX()) return i;
		if (children.at(i) == Label_POS()) return i;
		if (children.at(i) == Label_JJR()) return i;
	}

	for (int i = 0; i <= last; i++) {
		if (children.at(i) == Label_NP()) return i;
		// MODIFICATION: Added NPB here -jpt
		if (children.at(i) == Label_NPB()) return i;
	}

	for (int i = last; i >= 0; i--) {
		if (children.at(i) == Label_DOLLAR()) return i;
		if (children.at(i) == Label_ADJP()) return i;
		if (children.at(i) == Label_PRN()) return i;
	}

	for (int i = last; i >= 0; i--) {
		if (children.at(i) == Label_CD()) return i;
	}

	for (int i = last; i >= 0; i--) {
		if (children.at(i) == Label_JJ()) return i;
		if (children.at(i) == Label_JJS()) return i;
		if (children.at(i) == Label_RB()) return i;
		if (children.at(i) == Label_QP()) return i;
	}

	return last;
}

static unsigned head_get_help(const Label l, const vector<Label>& children) {
	if (!head_is_init)
		head_init();
	assert(head_is_init);

	// NP rules from Collins (1999, pg. 238)
	// MODIFICATION: We also apply these rules to NX (as suggested by Bikel'04 3.12)
	if (l == Label_NP() || l == Label_NX() || l == Label_NPB())
		return noun_head_get(children);

	int c, done, step;
	if (direction_map[l] == LEFT) {
		c = 0;
		done = children.size();
		step = +1;
	} else {
		assert(direction_map[l] == RIGHT);
		c = children.size() - 1;
		done = -1;
		step = -1;
	}

	assert(c >= 0);
	unsigned bestidx = (unsigned)c;
	c += step;
	// Find the index of the lowest-priority child,
	// iterating in the proper direction.
	while (c != done) {
		if (head_priority(l, children.at(c)) <
				head_priority(l, children.at(bestidx)))
			bestidx = c;
		c += step;
	}
	return bestidx;
}

unsigned head_get(const Label l, const vector<Label>& children) {
	unsigned idx = head_get_help(l, children);

	// Check for the special case for coordination given in pg. 239
	// of Collins (1999)
	// MODIFICATION: Collins does not apply coordination head movement
	// to basal NPs. However, we do.
	// This special case doesn't apply to basal NPs and NXs.
	// (cf. Bikel '03 section 3.3 / 3.12)
	// // FIXME: Change this "if" to a "while"?
	// \todo Add ':' and ',' to coordinating conjunction determination?
	// If so, then also change this in ItemToken.C
	if (idx > 1 && children.at(idx-1) == Label_CC() && (l != Label_NPB() && l != Label_NX()))
		return idx-2;
	return idx;
}
