/*  $Id: ParseState.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ParseState.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/ParseState.H"
#include "parse/Action.H"

#include "learn/examples/Example.H"
#include "learn/examples/Examples.H"

#include "universal/Debug.H"

ParseState& ParseState::operator=(const State& state) {
	_path = state.path();
	_frontier = state.frontier();
	_initial_state = state.initial_state_ptr();
	_chart = NULL;
	return *this;
}

/// Determine whether an Action is consistent.
bool ParseState::is_consistent(const Action& a) const {
	bool is_consistent;
	if (parameter::parse_left_to_right() || parameter::parse_right_to_left()) {
		Actions correct = this->consistent_actions();
		assert(correct.size() == 1);
		is_consistent = (a == correct.front());
	} else {
		is_consistent = this->chart().is_bottom_up_consistent(a);
	}

/*
	// Sanity check
	bool is_consistent2 = false;
	Actions correct = this->consistent_actions();
	for (Actions::const_iterator a2 = correct.begin(); a2 != correct.end(); a2++) {
		if (a == *a2) {
			is_consistent2 = true;
			break;
		}
	}
	assert(is_consistent == is_consistent2);
*/

	return is_consistent;
}

/// Perform an action.
/// An item will be created iff allow_inconsistent and iff this
/// action yields a new item.
/// \param action The action to perform.
/// \param allow_inconsistent Allow inconsistent actions?
/// (Default: false)
/// \return True iff the action performed is consistent.
/// This can be used to detect the first inconsistent
/// decision, but no subsequent inconsistencies.
/// \todo FIXME: Remove this weird special-case logic for r2l/l2r parsing!
bool ParseState::perform(const Action& action, bool allow_inconsistent) {
	if (Debug::will_log(6))
		Debug::log(6) << "ParseState::perform received action '" << action.to_string() << "'\n";

	const ItemToken* i = NULL;
	Chart& chart = this->chart();

	bool ret;
	/// \todo FIXME: Remove this weird special-case logic for r2l/l2r parsing!
	if (parameter::parse_right_to_left() || parameter::parse_left_to_right()) {
		ret = false;
	} else {
		ret = this->is_consistent(action);
		assert(ret == this->chart().is_bottom_up_consistent(action));
	}

	if (!ret) {
		// Although this action is inconsistent, it may still be consistent with the bottom-up ordering.
		// e.g. If we're parsing r2l, then a certain decision may be inconsistent with the r2l path, but
		// not inconsistent with non-unique bottom-up derivations.
		if (!this->chart().is_bottom_up_consistent(action)) {
			// Only allow new items to be created if we're allowing
			// inconsistent parsing.
			assert(allow_inconsistent);

			// Since this action has not been priorly used in any of this
			// Parser's productions, create a new item which is the yield of
			// the action.
			Debug::log(6) << "Adding performed action to chart: " << action.to_string() << "\n";
			i = chart.add(action);
		} else {
			Debug::log(6) << "Performed action already in chart: " << action.to_string() << "\n";
			i = chart.get(action);
		}
	} else {
		Debug::log(6) << "Performed action already in chart: " << action.to_string() << "\n";
		i = chart.get(action);
	}
	assert(i != NULL);

	Debug::log(5) << "Performing: " << action.to_string() << "\n";
	Production p(i, action);
	this->State::perform(p);
	Debug::log(5) << this->to_string() << "\n";

	return ret;
}

