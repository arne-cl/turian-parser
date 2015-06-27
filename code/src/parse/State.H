/*  $Id: State.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file State.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class State
 *  \brief A parse state.
 *
 *  A State is defined by the ItemTokens on its frontier, and the Path
 *  used to reach this State. (The frontier comprises all Item%s in the state
 *  that are not dominated by any other item.)
 *  Since the parser operates in a bottom-up fashion, an initial state
 *  will have all the terminal items on its frontier, and a final state
 *  will have only a single TOP item on its frontier.
 *
 *  \todo Move some of these methods to ParseState?
 *
 */

#ifndef __State_H__
#define  __State_H__

#include "parse/ItemToken.H"
#include "parse/Path.H"

#include <vector>
#include <ostream>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class Action;
class Parser;
class Production;
class Span;

class State {
public:
	/// Create an empty state.
	State() { this->clear(); }

	/// Create a state with a particular frontier.
	//State(const ItemTokens& frontier) { set(frontier); }

	/// Create a state with a particular frontier and Path.
	/// \todo Assert that the path can be replayed to obtain
	/// the state given by items.
	//State(const ItemTokens& frontier, const Path& path) { set(frontier); _path = path; }

	/// Restart at the initial state.
	/// Basically, clear the path and set the state items to be all its
	/// terminals.
	void restart();

	/// Is this a final state?
	/// \return True iff the frontier contains a TOP item, and no non-TOP non-punctuation item.
	bool is_final() const;

	/// Retrieve the frontier items in this state.
	const ItemTokens& frontier() const { return _frontier; }

	/// Perform an action.
	/// Given an action, update the state by replacing the children
	/// items in the action by the yielded item.
	/// \todo Where is this called from?
	void perform(const Production& p);

	const string to_string() const;

	const Path& path() const { return _path; }

	const Span span() const;

	/// Locate a span in the state.
	/// \param s The span we are locating.
	/// \return Those ItemTokens that the span comprises.
	const ItemTokens locate_span(const Span& s) const;

	/// Locate the left and right iterators of a span in the state.
	/// The left iterator points to the leftmost *inside* the span.
	/// The right iterator points to the rightmost *inside* the span.
	/// WRITEME: Including or excluding the rightmost item? I think it's *excluding*.
	/// \param s The span we are locating.
	/// \return A pair of ItemToken iterators, left and right.
	pair<ItemTokens::const_iterator, ItemTokens::const_iterator> span_iterators(const Span& s) const;

	/// \todo Make this *private*!
	/// \todo Add more safety checks.
	void add_item_to_frontier(const ItemToken* i);

	/// \todo Make this *private*!
	/// \todo Add more safety checks.
	void set_path(const Path& path) { _path = path;	}

	const ID<Sentence>& sentence() const { return _frontier.sentence(); }

	const State& initial_state() const;
	boost::shared_ptr<const State> initial_state_ptr() const { return _initial_state; }

	/// Find the terminal ItemToken with some span location.
	const ItemToken& locate_terminal(const Span& span) const;

protected:
	/// Initialize this object.
	void clear() { _frontier.clear(); _path.clear(); _initial_state.reset(); }

	/// Determine if this is an initial State.
	bool is_initial() const;

	/// Create the initial State from the current one.
	void create_initial_state();

	/// Set the frontier to a particular set of items.
	//void set(const ItemTokens& frontier) { _frontier.reserve(frontier.size()); _frontier = frontier; }

	/// Locate an (ordered) list of ItemToken%s in the frontier.
	ItemTokens::const_iterator locate(const ItemTokens& id) const;

	ItemTokens terminals_slow() const;

	Path _path;
	ItemTokens _frontier;

	// A pointer to the initial version of this State.
	// Empty iff this is an initial State.
	boost::shared_ptr<const State> _initial_state;
};

#endif /* __State_H__ */
