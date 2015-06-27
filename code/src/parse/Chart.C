/*  $Id: Chart.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Chart.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/Chart.H"
#include "parse/State.H"

#include "universal/Debug.H"

const Actions& Chart::consistent_bottom_up_actions() const {
	assert(!_actions.empty());
	return _actions;
}

/// Check whether an Action is a consistent, bottom-up decision.
/// i.e. Check whether an Action is in the chart.
/// \warning is_bottom_up_consistent returns a consistency check that
/// is *irrespective* of derivation order!
/// \param action The Action to check.
/// \return True iff action is in the chart. False otherwise.
bool Chart::is_bottom_up_consistent(const Action& action) const {
	return (_map.find(action) != _map.end());
}

/// Fetch the ItemToken associated with a particular Action.
/// \param action The Action to fetch.
/// \return The ItemToken of action. Never NULL.
/// \invariant Action action *must* be in the chart, otherwise an
/// assertion is thrown. Only use this function only after getting a
/// TRUE return value from Chart::action_exists().
const ItemToken* Chart::get(const Action& action) const {
	assert(this->is_bottom_up_consistent(action));
	assert(_map.find(action)->second != NULL);
	return _map.find(action)->second;
}

/// Create a new *terminal* ItemToken.
/// \param label (POS) Label of the new item.
/// \param word Actual word of the new item.
/// \param location Location of the new item (i.e. the number of terminals thus far).
/// Used to construct the width 1 span of the new item.
/// \param sentence The sentence in which this item appears.
/// \return A pointer to the new ItemToken.
/// \todo Assert location value? i.e. Make sure it's immediately
/// after the last location, and that this location hasn't already
/// been used.
const ItemToken* Chart::add(Label label, Word word, unsigned location, const ID<Sentence>& sentence) {
	assert(!_locked);
	Debug::log(6) << "Chart::add() received terminal: ";
	Debug::log(6) << "label = " << label_string(label) << ", word = " << word_string(word) << ", location = " << location << "\n";

	item_storage.push_back(ItemToken(label, word, location, sentence));
	_terminals.push_back(&item_storage.back());

	Debug::log(6) << "...Chart::add() constructed terminal\n";

	return &item_storage.back();
}

/// Create a new *constituent* ItemToken.
/// Given an Action, create the (new) ItemToken yielded by the action.
/// \param action An Action that is not part of any prior
/// Production.
/// \return The new ItemToken.
const ItemToken* Chart::add(const Action& action) {
	Debug::log(6) << "Chart::add() received action '" << action.to_string() << "'\n";

	assert(!this->is_bottom_up_consistent(action));

	item_storage.push_back(ItemToken(action));

	const ItemToken* i = &item_storage.back();
	_map.insert(make_pair(action, i));
	_actions.push_back(action);
	assert(this->is_bottom_up_consistent(action));
	assert(this->get(action) == i);
	Debug::log(6) << "Chart::add() yielded '" << i->to_string() << "'\n";

	return i;
}

/// Construct a chart from a state.
/// \todo Describe this function better.
/// \todo Actually create new memory and point the ItemToken* to the
/// new addresses. That way, we have no dangling pointers if the
/// original ItemTokens are deleted.
void Chart::construct(const State& state) {
	assert(!_locked);
	_map.clear();
	_actions.clear();
	_terminals.clear();
	item_storage.clear();

	ItemTokens::const_iterator i;
	for (i = state.frontier().begin(); i != state.frontier().end(); i++)
		this->construct(**i);

	assert(state.path().size() == _actions.size());

	this->sanity_check();
}

void Chart::construct(const ItemToken& item) {
	assert(!_locked);
	if (item.is_terminal()) {
		_terminals.push_back(&item);
		return;
	}

	assert(!this->is_bottom_up_consistent(item.action()));
	_map.insert(make_pair(item.action(), &item));
	assert(this->is_bottom_up_consistent(item.action()));

	_actions.push_back(item.action());

	ItemTokens::const_iterator c;
	for (c = item.children().begin(); c != item.children().end(); c++)
		this->construct(**c);
}

/// Sanity check a Chart object.
/// \todo Actually implement this sanity checks.
/// - Check that all items in item_storage are referenced in _items
/// - Check that we have all children items of actions
/// - Check that we have as many items as we need, and no more.
/// - Check all action spans for consistency.
/// \todo Check for TOP?
void Chart::sanity_check() const {
	assert(_map.size() == _actions.size());

	//assert(_terminals.size() + action_map.size() == _items.size());

/*
	hash_set<const ItemToken*> item_hash(_items.begin(), _items.end());
	assert (item_hash.size() == _items.size());

	/// - Check that all items in item_storage are referenced in _items
	for (list<ItemToken>::const_iterator i = item_storage.begin(); i != item_storage.end(); i++) {
		assert(item_hash.find(&(*i)) != item_hash.end());
	}

	/// - Check that we have all children items of actions
	for (hash_map<Action, ItemToken*, hash<Action>, equal_to<Action>>::const_iterator a = action_map.begin(); a != action_map.end(); a++) {
		for (ItemTokens::const_iterator i = a->first.children().begin(); i != a->first.children().end(); i++) {
			assert(item_hash.find(*i) != item_hash.end());
		}
	}

	/// - Check that we have as many items as we need, and no more.
	// First, remove all constituent items.
	for (hash_map<Action, ItemToken*, hash<Action>, equal_to<Action>>::const_iterator a = action_map.begin(); a != action_map.end(); a++) {
		assert(item_hash.find(a->second) != item_hash.end());
		item_hash.erase(item_hash.find(a->second));
	}
	// Second, remove all constituent items.
	for (ItemTokens::const_iterator i = terminals.begin(); i != terminals.end(); i++) {
		assert(item_hash.find(*i) != item_hash.end());
		item_hash.erase(item_hash.find(*i));
	}
	assert(item_hash.empty());
*/
	
	/// - Check all action spans for consistency.
	for (hash_map<Action, const ItemToken*, hash<Action>, equal_to<Action> >::const_iterator a = _map.begin(); a != _map.end(); a++) {
		assert(a->second->span() == a->first.span());
	}

	Span s(_terminals);
	assert(s.width() == _terminals.size());
}
