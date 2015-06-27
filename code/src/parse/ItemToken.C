/*  $Id: ItemToken.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ItemToken.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/ItemToken.H"
#include "parse/Action.H"
#include "parse/head.H"

#include <cassert>

/// Locate the start and end indices of an Action's items in this span.
/// \note If the action is of length 1, then the start index == end index.
const pair<unsigned, unsigned> ItemTokens::indices_of(const Action& a) const {
	pair<unsigned, unsigned> p;

	ItemTokens::const_iterator i;
	unsigned cnt = 0;
	for (i = this->begin(); i != this->end(); i++, cnt++) {
		assert(this->at(cnt) == *i);
		if (*i == a.children().front()) {
			p.first = cnt;
			break;
		}
	}
	p.second = p.first + a.children().size() - 1;
	for (; cnt <= p.second; i++, cnt++) {
		assert(i != this->end());
		assert(*i == a.children().at(cnt - p.first));
	}
	assert(a.children().front() == this->at(p.first));
	assert(a.children().back() == this->at(p.second));
	return p;
}

const ID<Sentence>& ItemTokens::sentence() const {
	assert(!this->empty());
	// Sanity check
	for (ItemTokens::const_iterator i = this->begin();
			i != this->end(); i++) {
		assert(!(*i)->sentence().empty());
		assert((*i)->sentence() == this->front()->sentence());
	}
	return this->front()->sentence();
}

ItemToken::ItemToken() : Item() {
	m_children.clear();
	m_head = NO_VALUE;
	m_sentence = ID<Sentence>();
	assert(m_sentence.empty());
}

/// Constituent ItemToken constructor
ItemToken::ItemToken(const Action& a) : Item() {
	assert(a.children().size() > 0);

	m_children.clear();
	m_head = NO_VALUE;
	m_is_terminal = false;
	m_label = a.label();
	m_children = a.children();
	m_span = a.span();
	m_sentence = m_children.sentence();

	this->update_head();
	this->set_predicates();
}

/// Terminal ItemToken constructor
ItemToken::ItemToken(const Label l, const Word w, const unsigned location, const ID<Sentence>& sentence) : Item() {
	assert(l != NO_LABEL);
	assert(w != NO_WORD);
	assert(is_terminal_label(l));

	m_children.clear();
	m_head = NO_VALUE;
	m_is_terminal = true;
	m_label = l;
	m_span = Span(location);
	assert(m_span.width() == 1);
	m_children.clear();
	m_headword = w;
	m_headlabel = NO_LABEL;
	m_headtag = l;
	m_headtagclass = tag_to_posclass(l);
	m_head = NO_VALUE;
	m_sentence = sentence;
	assert(!m_sentence.empty());

	this->set_predicates();
}

/// Find the terminal Item that heads this ItemToken.
const ItemToken& ItemToken::head_terminal() const {
	if (this->is_terminal()) return *this;
	return this->children().at(this->head())->head_terminal();
}

/// \todo Slow to return Action?
const Action ItemToken::action() const {
	return Action(this->label(), this->children());
}

void ItemToken::update_head() {
	assert(!m_is_terminal);

	vector<Label> childlabels;
	ItemTokens::iterator i;
	for (i = m_children.begin(); i != m_children.end(); i++) {
		childlabels.push_back((*i)->m_label);
	}

	m_head = head_get(m_label, childlabels);
	const ItemToken& head = *m_children.at(m_head);
	assert(head.m_label != NO_LABEL);

	if (!head.m_is_terminal) {
		assert(is_constituent_label(head.m_label));

		// Head label can only be a constituent label
		m_headlabel = head.m_label;
	} else {
		m_headlabel = NO_LABEL;
	}

	assert(head.m_headword != NO_WORD);
	assert(head.m_headtag != NO_LABEL);
	assert(is_terminal_label(head.m_headtag));

	m_headword = head.m_headword;
	m_headtag = head.m_headtag;
	m_headtagclass = head.m_headtagclass;
}

/// The number of terminals dominated by this ItemToken.
unsigned ItemToken::terminal_cnt(void) const {
	return m_terminal_cnt;
}

unsigned ItemToken::_terminal_cnt(void) const {
	if (this->is_terminal()) return 1;
	else return this->children().terminal_cnt();
}

/// The number of terminals dominated by these ItemTokens.
unsigned ItemTokens::terminal_cnt(void) const {
	unsigned cnt = 0;
	ItemTokens::const_iterator i;
	for (i = this->begin(); i != this->end(); i++) {
		cnt += (*i)->terminal_cnt();
	}
	return cnt;
}

	
/// Retrieve all terminals dominated by this item.
/// \param terminals All terminals dominated by this item will be appended to this list.
void ItemToken::get_terminals(ItemTokens& terminals) const {
	if (this->is_terminal()) {
		terminals.push_back(this);
	} else {
		for (ItemTokens::const_iterator i = this->children().begin(); i != this->children().end(); i++) {
			const ItemToken& item = **i;
			item.get_terminals(terminals);
		}
	}
}

string ItemToken::to_string(bool longform) const {
	ostringstream s;

//	s << "(";
	if (longform) {
		if (m_label != m_headtag) {
			s << label_string(m_label) << " ";
		}
		s << label_string(m_headtag) << "/" << word_string(m_headword);
	} else {
		s << label_string(this->label());
		if (this->is_terminal())
			s << " " << word_string(this->headword());
	}
//	s << ")";

	return s.str();
}

string ItemToken::to_string_with_children(bool longform) const {
	string s = "(";
	s += this->to_string(longform);

	if (this->is_terminal())
		return s + ")";

	ItemTokens::const_iterator c;
	for (c = this->children().begin(); c != this->children().end(); c++)
		s += " " + (*c)->to_string_with_children(longform);

	return s + ")";
}

string ItemTokens::to_string(bool longform) const {
	ostringstream s;
	ItemTokens::const_iterator i;
	for (i = this->begin(); i != this->end(); i++) {
		s << (*i);
		if (i+1 != this->end())
			s << ", ";
	}
	return "(" + s.str() + ")";
}


void ItemToken::set_predicates() {
	m_terminal_cnt = this->_terminal_cnt();
	m_is_verbal_headtag = this->_is_verbal_headtag();
	m_is_verbal_label = this->_is_verbal_label();
	m_dominates_some_verb = this->_dominates_some_verb();
	m_only_one_child = this->_only_one_child();
	m_is_basal_NP = this->_is_basal_NP();
	m_is_gapped_S = this->_is_gapped_S();
	m_is_coordinated_phrase = this->_is_coordinated_phrase();
/*
	m_are_all_children_terminals = this->_are_all_children_terminals();
	m_is_right_recursive_NP = this->_is_right_recursive_NP();
	m_is_possessive_NP_Klein_and_Manning_test = this->_is_possessive_NP_Klein_and_Manning_test();
	m_is_possessive_NP_MacIntyre_test = this->_is_possessive_NP_MacIntyre_test();
	*/
}

