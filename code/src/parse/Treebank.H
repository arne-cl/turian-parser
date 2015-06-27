/*  $Id: Treebank.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Treebank.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Treebank
 *  \brief An entire treebank of gold-standard parse trees.
 *
 *  \todo More documentation.
 *  \todo Skip sentences that have too few words!
 *  \todo Write a copy constructor.
 *
 */

#ifndef __Treebank_H__
#define  __Treebank_H__

#include "parse/Chart.H"
#include "parse/State.H"

#include <list>
#include <string>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class ItemToken;
class ItemTokens;
class Parser;
class PennTokenizer;
class Classifier;

class Treebank {
public:
	/// Empty constructor.
	/// \param Ensure that an empty treebank instance is not usable.
	Treebank() { }

	/// Load an entire treebank.
	/// \param filename The file containing the treebank.
	Treebank(string filename) {
		this->read(filename);
	}

	/// Load an entire treebank.
	/// \param filename The file containing the treebank.
	/// \todo How do we lex treebank with fixed parse paths?
	void read(string filename);

	/// NO copy constructor.
	/// Copies are not permitted.
	Treebank& operator= (const Treebank& t) {
		assert(0);
		return *this;
	}

	/// Choose new parse paths randomly.
	/// i.e. Draw a new sample set from the treebank.
	/// \param seed Seed for the random number generator.
	void choose_new_parse_paths(unsigned long seed);

	/// Choose new parse paths, using the Classifier and the oracle.
	/// i.e. Draw a new sample set from the treebank.
	/// \param seed Seed for the random number generator.
	void choose_new_parse_paths(Classifier& classifier, unsigned long seed);

	/// Convert the treebank to a string.
	/// Iff !parameter::treebank_has_parse_paths(), we output a Penn-style
	/// treebank.
	/// Iff parameter::treebank_has_parse_paths(), we output a Penn-style
	/// treebank and parse paths.
	/// \todo We can do this by iterating directly over ::sentences.
	const string to_string();

	/// Start retrieving sentences.
	/// \todo Allow random order, i.e. shuffled sentences, instead of treebank order?
	/// \todo Allow more than one pass over the examples?
	/// \todo Assert that sentence_iterator is invalid.
	void start();

	/// Stop retrieving sentences.
	/// This must be called if sentence retrieval is stopped prematurely
	/// and then restarted. Otherwise, the Treebank will not be closed.
	void stop();

	/// Are we done retrieving sentences?
	/// \todo Assert that sentence_iterator is valid, i.e. that
	/// Treebank::start() has been called.
	bool is_done() const;

	/// Retrieve the next sentence in the treebank.
	/// \todo Slow to return Parser instance?
	/// \invariant !is_done()
	/// \todo Assert that sentence_iterator is valid, i.e. that
	/// Treebank::start() has been called.
	/// \todo Skip sentences that have too few words!
	const Parser next_sentence();

	unsigned size() const {
		assert(!sentences.empty());
		return sentences.size();
	}

	/// Sample a sentence from the treebank.
	/// Each sentence is weighted by the number of constituents therein.
	/// \param include_top True iff TOP should be included in the constituents count. (Default: false)
	const Parser sample_sentence(bool include_top=false);

private:
	/// Input a sentence (i.e. tree) in the Penn Treebank format.
	/// \param lexer A tokenizer for the desired input stream.
	/// \return True iff we read a tree (false iff EOF).
	/// \todo WRITEME Do something with parse path!
	/// \todo Determine whether we want to lex a path, given a global variable.
	bool lex_sentence(const PennTokenizer &lexer);

	/// Read a node from the given lexer, and recursively read its children.
	/// We assume that LPAREN has already been consumed.
	/// \param terminal_cnt The number of terminal items lexed so far.
	/// \todo Assert label is a valid constituent or terminal label
	const ItemToken* lex_item(const PennTokenizer &lexer, const unsigned terminal_cnt, const ID<Sentence>& sentence);

	/// Read a path from the given lexer.
	/// \param lexer A tokenizer for the desired input stream.
	/// \param state We use state to replay the parse path and map
	/// Spans to current ItemToken*'s for each production.
	/// \sideeffects state will be updated to contain the path that we lex.
	void lex_path(const PennTokenizer &lexer, State& state);

	/// Read a single production from the given lexer.
	/// \param state We use state to replay the parse path and map
	/// Spans to current ItemToken*'s for each production.
	/// \return True iff we lexed a production. False iff we reached the
	/// end of the path (and consumed an RPAREN).
	bool lex_production(const PennTokenizer &lexer, State& state);

	/// Sentences (i.e. trees) in the treebank.
	/// Each sentence is stored as a State
	list<State> sentences;

	/// WRITEME
	/// \todo Make generic iterator for this class, w/o need for
	/// start() + next_sentence() methods?
	/// \warning This iterator will be invalidated if we copy the treebank
	/// instance.
	list<State>::iterator sentence_iterator;

	bool has_started;

	Chart _chart;

	unsigned _total_constituents_excluding_TOP;
	unsigned _total_constituents_including_TOP;
};

#endif /* __Treebank_H__ */
