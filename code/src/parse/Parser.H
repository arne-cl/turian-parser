/*  $Id: Parser.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Parser.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Parser
 *  \brief Parses a single sentence.
 *
 *  A Parser can parse a sentence in several different ways:
 *	- Simply parse the sentence, using a Classifier to evaluate
 *	potential Actions.
 *		- Choose Actions in a greedy fashion.
 *		- or, choose a parse Path using search.
 *	The parse can be compared to the gold-standard Treebank parse,
 *	dynamically tracking for each decision its delta prc, delta rcl,
 *	and delta crossing brackets, (both unlabelled and labelled).
 *
 *	- Generate training examples along some parse Path:
 *		- A random parse Path, consistent with the gold-standard
 *		Treebank parse.
 *		- or, a previously generated parse Path that we replay.
 *
 *	- Parse the sentence (using a Classifier to evaluate
 *	potential Actions) until it makes a mistake (i.e. it performs
 *	an Action inconsistent with the Treebank gold-standard parse.)
 *
 *  A Parser object is created from a State, typically the final State
 *  of a gold-standard parse tree in the Treebank.
 *
 *  \todo SUBTYPE into different Parser sub-classes.
 *  \todo Skip sentences that have too few words!
 *  \todo Better way to integrate Classifier?
 *  \todo Put State& arguments at the \b end of function parameters?
 *
 */

#ifndef __Parser_H__
#define  __Parser_H__

#include "parse/Chart.H"
#include "parse/ParseState.H"
#include "parse/SpanChart.H"

#include "learn/Classifier.H"

#include "universal/vocabulary.H"

class Action;
class Cost;
class SearchQueue;
class StateCost;

class ExampleREMOVE;
template<typename EXAMPLE> class Examples;

#include <deque>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class Parser {
public:
	/// Construct a Parser instance.
	/// \param gold_state The final state of the gold-standard parse.
	/// \todo Actually create new memory and point the ItemToken* to the
	/// new addresses. That way, we have no dangling pointers if the
	/// original ItemTokens are deleted.
	/// \todo Default value for classifier in constructor?
	Parser(const State& gold_state);

	/// Parse this sentence.
	/// \param only_consistent Only allow consistent decisions.
	/// (Default: false)
	/// \todo Write code to allow us to skip sentences that are too long.
	///	* Make the max sentence length a global.
	///	* Do we want to compute sentence size as the number of terminals
	///	or as the number of (non-punctuation* terminals?
	void parse(bool only_consistent=false);

	/// Parse this sentence, using search.
	/// \todo Write code to allow us to skip sentences that are too long.
	///	* Make the max sentence length a global.
	///	* Do we want to compute sentence size as the number of terminals
	///	or as the number of (non-punctuation* terminals?
	/// \param only_consistent Only allow consistent decisions.
	/// (Default: false)
	void parse_search(bool only_consistent=false);

	/// \todo RENAMEME! And add some documentation, you pill!
	void parse_search_greedy_initial(bool only_consistent=false);

	/// Parse this sentence until it makes the first error.
	/// Used for EDA.
	/// WRITEME
	/// \return True iff an error was made.
	bool parse_until_first_error();

	/// Generate examples from one sentence.
	/// For each state encountered along some consistent parse path, add the
	/// examples to exmpls.
	/// \note Iff parameter::treebank_has_parse_paths(), we use a
	/// fixed parse path to generate the examples.
	/// Otherwise, we use any random consistent parse path to generate the
	/// examples.
	/// \param exmpls A list into which we put the examples generated.
	/// \param target_label The target label for each decision. An
	/// example is true iff its its action is consistent with the treebank
	/// and if the action yields an item of type ::target_label.
	void generate_examples(Examples<Example>& exmpls, Label target_label);

	/// Make steps parse decisions.
	void parse_steps(unsigned steps);

	/// Retrieve the gold standard (treebank) state.
	/// \todo Make this method private.
	const ParseState& gold_state() const { return _gold_state; }

	/// Retrieve the parser's state.
	/// i.e. The state found by parsing.
	/// \todo Make this method private.
	const ParseState& parse_state() const { return _parse_state; }

	/// See if the Classifier makes an error in some skip-ahead State.
	/// \return rcl_good, prc_good,
	/// where rcl_good is true iff in the skip-ahead state we inferred a
	/// bracket that's correct given of the derivation order,
	/// and prc_good is true iff in the skip-ahead state we inferred a
	/// bracket that's correct irrespective of the derivation order
	/// (i.e. RCL errors are permissible).
	/// \todo Remove special case handling of r2l/l2r
	pair<bool, bool> skip_ahead_state_error();

	/// Retrieve state containing all final consistent decisions from
	/// pre-error parsing.
	/// \todo Describe this more clearly.
	/// \todo Ensure this ParseState is valid. i.e. That is was generated,
	/// through a ::parse_until_first_error() call.
	const ParseState& consistent_state() const { return _consistent_state; }

	/// Set the Classifer used by the parser.
	/// \todo Detect if ::_classifier has not been set, prior to using it?
	/// \todo Make this method private.
	void set_classifier(Classifier* c) { _classifier = c; }

	/// Parse the sentence using the Classifier and an oracle.
	/// We perform any consistent decision chosen by the Classifier.
	/// Whenever it makes a mistake, we use an oracle to randomly choose
	/// a consistent decision to perform.
	void parse_with_oracle();

	/// Consistently parse the sentence, using the Classifier to choose
	/// the order. Only when no legal consistent actions exist do
	/// we choose an illegal (consistent) action.
	void consistently_parse_using_classifier();