/// Get all legal actions.
/// An action is legal if it is of length <= parameter::max_span_length(),
/// and it does not duplicate other unary projections.
/// Also, disallow actions with punctuation as the leftmost or rightmost item.
/// Also, if we are parsing left-to-right/right-to-left
/// and !parameter::consider_all_examples_despite_r2l_or_l2r(), we allow
/// an inference only if there are no non-terminal constituents to
/// its right/left.
/// \todo WRITEME More description.
/// \param target_label Allow only actions that project to target_label.
/// NO_LABEL (default) iff all actions are allowed, regardless of label.
/// \return The list of legal actions.
/// \warning actions will be initialized.
const Actions ParseState::legal_actions(Label target_label) const {
	Actions actions;

	if (parameter::convert_PRT_to_ADVP())
		assert(target_label != string_to_label("PRT"));

	Span nonterminal_span;
	bool have_nonterminal_span = false;
	if (parameter::parse_left_to_right() && !parameter::consider_all_examples_despite_r2l_or_l2r()) {
		// Find the rightmost non-terminal
		ItemTokens::const_reverse_iterator i;
		for (i = _frontier.rbegin(); i != _frontier.rend(); i++) {
			if (!((*i)->is_terminal())) {
				have_nonterminal_span = true;
				nonterminal_span = (*i)->span();
				if (Debug::will_log(6))
					Debug::log(6) << "Inference cannot be left of " << nonterminal_span.to_string() << "\n";
				break;
			}
		}
	} else if (parameter::parse_right_to_left() && !parameter::consider_all_examples_despite_r2l_or_l2r()) {
		// Find the leftmost non-terminal
		ItemTokens::const_iterator i;
		for (i = _frontier.begin(); i != _frontier.end(); i++) {
			if (!((*i)->is_terminal())) {
				have_nonterminal_span = true;
				nonterminal_span = (*i)->span();
				if (Debug::will_log(6))
					Debug::log(6) << "Inference cannot be right of " << nonterminal_span.to_string() << "\n";
				break;
			}
		}
	}

	ItemTokens::const_iterator i;
	for (i = _frontier.begin(); i != _frontier.end(); i++) {
		ItemTokens ids;

		// Disallow actions with punctuation as the leftmost item.
		const ItemToken& left = **i;
		if (parameter::raise_punctuation() && left.is_terminal() && is_punctuation_terminal(left.label()))
			continue;

		for (unsigned j = 0; j < parameter::max_span_length(); j++) {
			if (i+j == _frontier.end())
				break;

			ids.push_back(*(i+j));
			assert(ids.size() == j+1);

			// Disallow actions with punctuation as the rightmost item.
			const ItemToken& right = **(i+j);
			if (parameter::raise_punctuation() && right.is_terminal() && is_punctuation_terminal(right.label()))
				continue;

			// FIXME: Slow to use list.
			LIST<Label> skip_labels;

			// If this is a unary projection, then disallow
			// a unary projection to any constituent label this span already has.
			if (j == 0 && !left.is_terminal()) {
				skip_labels.push_back(left.label());
				ItemToken* tmpchild = (ItemToken*)&left;
				while (!tmpchild->is_terminal() && tmpchild->children().size() == 1) {
					tmpchild = (ItemToken*)(tmpchild->children().at(0));
					if (!tmpchild->is_terminal()) {
						// If we've already made a TOP projection, we
						// *should* be finished parsing.
						assert(tmpchild->label() != string_to_label("TOP"));
						
						skip_labels.push_back(tmpchild->label());
					}
				}
			}

			LIST<Label>::const_iterator l;
			for (l = all_constituent_labels().begin(); l != all_constituent_labels().end(); l++) {
				// Skip any l in skip_labels
				// FIXME: Slow to use list.
				LIST<Label>::iterator skipl;
				bool skipme = false;
				for (skipl = skip_labels.begin(); skipl != skip_labels.end();
						skipl++) {
					if (*l == *skipl) {
						skipme = true;
						break;
					}
				}
				if (skipme)
					continue;

				// If we have a desired label, then only allow actions
				// with this label.
				if (*l != target_label && target_label != NO_LABEL)
					continue;
				if (parameter::convert_PRT_to_ADVP() && *l==string_to_label("PRT"))
					// Explicitly disallow PRT decisions
					continue;

				assert(ids.size() <= parameter::max_span_length());
					
				// Only consider TOP projections that span the entire state,
				// which must be only one item.
				if (*l == Label_TOP() && (ids.size() < _frontier.size() ||
						ids.size() != 1))
					continue;

				Action a(*l, ids);
				if (have_nonterminal_span) {
					if (parameter::parse_left_to_right() && !parameter::consider_all_examples_despite_r2l_or_l2r()) {
						// Allow the action only if
						// it dominates the non-terminal span
						// or is to its right
						if (!(a.span() >= nonterminal_span ||
								a.span().left() > nonterminal_span.right()))
							continue;
					} else if (parameter::parse_right_to_left() && !parameter::consider_all_examples_despite_r2l_or_l2r()) {
						// Allow the action only if
						// it dominates the non-terminal span
						// or is to its left
						if (!(a.span() >= nonterminal_span ||
								a.span().right() < nonterminal_span.left()))
							continue;
					} else {
						assert(0);
					}
				}

				actions.push_back(a);
			}
		}
	}

	return actions;
}