/// Is this node a verbal node (headtag check)?
bool ItemToken::_is_verbal_headtag() const {
	if (this->headtag() == Label_MD()) return true;
	if (this->headtag() == Label_VB()) return true;
	if (this->headtag() == Label_VBD()) return true;
	if (this->headtag() == Label_VBG()) return true;
	if (this->headtag() == Label_VBN()) return true;
	if (this->headtag() == Label_VBP()) return true;
	if (this->headtag() == Label_VBZ()) return true;
	if (this->headtag() == Label_AUX()) return true;
	if (this->headtag() == Label_AUXG()) return true;

	return false;
}


/// Is this node a verbal node (label check)?
bool ItemToken::_is_verbal_label() const {
	if (!this->is_terminal()) return false;

	if (this->label() == Label_MD()) return true;
	if (this->label() == Label_VB()) return true;
	if (this->label() == Label_VBD()) return true;
	if (this->label() == Label_VBG()) return true;
	if (this->label() == Label_VBN()) return true;
	if (this->label() == Label_VBP()) return true;
	if (this->label() == Label_VBZ()) return true;
	if (this->label() == Label_AUX()) return true;
	if (this->label() == Label_AUXG()) return true;

	return false;
}


/// Is this node a verbal node or does it dominate a verbal node?
bool ItemToken::_dominates_some_verb() const {
	if (this->is_terminal()) return this->is_verbal_headtag() || this->is_verbal_label();

	ItemTokens::const_iterator i;
	for (i = this->children().begin(); i != this->children().end(); i++)
		if ((*i)->dominates_some_verb())
			return true;

	return false;
}

/// Does this item have exactly one child?
/// (UNARY-INTERNAL in Klein+Manning)
bool ItemToken::_only_one_child() const {
	if (this->is_terminal()) return false;

	return this->children().size() == 1;
}

