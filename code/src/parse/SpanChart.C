/*  $Id: SpanChart.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file SpanChart.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/SpanChart.H"
#include "parse/State.H"

#include "universal/Debug.H"

/// Construct a chart from a State.
/// We do this by replaying the State's parse Path, adding the
/// Path's actions to the chart one-at-a-time to the chart.
void SpanChart::construct(const State& state) {
	uber_span = Span(state.frontier());
	_items.clear();
	Path p = state.path();
	assert(!p.empty());

	while (!p.empty()) {
		SpanItem i(p.pop());
		this->add(i);
	}
}

/// Add a SpanItem to the chart.
/// We maintain the following invariants:
///	- We ensure that no identical SpanItem is already in the
///	SpanChart.
///	- We ensure that the new SpanItem doesn't cross-brackets
///	with some SpanItem already in the SpanChart.
///	- We ensure that the new SpanItem is not dominated by some
///	SpanItem already in the SpanChart. Hence, we impose a bottom-up
///	search strategy on the SpanItems.
/// \bug We *should* assert there is no equal item in the treebank.
void SpanChart::add(const SpanItem& item) {
	assert(item.span() <= uber_span);
	assert(!this->exists_crosses(item));
	assert(!this->exists_dominates(item));

	// FIXME!!!!!!!!!! UNCOMMENT ME!!!!!!!!!!!!!!!!!!!!!!!!!!1
	//assert(!this->exists_equal_to(item));
	_items.push_back(item);
}

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
void SpanChart::subtract(const SpanItem& item, unsigned& prc, unsigned& rcl, unsigned& cbs) {
	assert(item.span() <= uber_span);
	list<SpanItem>::iterator it;
	for (it = _items.begin(); it != _items.end(); ) {
		const SpanItem& i = *it;
		if (i == item) {
			if (i.label() != Label_TOP())
				prc++;
		} else if (i.crosses(item)) {
			if (i.label() != Label_TOP()) {
				assert(rcl>0);
				rcl--;
				cbs++;
			}
		} else if (i < item) {
			if (i.label() != Label_TOP()) {
				assert(rcl>0);
				rcl--;
			}
		}

		if (i == item || i.crosses(item) || i < item) {
			list<SpanItem>::iterator it_rm = it++;
			_items.erase(it_rm);
		} else {
			it++;
		}
	}
}

bool SpanChart::exists_crosses(const SpanItem& item) const {
	assert(item.span() <= uber_span);
	list<SpanItem>::const_iterator i;
	for (i = _items.begin(); i != _items.end(); i++)
		if (i->crosses(item))
			return true;
	return false;
}

bool SpanChart::exists_dominates(const SpanItem& item) const {
	assert(item.span() <= uber_span);
	list<SpanItem>::const_iterator i;
	for (i = _items.begin(); i != _items.end(); i++)
		if ((*i) > item)
			return true;
	return false;
}

bool SpanChart::exists_equal_to(const SpanItem& item) const {
	assert(item.span() <= uber_span);
	list<SpanItem>::const_iterator i;
	for (i = _items.begin(); i != _items.end(); i++)
		if ((*i) == item)
			return true;
	return false;
}
