/*  $Id: Parser.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Parser.C
 *  \brief Implementations of top-level (public) Parser methods.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/Parser.H"

#include "learn/examples/Example.H"

#include "universal/Debug.H"
#include "universal/Double.H"
#include "universal/stats.H"

#include <queue>
#include <cstdlib>		///< For drand48

/// Parse this sentence.
/// \param only_consistent Only allow consistent decisions.
/// (Default: false)
/// \todo Write code to allow us to skip sentences that are too long.
///	* Make the max sentence length a global.
///	* Do we want to compute sentence size as the number of terminals
///	or as the number of (non-punctuation* terminals?
void Parser::parse(bool only_consistent) {
	this->restart();

/*
	ItemTokens terminals = this->get_terminals();

	if (terminals.size() > 40) {
		// Skip me
		// WRITEME: Or don't, depending upon parameters
		state.clear();
		return;
	}
*/

	Path workpath;
	if (only_consistent && parameter::treebank_has_parse_paths())
		workpath = _gold_state.path();

	while(!_parse_state.is_final()) {
		if (!only_consistent) {
			Debug::log(3) << this->current_scores() << "\n";
			this->make_decision(_parse_state);
		} else {
//			this->margin();
//			this->print_examples();
			if (parameter::treebank_has_parse_paths())
				this->make_consistent_decision(_parse_state, workpath);
			else
				this->make_consistent_decision(_parse_state);
		}
	}
	assert(workpath.empty());

	// We subtract one for TOP
	Debug::log(2) << "FINAL sentence #" << stats::sentence_count() << ": Correct brackets = " << _parse_prc << ", Test brackets = " << _parse_state.path().size()-1 << ", Gold brackets = " << _gold_state.path().size()-1 << "\n";

//	if (consistent)
//		assert(item_storage.empty());

//	state.add_fake_TOP();
}

/// Parse this sentence until it makes the first error.
/// Used for EDA.
/// WRITEME
/// \return True iff an error was made.
bool Parser::parse_until_first_error() {
	this->restart();

	bool error = false;
	unsigned cnt = 0;
	while(!_parse_state.is_final()) {
		_consistent_state = _parse_state;

		Debug::log(3) << "About to make a decision at state #" << cnt << "\n";
		// If we make an error...
		if (!this->make_decision(_parse_state)) {
			assert(_parse_state.path().size()-1 == cnt);
			Debug::log(2) << "First error made after " << 100.*cnt/_gold_state.path().size() << "% (" << cnt << "/" << _gold_state.path().size() << ") of the state was parsed (including TOP)\n";

			// ...then make all consistent decisions,
			// and return.
			error = true;

			Actions actions = _consistent_state.legal_actions();
			assert(!actions.empty());

//			Action action = this->best_action(_consistent_state, actions);
//			parameter::set_trace_confidence_paths(true);
//			double conf = this->confidence(_consistent_state, action);
//			Debug::log(3) << "TRACED " << action.to_string() << " with confidence " << conf << "\n";

			/// \warning This step may be broken,
			/// if we allow unary projections to self!
			Chart _consistent_chart;
			_consistent_chart.construct(_gold_state);
			_consistent_state.set_chart(&_consistent_chart);
			this->make_all_consistent_decisions(_consistent_state);

//			parameter::set_trace_confidence_paths(false);

			break;
		}
		cnt++;
	}

//	state.add_fake_TOP();

	return error;
}

