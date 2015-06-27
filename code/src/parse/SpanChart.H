/*  $Id: SpanChart.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file SpanChart.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class SpanChart
 *  \brief A Chart of SpanItems for a single sentence.
 *
 *  A SpanChart maintains consistency conditions over SpanItems.
 *  In this way, it provides a SpanItem logic. [is this the correct term?]
 *  Specifically, when adding SpanItems to a SpanChart:
 *	- We ensure that the new SpanItem doesn't cross-brackets with
 *	some SpanItem already in the SpanChart.
 *	- We ensure that the new SpanItem is not dominated by some
 *	SpanItem already in the SpanChart. Hence, we impose a bottom-up
 *	ordering on the SpanItems.
 *
 *  \warning SpanChart assumes a bottom-up ordering, and cannot handle r2l/l2r derivation ordering.
 *
 *  \todo Also add a depth, s.t. we can order SpanItems with equal
 *  Spans? This would allow us to ensure that unary productions are
 *  strictly ordered.
 *  \todo Ensure that no two SpanItems in the chart are equivalent?
 *  i.e. that there are no unary projections to self.
 *  \todo Don't have both this class and plain Chart class.
 *
 */

#ifndef __SpanChart_H__
#define  __SpanChart_H__

#include "parse/SpanItem.H"

#include <list>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class State;

class SpanChart {
public:
	SpanChart() : uber_span(Span()) {}

	/// Construct an empty chart.
	/// \param span Span of this chart's sentence.
	/// A boundary for the SpanItems herein.
	SpanChart(const Span& span) : uber_span(span) {}

	/// Construct a chart from a State.
	/// We do this by replaying the State's parse Path, adding the
	/// Path's actions to the chart one-at-a-time to the chart.
	SpanChart(const State& state) { this->construct(state); }
	void construct(const State& state);

	/// Add a SpanItem to the chart.
	/// We maintain the following invariants:
	///	- We ensure that no identical SpanItem is already in the
	///	SpanChart.
	///	- We ensure that the new SpanItem doesn't cross-brackets
	///	with some SpanItem already in the SpanChart.
	///	- We ensure that the new SpanItem is not dominated by some
	///	SpanItem already in the SpanChart. Hence, we impose a bottom-up
	///	search strategy on the SpanItems.
	void add(const SpanItem& item);

	/// Subtract a SpanItem from the chart.
	/// \param item The item to be subtracted from the chart.
	/// This operation allows us to score a SpanItem \b against
	/// this SpanChart. In other words, if this SpanChart contains
	/// the gold-standard SpanItems, subtracting this SpanItem will tell
	/// us what the change in precision, recall, and crossing-brackets
	/// this SpanItem incurs.
	/// \attention This operation is not as simple as than the name suggests.
	/// Not only do we remove the SpanItem from the chart, we also
	/// remove all SpanItems in the chart that are dominated by the
	/// SpanItem, and that cross brackets with this SpanItem.
	/// Alternate intuition:
	/// We are adding a negative SpanItem to the chart.
	/// Hence, we remove from the chart all SpanItems that---if this
	/// SpanItem were present in the chart---could not subsequently be
	/// added to the chart.
	/// \param prc A precision count, which we increment iff item is
	/// in the chart.
	/// \param rcl A recall count, from which we subtract the number of
	/// items removed that had crossed-brackets with or had been
	/// dominated by item.
	/// \param cbs Crossing brackets count, to which we add the number of
	/// items removed that had crossed-brackets with item.
	/// \warning This computation is based upon bottom-up ordering, and cannot compute the upper-bound based upon an l2r/r2l derivation.
	void subtract(const SpanItem& item, unsigned& prc, unsigned& rcl, unsigned& cbs);

private:
	/// \todo If we were clever, we could write a generic exists method
	/// to which we pass a function pointer. However, we're not.
	bool exists_crosses(const SpanItem& item) const;
	bool exists_dominates(const SpanItem& item) const;
	bool exists_equal_to(const SpanItem& item) const;

	list<SpanItem> _items;

	/// Span of the entire sentence.
	/// A boundary for the SpanItems herein.
	Span uber_span;
};

#endif /* __SpanChart_H__ */
