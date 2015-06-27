/*  $Id: SpanItem.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file SpanItem.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class SpanItem
 *  \brief A parse Item in a SpanChart.
 *
 *  A SpanItem is given by a (Label, Span) pair.
 *  Unlike an ItemToken, a SpanItem has no notion of children items.
 *  i.e. it exist in isolation from a particular State,
 *  and has no notion of which Item%s it immediately dominates.
 *
 *  \todo Also add a depth, s.t. we can order SpanItems with equal
 *  Spans? (i.e. strict unary projections)
 *  \todo Include a State* as a private member variable, so that we
 *  can ensure that we never compare Spans from different
 *  States. [Actually, this won't work, since States can change. However,
 *  a Parser* would suffice.]
 *  \todo Don't have both this class and plain ItemToken.
 *  \todo This + Action inherit from some supertype?
 *
 */

#ifndef __SpanItem_H__
#define  __SpanItem_H__

#include "parse/Span.H"

#include "universal/vocabulary.H"
#include "universal/posclass.H"

class Action;
class Production;

class SpanItem {
public:
	SpanItem(Label label, Span span) : _label(label), _span(span) {
		this->sanity_check();
	}

	SpanItem(const Action& a);
	SpanItem(const Production& p);

	Label label() const { return _label; }
	const Span& span() const { return _span; }

	/// Are these the same exact SpanItems.
	bool operator== (const SpanItem& s) const;

	/// Do these SpanItems comprise at least one item in common?
	bool overlaps(const SpanItem& s) const;

	/// Are we dominated by this other SpanItem?
	/// i.e. Is our item sequence a strict subset of this other
	/// SpanItem's item sequence?
	/// i.e. Do we strictly precede another SpanItem in a bottom-up
	/// parser strategy?
	bool operator< (const SpanItem& s) const;

	/// Do we dominate this other SpanItem?
	/// i.e. Is our item sequence a strict superset of this other
	/// SpanItem's item sequence?
	/// i.e. Do we strictly follow another SpanItem in a bottom-up
	/// parser strategy?
	bool operator> (const SpanItem& s) const;

	/// Do these SpanItems cross brackets?
	/// i.e. Do these SpanItems overlap, with neither SpanItem dominating
	/// the other?
	bool crosses(const SpanItem& s) const;

private:
	void sanity_check();

	Label _label;
	Span _span;

	/// \todo Also add a depth, s.t. we can order SpanItems with equal
	/// Spans? (i.e. strict unary projections)
	// double _depth;
};

#endif /* __SpanItem_H__ */
