/*  $Id: Production.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Production.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Production
 *  \brief An instantiated Action.
 *
 *  A Production is an Action, and the ItemToken yielded by this Action.
 *
 *  \todo REMOVEME?
 *
 */

#ifndef __Production_H__
#define  __Production_H__

#include "parse/Action.H"
#include "parse/ItemToken.H"

#include "universal/vocabulary.H"

#include <ostream>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class Production {
public:
	/// Construct a Production.
	/// \param i The resultant ItemToken produced by the Action.
	/// \param action The Action that yields this Production.
	Production(const ItemToken* i, const Action& action) {
		assert(i);
		m_itemtoken = i;
		m_action = action;
		assert(m_itemtoken->span() == m_action.span());
	}

	/// Output productions in a form that can be
	/// read by Treebank::lex_production
	const string to_string() const {
		return "(" + label_string(m_itemtoken->label()) + " <- "
			+ m_itemtoken->span().to_string() + ")";
	}

	const ItemToken* itemtoken() const { return m_itemtoken; }
	const Action& action() const { return m_action; }

private:
	const ItemToken* m_itemtoken;
	Action m_action;
};

#endif /* __Production_H__ */
