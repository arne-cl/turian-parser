/*  $Id: ItemToken.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ItemToken.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class ItemToken
 *  \brief An Item instance in a parse Chart.
 *
 *  ItemToken extends Item by also storing a list of children Item%s (as ItemToken%s).
 *  The children Item%s are those Item%s in the Chart that are
 *  immediately dominated by this ItemToken.
 *
 *  \todo Do we want terminal items to have their tag as the item label,
 *  or NO_LABEL as the item label?
 *  \todo REMOVEME, and move all functionality into Item.H?
 *
 */

#ifndef __ItemToken_H__
#define  __ItemToken_H__

#include "parse/Item.H"
#include "parse/Span.H"
#include "parse/Sentence.H"

#include "universal/vocabulary.H"
#include "universal/posclass.H"
#include "universal/ID.H"
#include "universal/Archive.H"

#include "universal/Debug.H" // REMOVEME ************************************************

#include <cassert>
#include <iostream>		// REMOVEME
#include <sstream>
#include <vector>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class Action;
class ItemToken;

/// A vector of ItemToken*.
class ItemTokens : public vector<const ItemToken*> {
public:
	string to_string(bool longform=false) const;

	/// The number of terminals dominated by these ItemTokens.
	unsigned terminal_cnt(void) const;

	/// Locate the start and end indices of an Action's items in this span.
	/// \note If the action is of length 1, then the start index == end index.
	const pair<unsigned, unsigned> indices_of(const Action& a) const;

	const ID<Sentence>& sentence() const;

	Span span() const { return Span(*this); }

private:

	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<vector<const ItemToken*> >(*this);
	}
};

class ItemToken : public Item {
public:
	ItemToken();

	/// Terminal ItemToken constructor
	ItemToken(const Label l, const Word w, const unsigned location, const ID<Sentence>& sentence);

	/// Constituent ItemToken constructor
	ItemToken(const Action& a);

	const ItemTokens& children() const
	{ assert(!this->is_terminal()); assert(m_children.size() > 0); return m_children; }
//	{ assert(!m_is_terminal); assert(m_children.size() > 0); return m_children; }

	unsigned head() const {
		assert(!this->is_terminal());
		assert(m_head != NO_VALUE);
		return m_head;
	}

	const Span& span() const { return m_span; }

	/// Find the terminal Item that heads this ItemToken.
	const ItemToken& head_terminal() const;

	/// \todo Slow to return Action?
	const Action action() const;

	const ID<Sentence>& sentence() const {
		assert(!m_sentence.empty());
		return m_sentence;
	}

	/// The number of terminals dominated by this ItemToken.
	unsigned terminal_cnt(void) const;

	string to_string(bool longform=false) const;
	string to_string_with_children(bool longform=false) const;

	string to_debug_string(const string s="") const;
	
	/// Retrieve all terminals dominated by this item.
	/// \param terminals All terminals dominated by this item will be appended to this list.
	void get_terminals(ItemTokens& terminals) const;

	/// Is this node a verbal node (headtag check)?
	bool is_verbal_headtag() const;

	/// Is this node a verbal node (label check)?
	bool is_verbal_label() const;
	
	/// Is this node a verbal node or does it dominate a verbal node?
	bool dominates_some_verb() const;

	/// Does this item have exactly one child?
	/// (UNARY-INTERNAL in Klein+Manning)
	bool only_one_child() const;

	/// Is this a Basal NP?
	/// True iff this is an NP that dominates no non-possessive NP.
	/// (from Bikel)
	bool is_basal_NP() const;

/*
	/// Are all the children of this node terminals?
	bool are_all_children_terminals() const;

	/// Is this a right-recursive NP?
	/// An NP is right-recursive iff the rightmost child is an NP.
	/// (from Klein+Manning)
	bool is_right_recursive_NP() const;

	/// Is this a possessive NP? (Klein+Manning rules)
	/// According to Klein+Manning, an NP is a possessive NP iff the leftmost child is an NP.
	/// In others, is_left_recursive_NP().
	bool is_possessive_NP_Klein_and_Manning_test() const;

	/// Is this a possessive NP? (MacIntyre rules)
	/// True iff this is an NP with a rightmost POS.
	/// NB Collins + Bikel allow any child to be a POS.
	/// tgrep2 indicates that an NP with a non-rightmost POS tag is typically *not* a possessive NP.
	/// (Technically, MacIntyre insists that the POS dominate only 's or ' or 'S, but we ignore this additional requirement.)
	/// (from ftp://ftp.cis.upenn.edu/pub/treebank/doc/faq.cd2)
	bool is_possessive_NP_MacIntyre_test() const;

	*/

	/// Is this a gapped S?
	/// True iff the head is a VP which is the leftmost child.
	/// Our SG test is simpler than that of Collins + Bikel (and stricter), since we don't allow pre-head arguments.
	/// (based upon Bikel)
	bool is_gapped_S() const;

	/// Is this a coordinated phrase?
	/// True iff:
	/// There is a non-head child that is a coordinating conjunction
	///	and
	/// It is post-head but non-final, or it is immediately pre-head but non-initial.
	/// (from Bikel)
	bool is_coordinated_phrase() const;

	/// Is this a content word?
	/// i.e. Is the tag of this word a content tag?
	/// \todo FIXME: Move the content tag list to globals.H
	bool is_content_word() const;

	/// Is this a noun phrase?
	/// True iff the label is NP or NPB
	bool is_noun_phrase() const;

	/// Is this a noun?
	/// True iff the headtagclass is '::N' or '::NP'.
	bool is_noun() const;

	/// True iff the label is WHADJP, WHADVP, WHNP, or WHPP
	bool is_WH_label() const;

	/// True iff the label is VB, VBN, VBG, VBZ, VBD, or VBP
	bool is_V_label() const;