/// Get all consistent actions.
/// \note If parameter::parse_left_to_right() or parameter::parse_right_to_left(),
/// there is only one correct example per state.
/// (Irrespective of parameter::consider_all_examples_despite_r2l_or_l2r())
/// \warning Keep in mind that some of these decisions will be
/// too long for classification.
/// \return The list of all legal actions.
const Actions ParseState::consistent_actions() const {
	Actions actions;

	ItemTokens::const_iterator i;
	for (i = _frontier.begin(); i != _frontier.end(); i++) {
		Actions::const_iterator a;
		for (a = this->chart().consistent_bottom_up_actions().begin(); a != this->chart().consistent_bottom_up_actions().end(); a++) {
			unsigned j;
			bool found_match = true;
			const ItemTokens& c = (*a).children();
			assert(!c.empty());
			for (j = 0; j < c.size() && found_match; j++) {
				if (i + j == _frontier.end()) {
					found_match = false;
					break;
				}
				if (*(i + j) != c.at(j)) {
					found_match = false;
					break;
				}
			}
			if (found_match) {
				assert(j > 0);

				actions.push_back(*a);
			}
		}
	}
	assert(!actions.empty());

	if (parameter::parse_left_to_right()) {
		// Choose the leftmost consistent action
		assert(!actions.empty());
		Action action = actions.front();
		Actions::const_iterator a;
		for (a = actions.begin()+1; a != actions.end(); a++) {
			if (a->span().left() < action.span().left()) {
				assert(a->span().right() < action.span().left());
				action = *a;
			} else {
				assert(a->span().left() > action.span().right());
			}
		}

		// Keep only the leftmost consistent action
		actions.clear();
		actions.push_back(action);
	} else if (parameter::parse_right_to_left()) {
		// Choose the rightmost consistent action
		assert(!actions.empty());
		Action action = actions.front();
		Actions::const_iterator a;
		for (a = actions.begin()+1; a != actions.end(); a++) {
			if (a->span().right() > action.span().right()) {
				assert(a->span().left() > action.span().right());
				action = *a;
			} else {
				assert(a->span().right() < action.span().left());
			}
		}

		// Keep only the rightmost consistent action
		actions.clear();
		actions.push_back(action);
	} else {
		// Sanity check
		for (Actions::const_iterator a = actions.begin(); a != actions.end(); a++)
			assert(this->is_consistent(*a));
	}

	return actions;
}

/// Get all legal, consistent actions
/// \todo Combine these two functions into one.
const Actions ParseState::consistent_legal_actions() const {
	Actions actions;

	Actions consistent_actions = this->consistent_actions();
	assert(!consistent_actions.empty());

	Actions legal_actions = this->legal_actions();
	for (vector<Action>::const_iterator a = consistent_actions.begin();
			a != consistent_actions.end(); a++) {
		bool illegal = true;
		for (vector<Action>::const_iterator a2 = legal_actions.begin();
				a2 != legal_actions.end(); a2++) {
			if (*a == *a2) {
				illegal = false;
				break;
			}
		}
		if (!illegal)
			actions.push_back(*a);
	}
	return actions;
}

/// Get all illegal, consistent actions
/// \todo Combine these two functions into one.
const Actions ParseState::consistent_illegal_actions() const {
	Actions actions;

	Actions consistent_actions = this->consistent_actions();
	assert(!consistent_actions.empty());

	Actions legal_actions = this->legal_actions();
	for (vector<Action>::const_iterator a = consistent_actions.begin();
			a != consistent_actions.end(); a++) {
		bool illegal = true;
		for (vector<Action>::const_iterator a2 = legal_actions.begin();
				a2 != legal_actions.end(); a2++) {
			if (*a == *a2) {
				illegal = false;
				break;
			}
		}
		if (illegal)
			actions.push_back(*a);
	}
	return actions;
}

/// Generate examples from the current state.
/// We append these examples to exmpls.
/// \note If parameter::parse_left_to_right() or parameter::parse_right_to_left(),
/// there is only one correct example per state.
/// (Irrespective of parameter::consider_all_examples_despite_r2l_or_l2r())
/// \warning This assumes that only consistent actions are in
/// _chart (i.e. we are doing consistent parsing).
/// \param exmpls A list into which we put the examples generated.
/// \param target_label The target label for each decision. An
/// example is true iff its its action is consistent with the treebank
/// and if the action yields an item of type ::target_label.
/// \bug LEAK: The State memory allocated in this function is never freed.
void ParseState::generate_state_examples(Examples<Example>& exmpls, Label target_label) const {
	if (parameter::convert_PRT_to_ADVP())
		assert(target_label != string_to_label("PRT"));

	Actions actions = this->legal_actions(target_label);

	unsigned all_legal_actions = this->legal_actions().size();
	unsigned correct_legal_actions = this->consistent_legal_actions().size();

	assert(all_legal_actions >= correct_legal_actions);

	// Create a copy of this State.
	/// \bug LEAK: The State memory allocated is never freed.
	const State* s = new State(*this);

	Actions::const_iterator a;
	Example e;
	for (a = actions.begin(); a != actions.end(); a++) {
		// Determine if e is a correct example or not.
		// We do this by constructing this action and
		// seeing if it's present in action_map.
		assert(a->children().size() <= parameter::max_span_length());
		bool is_correct = this->is_consistent(*a);

		if (parameter::share_example_weight_per_state()) {
			if (is_correct)
				e = Example(s, *a, is_correct, 1. / correct_legal_actions);
			else
				e = Example(s, *a, is_correct, 1. / (all_legal_actions - correct_legal_actions));
		} else {
			e = Example(s, *a, is_correct);
		}

		exmpls.push_back(e);
	}
}

/// Determine whether an Action is legal.
/// \todo This implementation is very slow!
bool ParseState::is_legal(const Action& action) const {
	Actions actions = this->legal_actions(action.label());

	Actions::const_iterator a;
	for (a = actions.begin(); a != actions.end(); a++) {
		if (action == *a) return true;
	}
	return false;
}
