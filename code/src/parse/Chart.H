/*  $Id: Chart.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Chart.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Chart
 *  \brief Store parse ItemToken%s.
 *
 *  The ItemTokens in a Chart need not be consistent with each other.
 *  For example, when parsing, we may explore many different, inconsistent
 *  parse Path%s. We store the ItemToken%s of these different parse Path%s in
 *  a single Chart, to save memory by not duplicating duplicate ItemToken%s
 *  yielded by different Path%s.
 *
 *  However, if desired, we can only store consistent
 *  (from a bottom-up perspective) ItemTokens in
 *  a Chart. For instance, if a Chart contains only ItemToken%s from
 *  a Treebank parse tree, then we can determine---when parsing---if a
 *  proposed action is consistent (bottom-up) with the Treebank by checking if the
 *  ItemToken yielded by the action is in a chart.
 *
 *  \todo Add copy constructor, which will throw an assertion. (Copies
 *  are not permitted, to prevent dangling pointers.)
 *
 */

#ifndef __Chart_H__
#define  __Chart_H__

#include "parse/Action.H"
#include "parse/ItemToken.H"

#include "universal/vocabulary.H"

class Actions;
class State;

#include <list>
#include <ext/hash_map>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

class Chart {
public:
	Chart() :
		_map(hash_map<Action, const ItemToken*, hash<Action>, equal_to<Action> >()),
		_actions(Actions()),
		_terminals(ItemTokens()),
		item_storage(list<ItemToken>()),
		_locked(false) {}

	/// SAFE copy constructor.
	/// This assignment operation will throw an assertion is
	/// ::item_storage is non-empty.
	/// Reason being, if we copy the item_storage, there will be
	/// dangling pointers to the *old* item_storage objects. Remapping
	/// pointers to the *new* item_storage objects is also unsafe,
	/// for reasons I determined five minutes ago but just forgot.
	/// \invariant ::item_storage must be empty.
	Chart& operator= (const Chart& c) {
		assert(c.item_storage.empty());
		_map = c._map;
		_actions = c._actions;
		_terminals = c._terminals;
		_locked = c._locked;
                return *this;
	}

	const Actions& consistent_bottom_up_actions() const;

	/// Check whether an Action is a consistent, bottom-up decision.
	/// i.e. Check whether an Action is in the chart.
	/// \warning is_bottom_up_consistent returns a consistency check that
	/// is *irrespective* of derivation order!
	/// \param action The Action to check.
	/// \return True iff action is in the chart. False otherwise.
	bool is_bottom_up_consistent(const Action& action) const;

	/// Fetch the ItemToken associated with a particular Action.
	/// \param action The Action to fetch.
	/// \return The ItemToken of action. Never NULL.
	/// \invariant Action action *must* be in the chart, otherwise an
	/// assertion is thrown. Only use this function only after getting a
	/// TRUE return value from Chart::action_exists().
	const ItemToken* get(const Action& action) const;

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
	const ItemToken* add(Label label, Word word, unsigned location, const ID<Sentence>& sentence);

	/// Create a new *constituent* ItemToken.
	/// Given an Action, create the (new) ItemToken yielded by the action.
	/// \param action An Action that is not part of any prior
	/// Production.
	/// \return The new ItemToken.
	const ItemToken* add(const Action& action);

	/// Construct a chart from a state.
	/// \todo Describe this function better.
	/// \todo Actually create new memory and point the ItemToken* to the
	/// new addresses. That way, we have no dangling pointers if the
	/// original ItemTokens are deleted.
	void construct(const State& state);

	/// Lock the chart against modification.
	void lock() { _locked = true; }

	void unlock() { assert(_locked); _locked = false; }

private:
	void construct(const ItemToken& item);

	/// Sanity check a Chart object.
	/// \todo Actually implement this sanity checks.
	/// - Check that all items in item_storage are referenced in _items
	/// - Check that we have all children items of actions
	/// - Check that we have all children items of actions
	/// - Check that we have as many items as we need, and no more.
	/// - Check all action spans for consistency.
	/// \todo Check for TOP?
	void sanity_check() const;

	/// Map from Action to constituent ItemToken*
	hash_map<Action, const ItemToken*, hash<Action>, equal_to<Action> > _map;

	/// Actions in the chart.
	Actions _actions;

	/// Terminal items in the chart.
	ItemTokens _terminals;

	/// Actual storage location of ItemTokens.
	list<ItemToken> item_storage;

	bool _locked;
};

#endif /* __Chart_H__ */
