/*  $Id: ParseState.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ParseState.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class ParseState
 *  \brief A State used by a Parser.
 *
 *  A ParseState is used to generate candidate Action%s, as well as generate
 *  training Example%s.
 *  ParseState extends State by containing a Chart*, which is used to
 *  determine which Actions are consistent with the Chart.
 *
 *  \todo Remove this class and move all the methods into State. State
 *  should then either contain a Chart*, or be passed a Chart* for the
 *  relevant functions. Is State ever used anywhere else besides Parser?
 *
 */

#ifndef __ParseState_H__
#define  __ParseState_H__

#include "parse/State.H"
#include "parse/Chart.H"
#include "parse/SpanChart.H"

#include "universal/LIST.H"	///< REMOVEME
//class LIST;

class Action;
class Actions;
class Example;
template<typename EXAMPLE> class Examples;

#include <deque>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class ParseState : public State {
public:
	ParseState() {
		this->clear();
		_chart = NULL;
	}

	ParseState& operator=(const State& state);

	/// Compare two partial parses for equality.
	/// Equality checks if the two states have the same frontier items.
	/// i.e. equality is irrespective of the states' paths.
	bool operator==(const ParseState& state) const {
		assert(_chart == state._chart);
		return _frontier == state._frontier;
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
	bool perform(const Action& action, bool allow_inconsistent=false);

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
	const Actions legal_actions(Label target_label=NO_LABEL) const;

	/// Get all consistent actions.
	/// \note If parameter::parse_left_to_right() or parameter::parse_right_to_left(),
	/// there is only one correct example per state.
	/// (Irrespective of parameter::consider_all_examples_despite_r2l_or_l2r())
	/// \warning Keep in mind that some of these decisions will be
	/// too long for classification.
	/// \return The list of all legal actions.
	const Actions consistent_actions() const;

	/// Get all legal, consistent actions
	/// \todo Combine these two functions into one.
	const Actions consistent_legal_actions() const;
	/// Get all illegal, consistent actions
	/// \todo Combine these two functions into one.
	const Actions consistent_illegal_actions() const;

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
	void generate_state_examples(Examples<Example>& exmpls, Label target_label) const;

	/// Construct the initial state from which this state was derived.
	/// Returns a state of all the terminals herein.
	State get_initial_state() const;

	/// \todo Sanity check the chart contents?
	/// \todo Make this private.
	void set_chart(Chart* chart) {
		_chart = chart;
	}

	/// Determine whether an Action is consistent.
	bool is_consistent(const Action& a) const;


	/// Determine whether an Action is legal.
	/// \todo This implementation is very slow!
	bool is_legal(const Action& a) const;

private:
	Chart& chart() const {
		assert (_chart);
		return *_chart;
	}

	Chart* _chart;
};

#ifndef DOXYGEN
namespace __gnu_cxx {
	template<> class hash<ParseState> {
		public: size_t operator()(const ParseState& s) const {
				return hash<string>()(s.frontier().to_string());
			}
	};
}
#endif


#endif /* __ParseState_H__ */
