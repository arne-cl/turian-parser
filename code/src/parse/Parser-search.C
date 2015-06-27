/*  $Id: Parser-search.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Parser-search.C
 *  \brief Implementations of search functions for Parser.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/Parser.H"
#include "parse/SearchQueue.H"

#include "universal/stats.H"

/// Parse this sentence, using search.
/// \param only_consistent Only allow consistent decisions.
/// (Default: false)
/// \todo Write code to allow us to skip sentences that are too long.
///	* Make the max sentence length a global.
///	* Do we want to compute sentence size as the number of terminals
///	or as the number of (non-punctuation* terminals?
/// \todo WRITEME: More debug output
void Parser::parse_search(bool only_consistent) {
	this->restart();

	SearchQueue q;
	ParseState initial = _gold_state;
	initial.restart();
	q.add(initial, Cost());

	this->parse_search(q, only_consistent);
	assert(q.done());
	this->replay(q.best(), only_consistent);
}


/// \todo RENAMEME! And add some documentation, you pill!
void Parser::parse_search_greedy_initial(bool only_consistent) {
	this->restart();

	/// WHY THE FUCK IS THIS STUPID BRITTLE LOGIC NEEDED?
	/// _chart*'s fuck us over again!
	_gold_state = _parse_state;

	SearchQueue q;
	ParseState initial = _parse_state;
	initial.restart();
	q.add(initial, Cost());

	while(!q.done()) {
		const StateCost s = q.pop();
		if (s.empty()) break;
//		Debug::log(3) << "Popped " << s.state().to_string() << " with cost " << s.cost().to_string() << "\n";
		this->parse_search_greedy(s, q, only_consistent);
	}
	Debug::log(3) << q.stats();

//	this->parse_search(q, only_consistent);

	assert(q.done());
	assert(!q.best().empty());

	this->replay(q.best(), only_consistent);
}

/// \todo RENAMEME! And add some documentation, you pill!
void Parser::parse_search_greedy(const StateCost& sc, SearchQueue& q, bool only_consistent) {
	StateCost s = sc;
	while(!s.state().is_final()) {
		assert(!s.empty());
		assert(!s.cost().is_final());

		s = this->explore(s, q, only_consistent);

		if (s.empty()) {
			assert(!q.best().empty());
//			Debug::log(3) << "Greedy search terminated early...\n";
			return;
		}
	}
	assert(s.cost().is_final());

//	Debug::log(3) << "Greedy search done...\n";
//	Debug::log(3) << stats::resource_usage() << "\n";
}

/// Parse using beam search.
/// \param q Search queue over which to search and store the best parse found.
/// \param only_consistent Only allow consistent decisions.
/// (Default: false)
void Parser::parse_search(SearchQueue& q, bool only_consistent) {
	while(!q.done()) {
		const StateCost cur = q.pop();
		if (cur.empty()) {
			assert(q.done());
			break;
		}
		assert(!cur.state().is_final());
		assert(!cur.cost().is_final());

		Debug::log(3) << "Popped " << cur.state().to_string() << " with cost " << cur.cost().to_string() << "\n";

		this->explore(cur, q, only_consistent);
	}

	Debug::log(2) << "Best parse has cost " << q.best().cost().to_string() << "\n";
	Debug::log(3) << q.stats() << "\n";
}

/// Replay the Path of some StateCost.
/// \param only_consistent Only allow consistent decisions.
/// (Default: false)
void Parser::replay(const StateCost& s, bool only_consistent) {
	_parse_state = s.state();

	// Replay the path of the best parse found.
	Path path = _parse_state.path();
	ParseState tmpstate = _gold_state;
	Cost tmpcost;
	tmpstate.restart();
	Debug::log(3) << "Best path: ";
	while(!path.empty()) {
		const Production& p = path.pop();
		const Action& a = p.action();
		double conf;
		if (only_consistent && !tmpstate.is_legal(a)) {
			conf = -666;
		} else {
			conf = this->confidence(tmpstate, a);
		}
		tmpcost += conf;
		tmpstate.perform(a);
		Debug::log(3) << "conf(" << a.to_string() << ") = " << conf;
		if (!path.empty()) Debug::log(3) << ", ";
		else Debug::log(3) << "\n";
	}
	assert(s.cost() == tmpcost);
}

/// Explore a StateCost.
/// i.e. Expand the search by depth 1.
/// \return The best subsequent StateCost of those explored.
StateCost Parser::explore(const StateCost& s, SearchQueue& q, bool only_consistent) {
	if (!q.can_explore(s)) return StateCost();

	const ParseState& state = s.state();
	const Cost& cost= s.cost();
	assert(!state.is_final());

	Actions actions;
	if (only_consistent) {
		actions = state.consistent_actions();
	} else {
		actions = state.legal_actions();
	}
	assert(!actions.empty());
	Actions::const_iterator a;

	double greedy_best_conf = -BIGVAL;
	ParseState greedy_best_state;
	unsigned tiecount = 1;
	for (a = actions.begin(); a != actions.end(); a++) {
		double action_conf;
		if (only_consistent && !state.is_legal(*a)) {
			action_conf = -666;
		} else {
			action_conf = this->confidence(state, *a);
		}

		ParseState newstate = state;
#ifdef SANITY_CHECKS
		assert(newstate == state);
#endif /* SANITY_CHECKS */
		newstate.perform(*a, true);

		Cost newcost = cost + action_conf;
		if (newstate.is_final())
			newcost.finalize();

		q.add(newstate, newcost);

		if (greedy_best_conf == -BIGVAL || action_conf > greedy_best_conf) {
			greedy_best_conf = action_conf;
			greedy_best_state = newstate;
			tiecount = 1;
		} else if (fabs(action_conf - greedy_best_conf) < parameter::small_epsilon()) {
			tiecount++;
			if (drand48() < 1. / tiecount) {
				greedy_best_conf = action_conf;
				greedy_best_state = newstate;
			}
		}
	}
	// Since we've just explored state,
	// remove it from the search queue.
	q.remove(state, cost);

	/// If no subsequent 
	if (greedy_best_conf == -BIGVAL)
		return StateCost();

	Cost greedy_best_cost = cost + greedy_best_conf;
	if (greedy_best_state.is_final())
		greedy_best_cost.finalize();
	return StateCost(greedy_best_state, greedy_best_cost);
}