	/// True iff the label is NN or NNS
	bool is_NN_label() const;

	/// True iff the label is JJ or JJS
	bool is_JJ_JJS_label() const;

	/// True iff the label is JJ or JJR
	bool is_JJ_JJR_label() const;

	/// True iff the label is RB or RP
	bool is_RB_RP_label() const;

	/// True iff the label is RBR or RBS
	bool is_RBR_RBS_label() const;

	/// True iff the label is PP or ADVP
	bool is_PP_ADVP_label() const;

	/// True iff the label is PP or SBAR 
	bool is_PP_SBAR_label() const;

	/// True iff the label is SBAR or ADJP
	bool is_SBAR_ADJP_label() const;

	/// True iff the label is SBAR, ADJP, or UCP
	bool is_SBAR_ADJP_UCP_label() const;

	/// True iff the label is EX, PRP, or QP
	bool is_EX_PRP_QP_label() const;

	/// True iff the label is DT or PRP$
	bool is_DT_PRPP_label() const;

	/// True iff the label is VBP or MD
	bool is_VBP_MD_label() const;

	/// True iff the label is MD or TO
	bool is_MD_TO_label() const;

	/// True iff the label is TO or IN
	bool is_TO_IN_label() const;

	/// True iff the label is WDT or WP
	bool is_WDT_WP_label() const;

	/// True iff the label is # or $
	bool is_HASH_DOLLAR_label() const;

	/// True iff the label is S, SINV, SBARQ, or FRAG
	bool is_S_SINV_SBARQ_FRAG_label() const;

	/// True iff the label is S, SINV, SBARQ, FRAG, SQ, or X
	bool is_S_SINV_SBARQ_FRAG_SQ_X_label() const;

	/// True iff the label is S, SINV, SBARQ, FRAG, SQ, X, or INTJ
	bool is_S_SINV_SBARQ_FRAG_SQ_X_INTJ_label() const;

	/// True iff the label is , or :
	bool is_COMMA_COLON_label() const;

	/// True iff the headtag is VB, VBN, VBG, VBZ, VBD, or VBP
	bool is_V_headtag() const;

	/// True iff the headtag is NN or NNS
	bool is_NN_headtag() const;

	/// True iff the headtag is JJ or JJS
	bool is_JJ_JJS_headtag() const;

	/// True iff the headtag is JJ or JJR
	bool is_JJ_JJR_headtag() const;

	/// True iff the headtag is RB or RP
	bool is_RB_RP_headtag() const;

	/// True iff the headtag is RBR or RBS
	bool is_RBR_RBS_headtag() const;

	/// True iff the headtag is EX, PRP, or QP
	bool is_EX_PRP_headtag() const;

	/// True iff the headtag is DT or PRP$
	bool is_DT_PRPP_headtag() const;

	/// True iff the headtag is VBP or MD
	bool is_VBP_MD_headtag() const;

	/// True iff the headtag is MD or TO
	bool is_MD_TO_headtag() const;

	/// True iff the headtag is TO or IN
	bool is_TO_IN_headtag() const;

	/// True iff the headtag is WDT or WP
	bool is_WDT_WP_headtag() const;

	/// True iff the headtag is # or $
	bool is_HASH_DOLLAR_headtag() const;

	/// True iff the headtag is , or :
	bool is_COMMA_COLON_headtag() const;

private:
	/// Update head information.
	/// Using this item's children, find the head-child and get head
	/// information for it.
	void update_head();

	/// This span of this item.
	/// For a terminal item, this is a span of width 1.
	/// For a constituent item, this is the aggregate span of ::m_children.
	Span m_span;

	/// Children of this item
	ItemTokens m_children;

	ID<Sentence> m_sentence;

	/// The index of the head in the span.
	/// \todo Don't store this?
	unsigned m_head;

	unsigned m_terminal_cnt;
	bool m_is_verbal_headtag;
	bool m_is_verbal_label;
	bool m_dominates_some_verb;
	bool m_only_one_child;
	bool m_is_basal_NP;
	bool m_is_coordinated_phrase;
	bool m_is_gapped_S;
	/*
	bool m_are_all_children_terminals;
	bool m_is_right_recursive_NP;
	bool m_is_possessive_NP_Klein_and_Manning_test;
	bool m_is_possessive_NP_MacIntyre_test;
	*/

	void set_predicates();
	unsigned _terminal_cnt() const;
	bool _is_verbal_headtag() const;
	bool _is_verbal_label() const;
	bool _dominates_some_verb() const;
	bool _only_one_child() const;
	bool _is_basal_NP() const;
	bool _is_coordinated_phrase() const;
	bool _is_gapped_S() const;
	/*
	bool _are_all_children_terminals() const;
	bool _is_right_recursive_NP() const;
	bool _is_possessive_NP_Klein_and_Manning_test() const;
	*/
	bool _is_possessive_NP_MacIntyre_test() const;

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<Item>(*this);
		ar & m_span;
		ar & m_children;
		ar & m_sentence;
		ar & m_head;
		ar & m_terminal_cnt;
		ar & m_is_verbal_headtag;
		ar & m_is_verbal_label;
		ar & m_dominates_some_verb;
		ar & m_only_one_child;
		ar & m_is_basal_NP;
		ar & m_is_coordinated_phrase;
		ar & m_is_gapped_S;
		/*
		ar & m_are_all_children_terminals;
		ar & m_is_right_recursive_NP;
		ar & m_is_possessive_NP_Klein_and_Manning_test;
		ar & m_is_possessive_NP_MacIntyre_test;
		*/
	}
};

//BOOST_CLASS_TRACKING(ItemToken, boost::serialization::track_always)

#endif /* __ItemToken_H__ */
