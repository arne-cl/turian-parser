/*  $Id: Span.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Span.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/Span.H"
#include "parse/ItemToken.H"
#include "parse/State.H"

#include <ostream>
#include <sstream>
#include <cassert>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

/// Construct a span comprising a single terminal item.
/// \param i The number of the terminal item comprised by this span.
Span::Span(unsigned i) {
	p = make_pair(i, i);
	this->sanity_check();
}

/// Construct a span between two endpoints.
/// \param l The number of the first terminal item comprised by this span.
/// \param r The number of the last terminal item comprised by this span.
Span::Span(unsigned l, unsigned r) {
	p = make_pair(l, r);
	this->sanity_check();
}

/// Construct a span by aggregating continuous spans.
/// \param items The items to be aggregated into a span.
Span::Span(const ItemTokens& items) {
//	TRACE; Debug::log(1) << items << "\n";

	assert(!items.empty());
	ItemTokens::const_iterator i;
	for (i = items.begin(); i+1 != items.end(); i++)
		assert((*i)->span().right()+1 == (*(i+1))->span().left());
	p = make_pair(items.front()->span().left(), items.back()->span().right());
	this->sanity_check();
}

/// Ensure that this object is good.
void Span::sanity_check() const {
	assert(p.first != NO_VALUE);
	assert(p.second != NO_VALUE);
	assert(p.first <= p.second);
}

const string Span::to_string() const {
	this->sanity_check();
	ostringstream o;
	o << p.first << " .. " << p.second;
	return o.str();
}

/// Do these spans comprise the exact same item sequence?
bool Span::operator== (const Span& s) const {
	this->sanity_check();
	s.sanity_check();
	return (p.first == s.p.first && p.second == s.p.second);
}

/// Do these spans comprise at least one item in common?
bool Span::overlaps(const Span& s) const {
	this->sanity_check();
	s.sanity_check();
	if (p.second < s.p.first) return false;
	if (s.p.second < p.first) return false;
	return true;
}

/// Are we dominated by this other Span?
/// i.e. Is our item sequence a strict subset of this other
/// Span's item sequence?
/// i.e. Do we strictly precede another Span in a bottom-up
/// parser strategy?
bool Span::operator< (const Span& s) const {
	if (!this->overlaps(s)) return false;
	if (*this == s) return false;
	return p.first >= s.p.first && p.second <= s.p.second;
}

/// Do we dominate this other Span?
/// i.e. Is our item sequence a strict superset of this other
/// Span's item sequence?
/// i.e. Do we strictly follow another Span in a bottom-up
/// parser strategy?
bool Span::operator> (const Span& s) const {
	if (!this->overlaps(s)) return false;
	if (*this == s) return false;
	return p.first <= s.p.first && p.second >= s.p.second;
}

/// Do these Spans cross brackets?
/// i.e. Do these Spans overlap, with neither Span dominating
/// the other, and the Spans not being equal?
bool Span::crosses(const Span& s) const {
	return (this->overlaps(s) && !(*this > s) && !(*this < s) && !(*this == s));
}