/*
 *  The following feature tests don't work if there are NPBs
 *  


/// Are all the children of this node terminals?
bool ItemToken::_are_all_children_terminals() const {
	if (this->is_terminal()) return false;

	ItemTokens::const_iterator i;
	for (i = this->children().begin(); i != this->children().end(); i++)
		if ((*i)->is_terminal())
			return false;

	return true;
}


/// Is this a right-recursive NP?
/// An NP is right-recursive iff the rightmost child is an NP.
/// (from Klein+Manning)
bool ItemToken::_is_right_recursive_NP() const {
	if (this->is_terminal()) return false;

	if (this->label() != Label_NP()) return false;
	if (this->children().back()->label() != Label_NP()) return false;

	return true;
}


/// Is this a possessive NP? (Klein+Manning rules)
/// According to Klein+Manning, an NP is a possessive NP iff the leftmost child is an NP.
/// In others, is_left_recursive_NP().
bool ItemToken::_is_possessive_NP_Klein_and_Manning_test() const {
	if (this->is_terminal()) return false;

	if (this->label() != Label_NP()) return false;
	if (this->children().front()->label() != Label_NP()) return false;

	return true;
}
*/


/// Is this a possessive NP? (MacIntyre rules)
/// True iff this is an NP with a rightmost POS.
/// NB Collins + Bikel allow any child to be a POS.
/// tgrep2 indicates that an NP with a non-rightmost POS tag is typically *not* a possessive NP.
/// (Technically, MacIntyre insists that the POS dominate only 's or ' or 'S, but we ignore this additional requirement.)
/// (from ftp://ftp.cis.upenn.edu/pub/treebank/doc/faq.cd2)
bool ItemToken::_is_possessive_NP_MacIntyre_test() const {
	if (this->is_terminal()) return false;

	if (this->label() != Label_NP()) return false;

	return this->children().back()->label() == Label_POS();
}

/// Is this a Basal NP?
/// True iff this is an NP that dominates no non-possessive NP.
/// (from Bikel)
bool ItemToken::_is_basal_NP() const {
	if (this->is_terminal()) return false;

	if (this->label() != Label_NP()) return false;

	ItemTokens::const_iterator i;
	for (i = this->children().begin(); i != this->children().end(); i++)
		if (!(*i)->is_terminal() &&
				(*i)->label() == Label_NP() &&
				!(*i)->_is_possessive_NP_MacIntyre_test())
			return false;

	return true;
}


/// Is this a gapped S?
/// True iff the head is a VP which is the leftmost child.
/// Our SG test is simpler than that of Collins + Bikel (and stricter), since we don't allow pre-head arguments.
/// (based upon Bikel)
bool ItemToken::_is_gapped_S() const {
	if (this->is_terminal()) return false;

	if (this->label() != Label_S()) return false;
	if (this->children().at(m_head)->label() != Label_VP()) return false;
	assert(this->headlabel() == Label_VP());
	if (m_head > 0) return false;

	return true;
}


/// Is this a coordinated phrase?
/// True iff:
/// There is a non-head child that is a coordinating conjunction
///	and
/// It is post-head but non-final, or it is immediately pre-head but non-initial.
/// (from Bikel)
bool ItemToken::_is_coordinated_phrase() const {
	if (this->is_terminal()) return false;

	unsigned i;
	for (i = 0; i < this->children().size(); i++) {
		// Is there a non-head child that is a coordinating conjunction?
		if (this->children().at(i)->label() == Label_CC() &&
				i != m_head) {
			// Is it post-head but non-final?
			if (i > m_head && i < this->children().size()-1) return true;

			// Is it immediately pre-head but non-initial?
			if (i == m_head-1 && i != 0) return true;
		}
	}
	return true;
}

bool ItemToken::is_verbal_headtag() const { return m_is_verbal_headtag; }
bool ItemToken::is_verbal_label() const { return m_is_verbal_label; }
bool ItemToken::dominates_some_verb() const { return m_dominates_some_verb; }
bool ItemToken::only_one_child() const { return m_only_one_child; }
/*
bool ItemToken::are_all_children_terminals() const { return m_are_all_children_terminals; }
bool ItemToken::is_right_recursive_NP() const { return m_is_right_recursive_NP; }
bool ItemToken::is_possessive_NP_Klein_and_Manning_test() const { return m_is_possessive_NP_Klein_and_Manning_test; }
bool ItemToken::is_possessive_NP_MacIntyre_test() const { return m_is_possessive_NP_MacIntyre_test; }
*/
bool ItemToken::is_basal_NP() const { return m_is_basal_NP; }
bool ItemToken::is_gapped_S() const { return m_is_gapped_S; }
bool ItemToken::is_coordinated_phrase() const { return m_is_coordinated_phrase; }

