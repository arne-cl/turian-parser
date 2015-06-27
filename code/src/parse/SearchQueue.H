/*  $Id: SearchQueue.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file SearchQueue.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class SearchQueue
 *  \brief A queue used during parsing with search.
 *
 *  \todo More documentation.
 *  \todo Move this and search related business into a search library
 *
 */

#ifndef __SearchQueue_H__
#define  __SearchQueue_H__

#include "parse/ParseState.H"
#include "parse/StateCost.H"

#include "learn/Cost.H"

#include "universal/NBest.H"

class SearchQueue {
public:
	SearchQueue();

	/// Is searching done?
	/// i.e. Is the queue empty or have we exceed the maximum number of states to visit?
	bool done() const;

	/// Can we explore a StateCost?
	/// We cannot if we've already explored the State with a no-worse Cost,
	/// or if the State cannot beat the current best final State.
	bool can_explore(const StateCost& s) const;

	/// Pop a non-final StateCost to explore.
	/// \return The highest priority (lowest cost) StateCost on the queue.
	/// Empty if no such StateCost exists, in which case SearchQueue is now done.
	/// \todo Don't duplicate code with SearchQueue::remove
	StateCost pop();

	/// Remove a ParseState with some associated Cost from the queue.
	/// i.e. don't duplicate work by allowing this State to be popped with
	/// no better Cost.
	void remove(const ParseState& state, const Cost& cost);

	/// Is there space for some Cost on the queue, or will it be ignored?
	bool can_add(const Cost& c) const;

	/// Add a new ParseState with its associated Cost to the queue.
	void add(const ParseState& state, const Cost& cost);

	const StateCost& best() {
		assert(_best.empty() || _best.is_final());
		return _best;
	}
	unsigned popcnt() const { return _popcnt; }
	unsigned skipcnt() const { return _skipcnt; }

	/// \todo Keep track of more stats here.
	string stats() const;

private:

	/// The priority-queue (beam) of remaining StateCost%s to explore.
	/// These are all *non-final*.
	NBest<StateCost> _q;

	/// The best *final* StateCost observed thus far.
	StateCost _best;

	/// For each visited ParseState, the best Cost observed for that
	/// ParseState so far.
	/// Used to avoid duplicating work.
	hash_map<ParseState, Cost> _best_cost_known;

	/// The number of states added to the queue.
	unsigned _addcnt;

	/// The number of states popped from the queue.
	unsigned _popcnt;

	/// The number of *duplicate* states popped from the queue.
	unsigned _skipcnt;
};

#endif /* __SearchQueue_H__ */
