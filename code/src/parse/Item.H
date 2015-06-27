/*  $Id: Item.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Item.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Item
 *  \brief An abstract parse Item.
 *
 *  This Item is defined by whether it is a terminal Item or a constituent Item,
 *  and by its label, headword, headtag, and headlabel.
 *  It has no notion of location or children, and exists independently of a State.
 *  (To be precise, there is no head without children, so the Item must exist somewhere.)
 *
 *  \todo Do we want terminal items to have their tag as the item label,
 *  or NO_LABEL as the item label?
 *
 */

#ifndef __Item_H__
#define  __Item_H__

#include "universal/globals.H"
#include "universal/posclass.H"
#include "universal/vocabulary.H"
#include "universal/Archive.H"

#include <cassert>
#include <ostream>
#include <sstream>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class Item {
public:
	Item() {
		m_label = NO_LABEL;
		m_headword = NO_WORD;
		m_headlabel = NO_LABEL;
		m_headtag = NO_LABEL;
		m_headtagclass = NO_POSCLASS;
	}

	bool is_terminal() const {
#ifdef SANITY_CHECKS
		assert ((m_is_terminal && is_terminal_label(m_label)) ||
			(!m_is_terminal && is_constituent_label(m_label)));
#endif /* SANITY_CHECKS */
		return m_is_terminal;
	}

	Label label() const {
#ifdef SANITY_CHECKS
		assert(is_label(m_label));
#endif /* SANITY_CHECKS */
		return m_label;
	}
	Word headword() const {
#ifdef SANITY_CHECKS
		assert(is_word(m_headword));
#endif /* SANITY_CHECKS */
		return m_headword;
	}
	Label headlabel() const {
#ifdef SANITY_CHECKS
		assert(m_headlabel == NO_LABEL || is_constituent_label(m_headlabel));
#endif /* SANITY_CHECKS */
		return m_headlabel;
	}
	Label headtag() const {
#ifdef SANITY_CHECKS
		assert(is_terminal_label(m_headtag));
#endif /* SANITY_CHECKS */
		return m_headtag;
	}
	Posclass headtagclass() const {
#ifdef SANITY_CHECKS
		assert(is_posclass(m_headtagclass));
		assert(tag_to_posclass(m_headtag) == m_headtagclass);
#endif /* SANITY_CHECKS */
		return m_headtagclass;
	}

	/// WRITEME
	bool is_punctuation() const
	{ assert(m_is_terminal); return is_punctuation_terminal(m_label); }

	/// WRITEME
	bool is_top() const
	{ assert(!m_is_terminal); return m_label == Label_TOP(); }

	string to_string() const {
		ostringstream o;
		o << "label=" << label_string(m_label) << " ";
		o << "headword=" << word_string(m_headword) << " ";
		o << "headlabel=" << label_string(m_headlabel) << " ";
		o << "headtag=" << label_string(m_headtag) << " ";
		o << "headtagclass=" << posclass_string(m_headtagclass);
		return o.str();
	}

protected:
	/// Whether this item is a terminal item or not
	/// \invariant true iff is_terminal_label(m_label)
	bool m_is_terminal;

	/// Label of this item
	/// \todo Do we want terminal items to have their tag as the item label,
	/// or NO_LABEL as the item label?
	Label m_label;

	/// Head-word
	Word m_headword;

	/// Label of the head-child item
	/// This must be a constituent label.
	Label m_headlabel;

	/// Tag of the head-word
	/// This must be a terminal label.
	Label m_headtag;

	/// POS class of the head-word
	Posclass m_headtagclass;

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & m_is_terminal;
		ar & m_label;
		ar & m_headword;
		ar & m_headlabel;
		ar & m_headtag;
		ar & m_headtagclass;
	}
};

#endif /* __Item_H__ */