private:
	void restart();

	/// Parse using q search.
	/// \param q Search queue over which to search and store the best parse found.
	/// \param only_consistent Only allow consistent decisions.
	/// (Default: false)
	void parse_search(SearchQueue& q, bool only_consistent=false);

	/// \todo RENAMEME! And add some documentation, you pill!
	void parse_search_greedy(const StateCost& sc, SearchQueue& q, bool only_consistent=false);

	/// Explore a StateCost.
	/// i.e. Expand the search by depth 1.
	/// \return The best subsequent StateCost of those explored.
	StateCost explore(const StateCost& s, SearchQueue& q, bool only_consistent=false);

	/// Replay the Path of some StateCost.
	/// \param only_consistent Only allow consistent decisions.
	/// (Default: false)
	void replay(const StateCost& s, bool only_consistent=false);

	/// Make some consistent parse decision.
	/// Randomly choose some consistent action, and perform it.
	/// \param state The state in which the parse decision should be made.
	/// \todo Allow classifier to confidence-rate actions, and choose highest rated
	/// action to perform?
	void make_consistent_decision(ParseState& state);

	/// Make a consistent parse decision from a parse path.
	/// \param state The state in which the parse decision should be made.
	/// \param path The parse path that gives us the order of the actions.
	/// \sideeffect The action performed is popped from path.
	void make_consistent_decision(ParseState& state, Path& path);

	/// Make some consistent parse decision using the Classifier.
	/// Choose the highest confidence legal consistent action.
	/// Only is no legal consistent action exists do
	/// we choose an illegal (consistent) action.
	/// \param state The state in which the parse decision should be made.
	void make_consistent_decision_with_classifier(ParseState& state);

	/// Make all consistent parse decisions.
	/// \param state The state in which the parse decisions should be made.
	void make_all_consistent_decisions(ParseState& state);

	/// Make some parse decision.
	/// We find all possible actions at the current parse state.
	/// We choose some action, and perform it.
	/// \param state The state in which the parse decision should be made
	/// \param output_threshold If the absolute value of the
	/// confidence of the chosen decision is at least output_threshold,
	/// then we will output the pre- and post-decision states to
	/// Debug::log(3).
	/// \return True iff the action performed is already in the parse
	/// chart. This can be used to detect the first inconsistent
	/// decision, but no subsequent inconsistencies.
	bool make_decision(ParseState& state, double output_threshold=BIGVAL);

	/// Make some consistent parse decision using the Classifier and an oracle.
	/// If the Classifier makes a correct decision, then we make that.
	/// Otherwise, we use an oracle to randomly choose
	/// a consistent decision to perform.
	void make_decision_with_oracle(ParseState& state);

	/// Return the action with highest confidence.
	/// \note This can only score legal actions. [TODO: Check for action legality]
	/// \todo Tiebreak in favor of more common label?
	/// \return A pair containing the highest confidence action, and its confidence.
	pair<Action, double> best_action(const ParseState& state, Actions& actions) const;

	/// Sort actions by their expected likelihood
	/// \todo Make bucket size a parameter
	void sort_actions(Actions& actions) const;

	/// Perform an action.
	/// An item will be created iff allow_inconsistent and iff this
	/// action yields a new item.
	/// \param action The action to perform.
	/// \param state The state in which the action should be performed.
	/// \param allow_inconsistent Allow inconsistent actions?
	/// (Default: false)
	/// \return True iff the action performed is already in the parse
	/// chart. This can be used to detect the first inconsistent
	/// decision, but no subsequent inconsistencies.
	bool perform(const Action& action, State& state, bool allow_inconsistent=false);

	/// Confidence rate an Action in some State.
	double confidence(const State& state, const Action& action) const;

	/// Retrieve this parser's classifier.
	/// \todo Make sure that _classifier has been initialized.
	const Classifier& classifier() const { return *_classifier; }

	/// Retrieve a string describing the current PARSEVAL scores.
	/// \param s Prefix for every line.
	/// \warning Recall is based upon bottom-up ordering, and cannot compute the upper-bound based upon an l2r/r2l derivation.
	const string current_scores(string s="");

	/// The gold standard (treebank) state.
	ParseState _gold_state;

	/// The parser's state.
	/// i.e. The state found by parsing.
	ParseState _parse_state;

	/// The state containing all final consistent decisions from
	/// pre-error parsing.
	/// \todo Describe this more clearly.
	ParseState _consistent_state;

	Chart _gold_chart;
	Chart _parse_chart;

	/// \todo These belong in ParseState.
	SpanChart _gold_span_chart;
	SpanChart _parse_span_chart;

	/// Parse precision count.
	unsigned _parse_prc;
	/// Parse recall count.
	unsigned _parse_rcl;
	/// Parse crossed-brackets.
	unsigned _parse_cbs;

	Classifier* _classifier;
};

#endif /* __Parser_H__ */
