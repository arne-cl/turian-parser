/*  $Id: SearchQueue.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file SearchQueue.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/SearchQueue.H"

#include "universal/Double.H"
#include "universal/stats.H"

SearchQueue::SearchQueue() :
	_q(parameter::beam_search_width()),
	_best(),
	_best_cost_known(),
	_addcnt(0),
	_popcnt(0),
	_skipcnt(0)
{
	assert(_best.empty());
}

/// Is searching done?
/// i.e. Is the queue empty or have we exceed the maximum number of states to visit?
bool SearchQueue::done() const {
	if (_q.empty()) {
//		// Make sure that best's Cost was initialized
//		assert(_best.cost() != Cost());
		assert(!_best.empty());
		return true;
	} else if (this->popcnt() >= parameter::max_search_states_to_pop() && !_best.empty()) {
		return true;
	//////////////////////////////////////////////////////////
	/// \todo ADD a PARAMETER ABOUT ADDCNT, REMOVE POPCNT PARAMETER!!!
	} else if (_addcnt >= parameter::max_search_states_to_pop() && !_best.empty()) {
		return true;
	} else {
		return false;
	}
}

/// Can we explore a StateCost?
/// We cannot if we've already explored the State with a no-worse Cost,
/// or if the State cannot beat the current best final State.
bool SearchQueue::can_explore(const StateCost& s) const {
	if (s.empty()) return false;

	const ParseState& state = s.state();
	const Cost& cost= s.cost();

	/// Have we've already explored the State with no better Cost,
	if (_best_cost_known.find(state) != _best_cost_known.end() &&
			cost <= _best_cost_known.find(state)->second) {
		return false;
	}

	// Can this State not beat the current best final State?
	if (!_best.empty() && s.always_more_costly_than(_best))
		return false;

	return true;
}

/// Pop a non-final StateCost to explore.
/// \return The highest priority (lowest cost) StateCost on the queue.
/// Empty if no such StateCost exists, in which case SearchQueue is now done.
/// \todo Don't duplicate code with SearchQueue::remove
StateCost SearchQueue::pop() {
	while(!this->done()) {
		const StateCost s = _q.top();
		_q.pop();
		_popcnt++;

		if (!this->can_explore(s)) {
			_skipcnt++;
			continue;
		}

		// Otherwise, keep s if it's not necessarily worst than _best
		if (_best.empty() || !s.always_more_costly_than(_best)) {
			assert(!s.is_final());
			return s;
		}
	}
	const StateCost tmp;
	assert(tmp.empty());
	return tmp;
}

/// Remove a ParseState with some associated Cost from the queue.
/// i.e. don't duplicate work by allowing this State to be popped with
/// no better Cost.
void SearchQueue::remove(const ParseState& state, const Cost& cost) {
	// If this state has not been seen before...
	if (_best_cost_known.find(state) == _best_cost_known.end())
		// ...then set the best cost seen for this state
		_best_cost_known[state] = cost;

	// Otherwise, if the current state cost makes the cost better...
	else if (cost > _best_cost_known[state])
		// ...then update the cost
		_best_cost_known[state] = cost;
}

/// Is there space for some Cost on the queue, or will it be ignored?
bool SearchQueue::can_add(const Cost& c) const {
	return _q.empty() || !(_q.bottom() > c && _q.full());
}

/// Add a new ParseState with its associated Cost to the queue.
void SearchQueue::add(const ParseState& state, const Cost& cost) {
	_addcnt++;

	// If this is a final state...
	if (state.is_final()) {
		StateCost news(state, cost);

		// And the new state is better than the current best state
		// FIXME: Don't switch >, < convention?
		if (_best.empty() || news > _best) {
			_best = news;
			assert(_best.is_final());
			Debug::log(3) << "Current best final cost <- " << _best.cost().to_string() << " (popped " << _popcnt << ", skipped " << _skipcnt << ")\n";
			Debug::log(3) << this->stats();
			Debug::log(3) << stats::resource_usage() << "\n";
		}
	} else {
		// If we don't have room for this state, then just return.
		if (!this->can_add(cost)) return;

		StateCost news(state, cost);

		if (!this->can_explore(news)) return;
		_q.push(news);
	}
}

/// \todo Keep track of more stats here.
string SearchQueue::stats() const {
	ostringstream o;
	o << "SearchQueue explored " << _best_cost_known.size() << " states, popped " << _popcnt << " states (" << _skipcnt << " skipped, " << _addcnt << " attempted added) from the " << parameter::beam_search_width() << "-best list\n";
//	o << _q.stats();
	
	return o.str();
}
