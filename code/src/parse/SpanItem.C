/*  $Id: SpanItem.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file SpanItem.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/SpanItem.H"
#include "parse/Action.H"
#include "parse/Production.H"

#include <cassert>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

SpanItem::SpanItem(const Action& a) {
	_label = a.label();
	_span = a.span();
	this->sanity_check();
}

SpanItem::SpanItem(const Production& p) {
	_label = p.action().label();
	_span = p.action().span();
	this->sanity_check();
}

void SpanItem::sanity_check() {
	assert(_label != NO_LABEL);
	if (is_terminal_label(_label)) {
		assert(_span.length() == 1);
	} else {
		assert(_span.length() >= 1);
	}
}

/// Are these the same exact SpanItems.
bool SpanItem::operator== (const SpanItem& s) const {
	return _span == s._span && _label == s._label;
}

/// Do these SpanItems comprise at least one item in common?
bool SpanItem::overlaps(const SpanItem& s) const {
	return _span.overlaps(s._span);
}

/// Are we dominated by this other SpanItem?
/// i.e. Is our item sequence a strict subset of this other
/// SpanItem's item sequence?
/// i.e. Do we strictly precede another SpanItem in a bottom-up
/// parser strategy?
bool SpanItem::operator< (const SpanItem& s) const {
	return _span < s._span;
}

/// Do we dominate this other SpanItem?
/// i.e. Is our item sequence a strict superset of this other
/// SpanItem's item sequence?
/// i.e. Do we strictly follow another SpanItem in a bottom-up
/// parser strategy?
bool SpanItem::operator> (const SpanItem& s) const {
	return _span > s._span;
}

/// Do these SpanItems cross brackets?
/// i.e. Do these SpanItems overlap, with neither SpanItem dominating
/// the other?
bool SpanItem::crosses(const SpanItem& s) const {
	return _span.crosses(s._span);
}
