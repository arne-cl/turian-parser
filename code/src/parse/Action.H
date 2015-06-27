/*  $Id: Action.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Action.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Action
 *  \brief A parse decision.
 *
 *  The yield of an Action is a parse ItemToken.
 *  Actions are defined by a parent Label (of the new parent item,
 *  to which we are projecting) and a set of children Item%s.
 *
 *  \todo REMOVEME, and just use a proposed new ItemToken instead of an Action.
 *  \todo Write more documentation.
 *  \todo Add method "can_confidence_rate" (i.e. the span is not too large to be classified)
 *  \todo When is Action used? Can we replace children with a pair of
 *  item indices (in the state, i.e. of the leftmost and rightmost items
 *  of the children span) + State* ?
 *
 */

#ifndef __Action_H__
#define  __Action_H__

#include "parse/ItemToken.H"

#include "universal/globals.H"
#include "universal/vocabulary.H"
#include "universal/LIST.H"

#include <string>
#include <ext/hash_map>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

class Action {
public:
	/// WRITEME
	Action() { this->clear(); }

	/// WRITEME
	Action(const Label l, const ItemTokens& c)
	{ assert(is_constituent_label(l)); m_label = l; m_children = c; }

	/// Initialize this object.
	void clear() { m_label = NO_LABEL; m_children.clear(); }

	bool operator== (const Action& a) const {
		if (m_children == a.m_children)
			assert(this->span() == a.span());
		return (m_label==a.m_label && m_children==a.m_children);
	}

	size_t hash() const {
		size_t h = m_label;
		ItemTokens::const_iterator c;
		for (c = m_children.begin(); c != m_children.end(); c++)
			h = h*PRIME + (size_t)(*c);
		return h;
	}

	Label label() const { return m_label; }

	const ItemTokens& children() const { return m_children; }

	const Span span() const { return Span(m_children); }

	string to_string(bool verbose=false) const {
		if (!verbose)
			return label_string(m_label) + " <- " + Span(m_children).to_string();
		else {
			ItemTokens::const_iterator i;
			string s;
			s += label_string(m_label) + " <- (";
			for (i = m_children.begin(); i != m_children.end(); ) {
				s += label_string((*i)->label());
				i++;
				if (i != m_children.end())
					s += ", ";
			}
			s += ")";
			return s;
		}
	}

private:
	Label m_label;
	ItemTokens m_children;
};

class Actions : public LIST<Action> {
};

#ifndef DOXYGEN
namespace __gnu_cxx {
	/// Template specialization of STL's hash object for Actions.
	template<> class hash<Action> {
		public: size_t operator() (const Action& a) const{ return a.hash(); }
	};
}
using namespace __gnu_cxx;
#endif /* DOXYGEN */

#endif /* __Action_H__ */