/// Is this a content word?
/// i.e. Is the tag of this word a content tag?
/// \todo FIXME: Move the content tag list to globals.H
bool ItemToken::is_content_word() const {
	if (this->headtag() == Label_CD()) return true;
	if (this->headtag() == Label_FW()) return true;
	if (this->headtag() == Label_JJ()) return true;
	if (this->headtag() == Label_JJR()) return true;
	if (this->headtag() == Label_JJS()) return true;
	if (this->headtag() == Label_LS()) return true;
	if (this->headtag() == Label_NN()) return true;
	if (this->headtag() == Label_NNS()) return true;
	if (this->headtag() == Label_NNP()) return true;
	if (this->headtag() == Label_NNPS()) return true;
	if (this->headtag() == Label_RB()) return true;
	if (this->headtag() == Label_RBR()) return true;
	if (this->headtag() == Label_RBS()) return true;
	if (this->headtag() == Label_SYM()) return true;
	if (this->headtag() == Label_UH()) return true;
	if (this->headtag() == Label_VB()) return true;
	if (this->headtag() == Label_VBD()) return true;
	if (this->headtag() == Label_VBG()) return true;
	if (this->headtag() == Label_VBN()) return true;
	if (this->headtag() == Label_VBP()) return true;
	if (this->headtag() == Label_VBZ()) return true;

	return false;
}

/// Is this a noun phrase?
/// True iff the label is NP or NPB
bool ItemToken::is_noun_phrase() const {
	return this->label() == Label_NP() || this->label() == Label_NPB();
}

/// Is this a noun?
/// True iff the headtagclass is '::N' or '::NP'.
bool ItemToken::is_noun() const {
	return this->headtagclass() == Posclass_N() || this->headtagclass() == Posclass_NP();
}

/// True iff the label is WHADJP, WHADVP, WHNP, or WHPP
bool ItemToken::is_WH_label() const {
	return this->label() == Label_WHADJP() || this->label() == Label_WHADVP() || this->label() == Label_WHNP() || this->label() == Label_WHPP();
}

/// True iff the label is VB, VBN, VBG, VBZ, VBD, or VBP
bool ItemToken::is_V_label() const {
	return this->label() == Label_VB() || this->label() == Label_VBN() || this->label() == Label_VBG() || this->label() == Label_VBZ() || this->label() == Label_VBD() || this->label() == Label_VBP();
}

/// True iff the label is NN or NNS
bool ItemToken::is_NN_label() const {
	return this->label() == Label_NN() || this->label() == Label_NNS();
}

/// True iff the label is JJ or JJS
bool ItemToken::is_JJ_JJS_label() const {
	return this->label() == Label_JJ() || this->label() == Label_JJS();
}

/// True iff the label is JJ or JJR
bool ItemToken::is_JJ_JJR_label() const {
	return this->label() == Label_JJ() || this->label() == Label_JJR();
}

/// True iff the label is RB or RP
bool ItemToken::is_RB_RP_label() const {
	return this->label() == Label_RB() || this->label() == Label_RP();
}

/// True iff the label is RBR or RBS
bool ItemToken::is_RBR_RBS_label() const {
	return this->label() == Label_RBR() || this->label() == Label_RBS();
}

/// True iff the label is PP or ADVP
bool ItemToken::is_PP_ADVP_label() const {
	return this->label() == Label_PP() || this->label() == Label_ADVP();
}

/// True iff the label is PP or SBAR 
bool ItemToken::is_PP_SBAR_label() const {
	return this->label() == Label_PP() || this->label() == Label_SBAR();
}

/// True iff the label is SBAR or ADJP
bool ItemToken::is_SBAR_ADJP_label() const {
	return this->label() == Label_SBAR() || this->label() == Label_ADJP();
}

/// True iff the label is SBAR, ADJP, or UCP
bool ItemToken::is_SBAR_ADJP_UCP_label() const {
	return this->label() == Label_SBAR() || this->label() == Label_ADJP() || this->label() == Label_UCP();
}

/// True iff the label is EX, PRP, or QP
bool ItemToken::is_EX_PRP_QP_label() const {
	return this->label() == Label_EX() || this->label() == Label_PRP() || this->label() == Label_QP();
}

/// True iff the label is DT or PRP$
bool ItemToken::is_DT_PRPP_label() const {
	return this->label() == Label_DT() || this->label() == Label_PRPP();
}

/// True iff the label is VBP or MD
bool ItemToken::is_VBP_MD_label() const {
	return this->label() == Label_VBP() || this->label() == Label_MD();
}

/// True iff the label is MD or TO
bool ItemToken::is_MD_TO_label() const {
	return this->label() == Label_MD() || this->label() == Label_TO();
}

