/*  $Id: Parser-help.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Parser-help.C
 *  \brief Implementations of helper functions for Parser.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/Parser.H"
#include "parse/Production.H"

#include "learn/examples/Example.H"

#include "universal/Debug.H"
#include "universal/NBestSlow.H"
#include "universal/stats.H"
#include "universal/Random.H"

#include <cstdlib>		///< For drand48
#include <ostream>
#include <sstream>

/// Construct a Parser instance.
/// \param gold_state The final state of the gold-standard parse.
/// \todo Actually create new memory and point the ItemToken* to the
/// new addresses. That way, we have no dangling pointers if the
/// original ItemTokens are deleted.
/// \todo Default value for classifier in constructor?
Parser::Parser(const State& gold_state) {
	_gold_chart.construct(gold_state);
	_gold_chart.lock();
	_gold_state = gold_state;
	_gold_state.set_chart(&_gold_chart);
	_gold_span_chart.construct(gold_state);

	this->restart();
}

void Parser::restart() {
	_parse_chart = _gold_chart;
	_parse_chart.unlock();
	_parse_state = _gold_state;
	_parse_state.set_chart(&_parse_chart);
	_parse_state.restart();
	_parse_span_chart = _gold_span_chart;

	_parse_prc = 0;
	assert(_gold_state.path().back().action().label() == Label_TOP());
	_parse_rcl = _gold_state.path().size() - 1;	// - 1 because TOP isn't scored.
	_parse_cbs = 0;
}

/// Make some consistent parse decision.
/// Randomly choose some consistent action, and perform it.
/// \param state The state in which the parse decision should be made.
/// \todo Allow classifier to confidence-rate actions, and choose highest rated
/// action to perform?
void Parser::make_consistent_decision(ParseState& state) {
	assert(!parameter::treebank_has_parse_paths());

	Actions actions = state.consistent_actions();
	assert(!actions.empty());

	if (parameter::parse_left_to_right() || parameter::parse_right_to_left()) {
		assert(actions.size() == 1);
	}
	
	// Randomly choose some consistent action.
	Action action = actions.at(ParsePathRandom.get(actions.size()));

	//Action action = this->best_action(state, actions);

	Debug::log(5) << "Performing: " << action.to_string() << "\n";
	state.perform(action);

	// FIXME: Don't necessarily use _parse_span_chart.
	_parse_span_chart.subtract(SpanItem(action), _parse_prc, _parse_rcl, _parse_cbs);
}

/// Make some consistent parse decision using the Classifier.
/// Choose the highest confidence legal consistent action.
/// Only is no legal consistent action exists do
/// we choose an illegal (consistent) action.
/// \param state The state in which the parse decision should be made.
void Parser::make_consistent_decision_with_classifier(ParseState& state) {
	assert(!parameter::treebank_has_parse_paths());

	Actions actions = state.consistent_legal_actions();
	Action action;

	// If some consistent legal action exists,
	if (!actions.empty()) {
		action = this->best_action(state, actions).first;
		Debug::log(5) << "Performing: " << action.to_string() << "\n";

		bool debugout = false;
		double conf = this->confidence(state, action);
		if (conf <= -5) debugout = true;
		//if (conf <= -10) debugout = true;

		if (debugout) Debug::log(3) << "PRE  (conf=" << conf << "):  " << state.to_string() << "\n";
		bool ret = state.perform(action, true);
		assert(ret);
		if (debugout) Debug::log(3) << "POST (conf=" << conf << "): " << state.to_string() << "\n";
	} else {
		actions = state.consistent_illegal_actions();
		assert(!actions.empty());

		if (parameter::parse_left_to_right() || parameter::parse_right_to_left()) {
			assert(actions.size() == 1);
		}
	
		// Randomly choose some consistent (illegal) action.
		action = actions.at((unsigned)(drand48() * actions.size()));

		Debug::log(3) << "Performing illegal action: " << action.to_string() << "\n";
		bool ret = state.perform(action);
		assert(ret);
	}

	// FIXME: Don't necessarily use _parse_span_chart.
	_parse_span_chart.subtract(SpanItem(action), _parse_prc, _parse_rcl, _parse_cbs);
}

/// Make a consistent parse decision from a parse path.
/// \param state The state in which the parse decision should be made.
/// \param path The parse path that gives us the order of the actions.
/// \sideeffect The action performed is popped from path.
void Parser::make_consistent_decision(ParseState& state, Path& path) {
	assert(parameter::treebank_has_parse_paths());

	// Just use the first production from the parse path.
	Production p = path.pop();
	Action a = p.action();
//	assert(m_parser->ItemToken_from_action(a) == p.itemtoken());
	Debug::log(5) << "Performing: " << a.to_string() << "\n";
	state.perform(a);

	stats::total_actions_add(1);

	// FIXME: Don't necessarily use _parse_span_chart.
	_parse_span_chart.subtract(SpanItem(a), _parse_prc, _parse_rcl, _parse_cbs);
}

/// Make all consistent parse decisions.
/// \param state The state in which the parse decisions should be made.
void Parser::make_all_consistent_decisions(ParseState& state) {
	Actions actions = state.consistent_actions();
	assert(!actions.empty());

	Debug::log(3) << "Performing " << actions.size() << " consistent decisions...\n";

	Actions legal_actions = state.consistent_legal_actions();
	Actions illegal_actions = state.consistent_illegal_actions();
	assert(legal_actions.size() + illegal_actions.size() == actions.size());

	// Output the confidences *before* actually performing any of the actions.
	// (Otherwise, the confidences values will be tainted.)
	for (vector<Action>::const_iterator a = legal_actions.begin();
			a != legal_actions.end(); a++)
		Debug::log(3) << "\t" << a->to_string() << " (conf=" << this->confidence(state, *a) << ")\n";
	for (vector<Action>::const_iterator a = illegal_actions.begin();
			a != illegal_actions.end(); a++)
		Debug::log(3) << "\t" << a->to_string() << " (ILLEGAL)\n";

	for (vector<Action>::iterator a = actions.begin();
			a != actions.end(); a++) {
		Debug::log(4) << "Actually performing " << a->to_string() << "\n";
//		TRACE; cerr << state.to_string() << "\n";
//		TRACE; cerr << a->to_string() << "\n";
		bool b = state.perform(*a);
		assert(b);
//		Debug::log(4) << this->m_items.to_string() << "\n";
//		Debug::log(9) << this->to_string() << "\n";
	}
	Debug::log(3) << "...done performing " << actions.size() << " consistent decisions.\n";
}

/// Make some parse decision.
/// We find all possible actions at the current parse state.
/// We choose some action, and perform it.
/// \param state The state in which the parse decision should be made.
/// \param output_threshold If the absolute value of the
/// confidence of the chosen decision is at least output_threshold,
/// then we will output the pre- and post-decision states to
/// Debug::log(3).
/// \return True iff the action performed is already in the parse
/// chart. This can be used to detect the first inconsistent
/// decision, but no subsequent inconsistencies.
bool Parser::make_decision(ParseState& state, double output_threshold) {
	Actions actions = state.legal_actions();
	assert(!actions.empty());

	pair<Action, double> p = this->best_action(state, actions);
	Action action = p.first;
	double conf= p.second;

	bool debugout = false;
	if (fabs(conf) >= output_threshold) debugout = true;

	if (debugout) Debug::log(3) << "PRE  (conf=" << conf << "):  " << state.to_string() << "\n";
	else Debug::log(5) << "Performing: " << action.to_string() << "\n";

	bool ret = state.perform(action, true);

	if (debugout) Debug::log(3) << "POST (conf=" << conf << ", ret=" << ret << "): " << state.to_string() << "\n";

	// FIXME: Don't necessarily use _parse_span_chart.
	_parse_span_chart.subtract(SpanItem(action), _parse_prc, _parse_rcl, _parse_cbs);

	return ret;
}

/// Make some consistent parse decision using the Classifier and an oracle.
/// If the Classifier makes a correct decision, then we make that.
/// Otherwise, we use an oracle to randomly choose
/// a consistent decision to perform.
void Parser::make_decision_with_oracle(ParseState& state) {
	Actions actions = state.legal_actions();
	assert(!actions.empty());

	Action action = this->best_action(state, actions).first;

	if(state.is_consistent(action)) {
		Debug::log(5) << "Correctly performing: " << action.to_string() << "\n";
		stats::correct_actions_add(1);
	} else {
		Debug::log(5) << "Instead of " << action.to_string() << ", ";

		actions = state.consistent_actions();
		assert(!actions.empty());
		if (parameter::parse_left_to_right() || parameter::parse_right_to_left()) {
			assert(actions.size() == 1);
		}
		// Randomly choose some consistent action.
		action = actions.at((unsigned)(drand48() * actions.size()));

		Debug::log(5) << "oracle chose " << action.to_string() << "\n";
	}
	bool ret = state.perform(action, true);
	assert(ret);
	stats::total_actions_add(1);

	// FIXME: Don't necessarily use _parse_span_chart.
	_parse_span_chart.subtract(SpanItem(action), _parse_prc, _parse_rcl, _parse_cbs);
	Debug::warning(__FILE__, __LINE__, this->current_scores());
}

/// Sort actions by their expected likelihood
/// WRITEME more documentation
/// \todo Make bucket size a parameter
/// \todo Don't hardcode children span frequencies
void Parser::sort_actions(Actions& actions) const {
	/// Put all TOP inferences last (first?)
	Actions top_actions;

	vector<Actions> sort;

	/// \todo Make bucket size a parameter
	unsigned bucketsize = 1000;
	sort.resize(bucketsize);

	for (Actions::const_iterator a = actions.begin();
			a != actions.end(); a++) {
		if (a->label() == Label_TOP()) {
			top_actions.push_back(*a);
			continue;
		}

		double sizefreq = 0;

		/// \todo Don't hardcode children span frequencies
		/// We exclude TOP from these frequencies, since it skews the number of unaries
		switch(a->children().size()) {
			case 1: sizefreq = 0.231926; break;
			case 2: sizefreq = 0.565159; break;
			case 3: sizefreq = 0.147423; break;
			case 4: sizefreq = 0.0422515; break;
			case 5: sizefreq = 0.00985868; break;
			case 6: sizefreq = 0.00269639; break;
			case 7: sizefreq = 0.000553723; break;
			case 8: sizefreq = 3.61124e-05; break;
			case 9: sizefreq = 6.01873e-05; break;
			case 10: sizefreq = 1.20375e-05; break;
			case 11: sizefreq = 2.40749e-05; break;
			default: sizefreq = 0; break;
		}

		sizefreq /= 0.565159;
		double labelfreq = label_frequency(a->label()) / max_label_frequency();

		double freq = sizefreq * labelfreq;
		unsigned bucket = (unsigned)((bucketsize-1) * freq);
		assert(bucket >= 0 && bucket < bucketsize);
		sort.at(bucket).push_back(*a);
	}

	unsigned actionsize = actions.size();
	actions.clear();

	//for(vector<Actions>::const_reverse_iterator as = sort.rbegin();
	for(vector<Actions>::reverse_iterator as = sort.rbegin();
			as != sort.rend(); as++) {
		actions.insert(actions.end(), as->begin(), as->end());
	}
	// Insert TOP inferences at the end of the list
	actions.insert(actions.end(), top_actions.begin(), top_actions.end());
	assert(actions.size() == actionsize);
}

/// Return the action with highest confidence.
/// \note This can only score legal actions. [TODO: Check for action legality]
/// \todo Tiebreak in favor of more common label?
/// \return A pair containing the highest confidence action, and its confidence.
pair<Action, double> Parser::best_action(const ParseState& state, Actions& actions) const {
	unsigned tiecount = 1;

	// DISABLED FOR NOW
	assert(parameter::minconf_for_greedy_decision() > 100);
	//this->sort_actions(actions);

	pair<Action, double> best;
	best.second = -BIGVAL;
	Actions::const_iterator a;
	for (a = actions.begin(); a != actions.end(); a++) {
//		Debug::log(5) << "Considering action " << a->to_string() << "...\n";
		double conf = this->confidence(state, *a);
		if (conf > -10)
			Debug::log(5) << "Action " << a->to_string() << " has conf = " << conf << "\n";

		if (best.first.label() == NO_LABEL || conf > best.second) {
			best = make_pair(*a, conf);
			tiecount = 1;
		} else if (fabs(conf - best.second) < parameter::small_epsilon()) {
			tiecount++;
			if (drand48() < 1. / tiecount) {
				best = make_pair(*a, conf);
			}
		}

		if (conf > parameter::minconf_for_greedy_decision()) {
			assert(*a == best.first);
			assert(conf == best.second);
			Debug::log(3) << "Greedily choosing action " << best.first.to_string() << " with conf = " << best.second << " > mingreedyconf " << parameter::minconf_for_greedy_decision() << "\n";
			break;
		}
	}
	Debug::log(3) << "Best action " << best.first.to_string() << " with conf = " << best.second << "\n";
	return best;
}

/// Confidence rate an Action in some State.
double Parser::confidence(const State& state, const Action& action) const {
	Example e(&state, action);
	return this->classifier().confidence(action.label(), e);
}

/// Retrieve a string describing the current PARSEVAL scores.
/// \param s Prefix for every line.
/// \warning Recall is based upon bottom-up ordering, and cannot compute the upper-bound based upon an l2r/r2l derivation.
const string Parser::current_scores(string s) {
	ostringstream prefix;
	ostringstream o;
	prefix << s << "Sentence #" << stats::sentence_count() << ", decision #" << _parse_state.path().size() << ": ";
	o << prefix.str() << "Correct decisions thus far    = " << _parse_prc << "\n";
	o << prefix.str() << "Correct decisions upper bound = " << _parse_rcl << "\n";
	o << prefix.str() << "Crossing brackets thus far    = " << _parse_cbs << "\n";
	return o.str();
}


