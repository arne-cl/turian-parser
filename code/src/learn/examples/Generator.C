/*  $Id: Generator.C 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Generator.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/examples/Generator.H"
#include "learn/examples/Example.H" // REMOVEME

#include "parse/Treebank.H"
#include "parse/Parser.H"

#include "universal/stats.H"


/// Construct a set of Example%s.
/// \param filename The name of the file from which to read the Treebank and set of Example%s.
/// \param target_label Label of desired Example%s.
Generator::Generator(string filename, const Label target_label) {
	assert(is_constituent_label(target_label));

	// Read in the Example%s from disk.
	this->load(filename, target_label);
}



/// Read in a set of Example%s.
/// \param filename The name of the file from which to read the
/// Treebank and set of Example%s.
/// \param target_label Label of desired Example%s.
void Generator::load(string filename, const Label target_label) {
	Debug::log(2) << "Generator::load(filename, target_label) at " << stats::current_time() << "...\n";
	Debug::log(2) << "\tfilename = " << filename << "\n";
	Debug::log(2) << "\ttarget_label = " << label_string(target_label) << "\n";

	// Make sure we haven't read the examples yet.
	assert(this->empty());

	assert(is_constituent_label(target_label));

	// Load the treebank.
	_treebank.read(filename);

/*
	Debug::log(2) << "Reserving " << 1125*_treebank.size() << " cells... " << "\n";
	Debug::log(2) << stats::resource_usage() << "\n";

	// Reserve cells, roughly 1125 per sentence
	this->reserve(1125*_treebank.size());

	Debug::log(2) << "...done" << "\n";
	Debug::log(2) << stats::resource_usage() << "\n";
	*/

	unsigned sentence_cnt = 0;
	_treebank.start();
	while(!_treebank.is_done()) {
		Parser sentence = _treebank.next_sentence();

		//sentence.generate_examples(exmpls, target_label);
		// Insert this sentence's examples in reverse order
		// to mimic old implementation
		Examples<Example> tmp;
		sentence.generate_examples(tmp, target_label);
		this->insert(this->end(), tmp.rbegin(), tmp.rend());

		sentence_cnt++;

		if (sentence_cnt % 100 == 0) {
			Debug::log(3) << "Generator::load() read " << this->size() << " examples so far over " << sentence_cnt << " sentences (capacity=" << this->capacity() << ")\n";
			Debug::log(3) << stats::resource_usage() << "\n";
		} else if (sentence_cnt % 1000 == 0) {
			Debug::log(2) << "Generator::load() read " << this->size() << " examples so far over " << sentence_cnt << " sentences (capacity=" << this->capacity() << ") at " << stats::current_time() << "\n";
			Debug::log(3) << stats::resource_usage() << "\n";
		}
	}
	assert(!this->empty());

/*
	if (this->capacity() > 1125*_treebank.size()) {
		TRACE;
		Debug::log(1) << "\n\n\nWARNING!!!\nVector capacity " << this->capacity() << " exceeds 1125*" << _treebank.size() << "=" << 1125*_treebank.size() << "\n\n\n";
	}
	*/

	// WARNING!
	// If we want to normalize the examples' initial weights,
	// we CAN't just do it over this example set.
	// We also have to include the weights of the examples with
	// different labels!!!
	assert(!parameter::normalize_feature_counts());

	Debug::log(2) << "...Generator::load(filename, target_label) at " << stats::current_time() << ":\n";

	Debug::log(2) << "\tRead " << this->size() << " examples from " << sentence_cnt << " sentences (vector capacity = " << this->capacity() << ")\n";
	Debug::log(2) << "\t(for this target label, mean " << 1.*this->size()/sentence_cnt << " examples per sentence)\n";
//	Debug::log(2) << "\tRead " << example_cnt << " examples from " << state_cnt << " states over " << sentence_cnt << " sentences\n";
//	Debug::log(2) << "\t(for this target label, mean " << 1.*example_cnt/state_cnt << " exmpls per state, " << 1.*state_cnt/sentence_cnt << " states per sentence => " << 1.*example_cnt/sentence_cnt << " exmpls per sentence)\n";

	// Shuffle the order of the Example%s.
	// \todo Use Random instance to choose random numbers for std::random_shuffle.
	Debug::log(2) << "Shuffling examples at " << stats::current_time() << "...\n";
	random_shuffle(this->begin(), this->end());
	Debug::log(2) << "...done shuffling examples at " << stats::current_time() << "\n";

	Debug::log(2) << stats::resource_usage() << "\n";
}