/// See if the Classifier makes an error in some skip-ahead State.
/// \return rcl_good, prc_good,
/// where rcl_good is true iff in the skip-ahead state we inferred a
/// bracket that's correct given of the derivation order,
/// and prc_good is true iff in the skip-ahead state we inferred a
/// bracket that's correct irrespective of the derivation order
/// (i.e. RCL errors are permissible).
/// \todo Remove special case handling of r2l/l2r
pair<bool, bool> Parser::skip_ahead_state_error() {
	assert(_gold_state.path().size() >= 2);
	this->restart();

	Path workpath;
	if (parameter::treebank_has_parse_paths())
		workpath = _gold_state.path();

	assert(_parse_rcl == _gold_state.path().size() - 1);     // - 1 because TOP isn't scored.
	unsigned old_parse_prc;
	unsigned old_parse_rcl;

	// Choose a random number of decisions to perform, s.t. we have at least
	// one *non-TOP* move left to perform.
	unsigned decisions = (unsigned)(drand48() * (_gold_state.path().size()-1));

/*
	// Warning: When sampling half of the skip-ahead states, we
	// allow both halves to pick the *middle* state if there are an
	// odd number of skip-ahead states. Otherwise, we have no options
	// if there is only 1 skip-ahead state.
	// IDEALLY, we'd want to exclude that state.
	unsigned len = ((int)((_gold_state.path().size()-2)/2))+1;
	unsigned min = 0;		// First half of skip-ahead states
	//unsigned min = len-1;		// Second half of skip-ahead states
	decisions = (unsigned)(drand48() * len) + min;
	assert(decisions >= min && decisions < min+len);
	TRACE; cerr << "N = " << _gold_state.path().size() << ", i.e. " << _gold_state.path().size()-1 << " skip-ahead states\n";
	TRACE; cerr << "rand in [" << min << ", " << min+len << ") -> " << decisions << "\n";
	*/

	for (unsigned i = 0; i < decisions; i++) {
		old_parse_prc = _parse_prc;
		old_parse_rcl = _parse_rcl;

		assert(!_parse_state.is_final());
		if (parameter::treebank_has_parse_paths())
			this->make_consistent_decision(_parse_state, workpath);
		else
			this->make_consistent_decision(_parse_state);

		assert(_parse_prc == old_parse_prc+1);
		assert(_parse_rcl == old_parse_rcl);
//		assert(_parse_cbs == 0);
	}

	assert(!_parse_state.is_final());

	old_parse_prc = _parse_prc;
	old_parse_rcl = _parse_rcl;
//	old_parse_cbs = _parse_cbs;

	Debug::log(3) << _parse_state.consistent_legal_actions().size() << " legal, consistent decisions possible at this skip-ahead state.\n";

	/// \todo Remove special case handling of r2l/l2r
	if (parameter::parse_right_to_left() || parameter::parse_left_to_right()) {
		Actions legal_actions = _parse_state.legal_actions();
		assert(!legal_actions.empty());
		Action action = this->best_action(_parse_state, legal_actions).first;
		return make_pair(false, _parse_state.is_consistent(action));
	}

	bool rcl_good = this->make_decision(_parse_state, 2);
	bool prc_good;

	if (!rcl_good) {
		// The latter holds we made a RCL error, given our particular derivation ordering
		assert(_parse_prc == old_parse_prc || _parse_prc == old_parse_prc+1);
		// NB parse rcl tracks *bottom-up* recall, not r2l or l2r recall
		assert(_parse_rcl <= old_parse_rcl);

		// Did we merely make a RCL error wrt our particular derivation order?
		if (_parse_prc == old_parse_prc+1) {
			prc_good = true;
			Debug::log(3) << "Correct PRC, incorrect RCL.\n";
		} else {
			prc_good = false;
			Debug::log(3) << "Incorrect PRC, incorrect RCL.\n";
		}
	} else {
		assert(_parse_prc == old_parse_prc + 1);
		assert(_parse_rcl == old_parse_rcl);
		prc_good = true;
	
		Debug::log(3) << "Correct PRC, correct RCL.\n";
	}
	return make_pair(rcl_good, prc_good);
}

/// Generate examples from one sentence.
/// For each state encountered along some consistent parse path, add the
/// examples to exmpls.
/// \note Iff parameter::treebank_has_parse_paths(), we use a
/// fixed parse path to generate the examples.
/// Otherwise, we use any random consistent parse path to generate the
/// examples.
/// \param exmpls A list into which we put the examples generated.
/// \warning The contents of exmpls will be overwritten.
/// \param target_label The target label for each decision. An
/// example is true iff its its action is consistent with the treebank
/// and if the action yields an item of type ::target_label.
void Parser::generate_examples(Examples<Example>& exmpls, Label target_label) {
	this->restart();

	Path workpath;
	if (parameter::treebank_has_parse_paths())
		workpath = _gold_state.path();

	while(!_parse_state.is_final()) {
		_parse_state.generate_state_examples(exmpls, target_label);

		/// \todo Ensure that the decisions are random.
		this->make_consistent_decision(_parse_state, workpath);
	}
	assert(workpath.empty());
}

/// Make steps parse decisions.
void Parser::parse_steps(unsigned steps) {
	this->restart();

	for (unsigned i = 0; i < steps; i++) {
		assert(!_parse_state.is_final());
		this->make_decision(_parse_state);
	}
}

/// Parse the sentence using the Classifier and an oracle.
/// We perform an consistent decision chosen by the Classifier.
/// Whenever it makes a mistake, we use an oracle to randomly choose
/// a consistent decision to perform.
void Parser::parse_with_oracle() {
	this->restart();

	while(!_parse_state.is_final()) {
		this->make_decision_with_oracle(_parse_state);
	}
}

/// Consistently parse the sentence, using the Classifier to choose
/// the order. Only when no legal consistent actions exist do
/// we choose an illegal (consistent) action.
void Parser::consistently_parse_using_classifier() {
	this->restart();

	while(!_parse_state.is_final()) {
		this->make_consistent_decision_with_classifier(_parse_state);
	}
}