/// True iff the label is TO or IN
bool ItemToken::is_TO_IN_label() const {
	return this->label() == Label_TO() || this->label() == Label_IN();
}

/// True iff the label is WDT or WP
bool ItemToken::is_WDT_WP_label() const {
	return this->label() == Label_WDT() || this->label() == Label_WP();
}

/// True iff the label is # or $
bool ItemToken::is_HASH_DOLLAR_label() const {
	return this->label() == Label_HASH() || this->label() == Label_DOLLAR();
}

/// True iff the label is S, SINV, SBARQ, or FRAG
bool ItemToken::is_S_SINV_SBARQ_FRAG_label() const {
	return this->label() == Label_S() || this->label() == Label_SINV() || this->label() == Label_SBARQ() || this->label() == Label_FRAG();
}

/// True iff the label is S, SINV, SBARQ, FRAG, SQ, or X
bool ItemToken::is_S_SINV_SBARQ_FRAG_SQ_X_label() const {
	return this->label() == Label_S() || this->label() == Label_SINV() || this->label() == Label_SBARQ() || this->label() == Label_FRAG() || this->label() == Label_SQ() || this->label() == Label_X();
}

/// True iff the label is S, SINV, SBARQ, FRAG, SQ, X, or INTJ
bool ItemToken::is_S_SINV_SBARQ_FRAG_SQ_X_INTJ_label() const {
	return this->label() == Label_S() || this->label() == Label_SINV() || this->label() == Label_SBARQ() || this->label() == Label_FRAG() || this->label() == Label_SQ() || this->label() == Label_X() || this->label() == Label_INTJ();
}

/// True iff the label is , or :
bool ItemToken::is_COMMA_COLON_label() const {
	return this->label() == Label_COMMA() || this->label() == Label_COLON();
}

/// True iff the headtag is VB, VBN, VBG, VBZ, VBD, or VBP
bool ItemToken::is_V_headtag() const {
	return this->headtag() == Label_VB() || this->headtag() == Label_VBN() || this->headtag() == Label_VBG() || this->headtag() == Label_VBZ() || this->headtag() == Label_VBD() || this->headtag() == Label_VBP();
}

/// True iff the headtag is NN or NNS
bool ItemToken::is_NN_headtag() const {
	return this->headtag() == Label_NN() || this->headtag() == Label_NNS();
}

/// True iff the headtag is JJ or JJS
bool ItemToken::is_JJ_JJS_headtag() const {
	return this->headtag() == Label_JJ() || this->headtag() == Label_JJS();
}

/// True iff the headtag is JJ or JJR
bool ItemToken::is_JJ_JJR_headtag() const {
	return this->headtag() == Label_JJ() || this->headtag() == Label_JJR();
}

/// True iff the headtag is RB or RP
bool ItemToken::is_RB_RP_headtag() const {
	return this->headtag() == Label_RB() || this->headtag() == Label_RP();
}

/// True iff the headtag is RBR or RBS
bool ItemToken::is_RBR_RBS_headtag() const {
	return this->headtag() == Label_RBR() || this->headtag() == Label_RBS();
}

/// True iff the headtag is EX or PRP.
bool ItemToken::is_EX_PRP_headtag() const {
	return this->headtag() == Label_EX() || this->headtag() == Label_PRP() || this->headtag() == Label_QP();
}

/// True iff the headtag is DT or PRP$
bool ItemToken::is_DT_PRPP_headtag() const {
	return this->headtag() == Label_DT() || this->headtag() == Label_PRPP();
}

/// True iff the headtag is VBP or MD
bool ItemToken::is_VBP_MD_headtag() const {
	return this->headtag() == Label_VBP() || this->headtag() == Label_MD();
}

/// True iff the headtag is MD or TO
bool ItemToken::is_MD_TO_headtag() const {
	return this->headtag() == Label_MD() || this->headtag() == Label_TO();
}

/// True iff the headtag is TO or IN
bool ItemToken::is_TO_IN_headtag() const {
	return this->headtag() == Label_TO() || this->headtag() == Label_IN();
}

/// True iff the headtag is WDT or WP
bool ItemToken::is_WDT_WP_headtag() const {
	return this->headtag() == Label_WDT() || this->headtag() == Label_WP();
}

/// True iff the headtag is # or $
bool ItemToken::is_HASH_DOLLAR_headtag() const {
	return this->headtag() == Label_HASH() || this->headtag() == Label_DOLLAR();
}

/// True iff the headtag is , or :
bool ItemToken::is_COMMA_COLON_headtag() const {
	return this->headtag() == Label_COMMA() || this->headtag() == Label_COLON();
}
