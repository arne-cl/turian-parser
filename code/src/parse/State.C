/*  $Id: State.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file State.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/State.H"
#include "parse/Production.H"

#include "universal/globals.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"

#include <cstdlib>

/// Restart at the initial state.
/// Basically, clear the path and set the state items to be all its
/// terminals.
void State::restart() {
	*this = this->initial_state();
	assert(this->is_initial());
}

/// \todo Make this *private*!
/// \todo Add more safety checks.
void State::add_item_to_frontier(const ItemToken* i) {
	assert(i->is_terminal());
	if (_frontier.empty())
		assert(i->span().left() == 0);
	else
		assert(_frontier.back()->span().right()+1 == i->span().left());
	_frontier.push_back(i);
	this->create_initial_state();
}


const State& State::initial_state() const {
	if (!_initial_state) {
		assert(this->is_initial());
		return *this;
	} else {
		return *_initial_state;
	}
}

/// Determine if this is an initial State.
/// \return True iff every frontier item is a terminal.
bool State::is_initial() const {
	for (ItemTokens::const_iterator id = _frontier.begin(); id != _frontier.end(); id++) {
		const ItemToken& i = **id;
		if (!i.is_terminal()) {
			assert(!_path.empty());
			return false;
		}
	}
	assert(_path.empty());
	return true;
}

/// Is this a final state?
/// \return True iff the frontier contains a TOP item, and no non-TOP non-punctuation item.
bool State::is_final() const {
	unsigned topcnt = 0;

	for (ItemTokens::const_iterator id = _frontier.begin(); id != _frontier.end(); id++) {
		const ItemToken& i = **id;
		if (i.is_terminal()) {
			if (!i.is_punctuation())
				return false;
		} else {
			if (i.is_top())
				topcnt++;
			else
				return false;
		}
	}
	// // FIXME: Or just return true regardless?
	// return (topcnt == 1);

	// If the state contains only TOP items, then we should stop.
	// Otherwise, the parser will flail around, sometimes choosing PRT (!)
	// and generally incurring loss
	return true;
}

/// Locate an (ordered) list of ItemToken%s in the frontier.
ItemTokens::const_iterator State::locate(const ItemTokens& ids) const {
	bool found = false;
	ItemTokens::const_iterator i;
	for(i = _frontier.begin(); i != _frontier.end(); i++) {
		bool tmpfound = true;
		for (unsigned j = 0; j < ids.size(); j++)
			if (*(i+j) != ids.at(j)) {
				tmpfound = false;
				break;
			}
		if (tmpfound) {
			found = true;
			break;
		}
	}
	assert(found);
	return i;
}

/// Perform an action.
/// Given an action, update the state by replacing the children
/// items in the action by the yielded item.
/// \todo Should add to ::_path
/// \todo Where is this called from?
void State::perform(const Production& p) {
	boost::shared_ptr<const State> initial = _initial_state;
	if (this->is_initial()) {
		assert(!initial);
		initial = boost::shared_ptr<const State>(new const State(*this));
	}

	ItemTokens newfrontier;

	ItemTokens::const_iterator i;
	ItemTokens::const_iterator childi = this->locate(p.action().children());
	for(i = _frontier.begin(); i != childi; i++) {
		newfrontier.push_back(*i);
	}
	newfrontier.push_back(p.itemtoken());
	i += p.action().children().size();
	for(; i != _frontier.end(); i++) {
		newfrontier.push_back(*i);
	}
	assert(_frontier.size() == newfrontier.size() + p.action().children().size() - 1);
	Debug::log(5) << "Old state: " << _frontier.to_string() << "\n";
	Debug::log(5) << "New state: " << newfrontier.to_string() << "\n";
	_frontier = newfrontier;
	_path.add(p);

	_initial_state = initial;
	assert(this->initial_state().is_initial());
}

const string State::to_string() const {
	string s("");
	ItemTokens::const_iterator i;
	for (i = _frontier.begin(); i != _frontier.end(); i++) {
		s += (*i)->to_string_with_children();
		if (i+1 != _frontier.end())
			s += " ";
	}
	return s;
}

const Span State::span() const {
	return Span(_frontier);
}

/// Locate a span in the state.
/// \param s The span we are locating.
/// \return Those ItemTokens that the span comprises.
const ItemTokens State::locate_span(const Span& s) const {
	Span tmps(_frontier);
	assert(tmps.left() <= s.left());
	assert(tmps.right() >= s.right());

	ItemTokens is;
	ItemTokens::const_iterator i;
	for (i = _frontier.begin(); i != _frontier.end(); i++) {
		if ((*i)->span().left() == s.left())
			break;
		assert((*i)->span().left() < s.left());
	}
	assert((*i)->span().left() == s.left());

	for (; i != _frontier.end(); i++) {
		is.push_back(*i);
		if ((*i)->span().right() == s.right())
			break;
		assert((*i)->span().right() < s.right());
	}
	assert((*i)->span().right() == s.right());

	assert(!is.empty());
	return is;
}

/// Locate the left and right iterators of a span in the state.
/// The left iterator points to the leftmost *inside* the span.
/// The right iterator points to the rightmost *inside* the span.
/// (For a span containing one item, left == right.)
/// \param s The span we are locating.
/// \return A pair of ItemToken iterators, left and right.
pair<ItemTokens::const_iterator, ItemTokens::const_iterator> State::span_iterators(const Span& s) const {
	Span tmps(_frontier);
	assert(tmps.left() <= s.left());
	assert(tmps.right() >= s.right());

	ItemTokens::const_iterator l, r;

	ItemTokens::const_iterator i;
	for (i = _frontier.begin(); i != _frontier.end(); i++) {
		if ((*i)->span().left() == s.left())
			break;
		assert((*i)->span().left() < s.left());
	}
	assert((*i)->span().left() == s.left());
	l = i;

	for (; i != _frontier.end(); i++) {
		if ((*i)->span().right() == s.right())
			break;
		assert((*i)->span().right() < s.right());
	}
	assert((*i)->span().right() == s.right());
	r = i;

	return make_pair(l, r);
}


ItemTokens State::terminals_slow() const {
	ItemTokens items;

	for (ItemTokens::const_iterator i = _frontier.begin(); i != _frontier.end(); i++)
		(*i)->get_terminals(items);
	return items;
}

/// Create the initial State from the current one.
void State::create_initial_state() {
	if (this->is_initial()) {
		_initial_state.reset();
	} else {
		// Create a new State to hold the initial state.
		State initial;
		initial.clear();
		initial._frontier = this->terminals_slow();
		assert(initial.is_initial());

		// A pointer to the initial version of this State.
		_initial_state = boost::shared_ptr<const State>(new const State(initial));
		assert(this->initial_state().is_initial());
	}
}

/// Find the terminal ItemToken with some span location.
const ItemToken& State::locate_terminal(const Span& span) const {
	assert(span.width() == 1);
	const ItemToken* t = this->initial_state().frontier().at(span.left());
	assert(t);
	assert(t->span() == span);
	return *t;
}
