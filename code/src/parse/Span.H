/*  $Id: Span.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Span.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Span
 *  \brief A range in a State.
 *
 *  Specifically, a Span comprises a *sequence* of terminal Items in
 *  a State.
 *
 *  \note Span forces that constituents have continuous items. i.e. It
 *  imposes a geometric constraints on valid actions.
 *
 *  \todo Include a State* as a private member variable, so that we
 *  can ensure that we never compare Spans from different
 *  States. [Actually, this won't work, since States can change. However,
 *  a Parser* would suffice.]
 *  \todo Describe that numbering scheme is over terminal items
 *  (starting at 0?)
 *  \todo Describe start and end *inclusive*.
 *
 */

#ifndef __Span_H__
#define  __Span_H__

#include "universal/Archive.H"
#include "universal/globals.H"

#include <string>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class ItemTokens;
class State;

class Span {
public:
	Span() : p(make_pair(NO_VALUE, NO_VALUE)) {}

	Span(const Span& s) : p(s.p) { this->sanity_check(); }

	/// Construct a span comprising a single terminal item.
	/// \param i The number of the terminal item comprised by this span.
	Span(unsigned i);

	/// Construct a span between two endpoints.
	/// \param l The number of the first terminal item comprised by this span.
	/// \param r The number of the last terminal item comprised by this span.
	Span(unsigned l, unsigned r);

	/// Construct a span by aggregating continuous spans.
	/// \param items The items to be aggregated into a span.
	Span(const ItemTokens& items);

	unsigned left() const { this->sanity_check(); return p.first; }
	unsigned right() const { this->sanity_check(); return p.second; }

	unsigned width() const { this->sanity_check(); return p.second - p.first + 1; }
	unsigned length() const { return this->width(); }

	unsigned start() const { return this->left(); }
	unsigned end() const { return this->right(); }

	unsigned first() const { return this->left(); }
	unsigned last() const { return this->right(); }

	const string to_string() const;

	/// Do these spans comprise the exact same item sequence?
	bool operator== (const Span& s) const;

	/// Do these spans comprise at least one item in common?
	bool overlaps(const Span& s) const;

	/// Are we dominated by this other span?
	/// i.e. Is our item sequence a strict subset of this other
	/// span's item sequence?
	/// i.e. Do we strictly precede another span in a bottom-up
	/// parser strategy?
	bool operator< (const Span& s) const;

	/// Do we dominate this other span?
	/// i.e. Is our item sequence a strict superset of this other
	/// span's item sequence?
	/// i.e. Do we strictly follow another span in a bottom-up
	/// parser strategy?
	bool operator> (const Span& s) const;

	/// Do these spans cross brackets?
	/// i.e. Do these spans overlap, with neither span dominating
	/// the other, and the spans not being equal?
	bool crosses(const Span& s) const;

	bool operator<= (const Span& s) const { return *this < s || *this == s; }
	bool operator>= (const Span& s) const { return *this > s || *this == s; }

private:
	/// Ensure that this object is good.
	void sanity_check() const;

	pair<unsigned, unsigned> p;


	friend class boost::serialization::access;
	template <class Archive> void serialize(Archive & ar, const unsigned int version) { ar & p; }
};

#endif /* __Span_H__ */
