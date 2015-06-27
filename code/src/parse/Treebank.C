/*  $Id: Treebank.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Treebank.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/Treebank.H"
#include "parse/Parser.H"
#include "parse/PennTokenizer.H"
#include "parse/State.H"

#include "learn/Classifier.H"

#include "universal/Debug.H"
#include "universal/stats.H"
#include "universal/Random.H"

/// Load an entire treebank.
/// \param filename The file containing the treebank.
void Treebank::read(string filename) {
	Debug::log(1) << "Treebank::Treebank() opening file: " << filename << "\n";

	PennTokenizer lexer;
	lexer.open(filename);

	assert(sentences.empty());
	unsigned cnt = 0;
	while (this->lex_sentence(lexer)) {
		cnt++;
	}
	lexer.close();

	assert(cnt != 0);
	Debug::log(1) << "Treebank::Treebank() read " << cnt << " sentences from treebank: " << filename << "\n";

	has_started = false;

	_total_constituents_excluding_TOP = 0;
	_total_constituents_including_TOP = 0;

	// Count the total number of constituents
	this->start();
	while(!this->is_done()) {
		Parser parser = this->next_sentence();
		const Path& p = parser.gold_state().path();
		assert(p.back().action().label() == Label_TOP());
		assert(p.size() >= 1);
		_total_constituents_including_TOP += p.size();
		_total_constituents_excluding_TOP += p.size() - 1;
	}


//	this->sanity_check();
}

/// Choose new parse paths randomly.
/// i.e. Draw a new sample set from the treebank.
/// \param seed Seed for the random number generator.
void Treebank::choose_new_parse_paths(unsigned long seed) {
	assert(parameter::treebank_has_parse_paths() == false);

//	/// \todo FIXME: The following doesn't feel kosher
//	parameter::treebank_has_parse_paths() = false;

	ParsePathRandom.seed(seed);

	list<State> new_sentences;

	this->start();
	unsigned production_cnt = 0;
	while (!this->is_done()) {
		Parser parser = this->next_sentence();
		/// Consistently parse the sentence.
		parser.parse(true);
		production_cnt += parser.parse_state().path().size();
		new_sentences.push_back(parser.parse_state());
	}
	assert(sentences.size() == new_sentences.size());

	// Copy the new paths over.
	sentences = new_sentences;

//	parameter::treebank_has_parse_paths() = true;

	Debug::log(1) << "Treebank::choose_new_parse_paths() drew new sample set, with " << production_cnt << " productions.\n";
}

/// Choose new parse paths, using the Classifier and the oracle.
/// i.e. Draw a new sample set from the treebank.
/// \param seed Seed for the random number generator.
void Treebank::choose_new_parse_paths(Classifier& classifier, unsigned long seed) {
	assert(parameter::treebank_has_parse_paths() == false);

//	/// \todo FIXME: The following doesn't feel kosher
//	parameter::treebank_has_parse_paths() = false;

	ParsePathRandom.seed(seed);

	list<State> new_sentences;

	this->start();
	unsigned production_cnt = 0;
	unsigned cnt = 0;
	while (!this->is_done()) {
		Parser parser = this->next_sentence();
		parser.set_classifier(&classifier);
		parser.parse_with_oracle();
		production_cnt += parser.parse_state().path().size();
		new_sentences.push_back(parser.parse_state());
		cnt++;

		if (cnt % 100 == 0)
			Debug::log(1) << "\nParsed " << cnt << " trees, oracle was invoked for " << 100.*(stats::total_actions()-stats::correct_actions())/stats::total_actions() << "% (" << stats::total_actions()-stats::correct_actions() << "/" << stats::total_actions() << ") of decisions\n";
		else if (cnt % 10 == 0)
			Debug::log(2) << "\nParsed " << cnt << " trees, oracle was invoked for " << 100.*(stats::total_actions()-stats::correct_actions())/stats::total_actions() << "% (" << stats::total_actions()-stats::correct_actions() << "/" << stats::total_actions() << ") of decisions\n";
		else
			Debug::log(3) << "\nParsed " << cnt << " trees, oracle was invoked for " << 100.*(stats::total_actions()-stats::correct_actions())/stats::total_actions() << "% (" << stats::total_actions()-stats::correct_actions() << "/" << stats::total_actions() << ") of decisions\n";

	}
	assert(sentences.size() == new_sentences.size());

	// Copy the new paths over.
	sentences = new_sentences;

//	parameter::treebank_has_parse_paths() = true;

	Debug::log(1) << "Treebank::choose_new_parse_paths() drew new sample set, with " << production_cnt << " productions.\n";
	Debug::log(1) << "Oracle was invoked for " << 100.*(stats::total_actions()-stats::correct_actions())/stats::total_actions() << "% (" << stats::total_actions()-stats::correct_actions() << "/" << stats::total_actions() << ") of decisions\n";
}

/// Convert the treebank to a string.
/// Iff !parameter::treebank_has_parse_paths(), we output a Penn-style
/// treebank.
/// Iff parameter::treebank_has_parse_paths(), we output a Penn-style
/// treebank and parse paths.
/// \todo We can do this by iterating directly over ::sentences.
const string Treebank::to_string() {
	this->start();
	string s;
	while (!this->is_done()) {
		Parser parser = this->next_sentence();
		/// Consistently parse the sentence.
		parser.parse(true);
		assert(parser.parse_state().to_string() == parser.gold_state().to_string());
		s += parser.parse_state().to_string() + "\n";
		if (parameter::treebank_has_parse_paths())
			s += parser.parse_state().path().to_string() + "\n";
	}
	return s;

}

/// Start retrieving sentences.
/// \todo Allow random order, i.e. shuffled sentences, instead of treebank order?
/// \todo Allow more than one pass over the examples?
/// \todo Assert that sentence_iterator is invalid.
void Treebank::start() {
	if (has_started)
		assert(this->is_done());
	else
		has_started = true;

	assert(!sentences.empty());
	sentence_iterator = sentences.begin();
	assert(!this->is_done());
}

/// Stop retrieving sentences.
/// This must be called if sentence retrieval is stopped prematurely
/// and then restarted. Otherwise, the Treebank will not be closed.
void Treebank::stop() {
	assert(has_started);
	assert(!this->is_done());
	sentence_iterator = sentences.end();
	assert(this->is_done());
}

/// Are we done retrieving sentences?
/// \todo Assert that sentence_iterator is valid, i.e. that
/// Treebank::start() has been called.
bool Treebank::is_done() const {
	assert(has_started);
	return (sentence_iterator == sentences.end());
}

/// Retrieve the next sentence in the treebank.
/// \todo Slow to return Parser instance?
/// \invariant !is_done()
/// \todo Assert that sentence_iterator is valid, i.e. that
/// Treebank::start() has been called.
/// \todo Skip sentences that have too few words!
const Parser Treebank::next_sentence() {
	assert(has_started);
	assert(!this->is_done());
	Parser p(*sentence_iterator);
	sentence_iterator++;
	return p;
}

/// Sample a sentence from the treebank.
/// Each sentence is weighted by the number of constituents therein.
/// \param include_top True iff TOP should be included in the constituents count. (Default: false)
const Parser Treebank::sample_sentence(bool include_top) {
	unsigned tot;
	if (include_top) tot = _total_constituents_including_TOP;
	else tot = _total_constituents_excluding_TOP;

	// Sample a state
	float x = drand48();
	unsigned cnt = 0;
	unsigned sze = 0;
	unsigned i = 0;
	this->start();
	while(!this->is_done()) {
		i++;

//		Parser parser = this->next_sentence();
//		const Path& p = parser.gold_state().path();

		list<State>::iterator last_sentence = sentence_iterator;
		assert(has_started);
		assert(!this->is_done());
		const Path& p = last_sentence->path();
		sentence_iterator++;

		assert(p.back().action().label() == Label_TOP());
		assert(p.size() >= 1);
		if (include_top) sze = p.size();
		else sze = p.size() - 1;
		cnt += sze;
		if (x < (float)cnt/tot) {
			if (!this->is_done()) this->stop();
			Debug::log(3) << "Sampled sentence #" << i << " (weight=" << sze << ")\n";
			//return parser;

			return Parser(*last_sentence);
		}
	}
	// We should never reach here.
	assert(0);
}
