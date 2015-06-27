/*  $Id: make-treepaths-random.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file make-treepaths-random.C
 *  \brief Choose a random fixed parse path for each tree.
 *
 *  Choose a fixed parse path for each tree (sentence) in the treebank.
 *  This fixes a static example set.
 *
 *  \todo parameters currently do not have any effect on output.
 *  But, we should be able to choose the number of passes over the examples.
 *
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "parse/Treebank.H"

#include "universal/globals.H"
#include "universal/vocabulary.H"
#include "universal/stats.H"
#include "universal/Debug.H"

#include <cassert>

int main(int argc, char **argv) {
	stats::stats();

	ios::sync_with_stdio(false);	// Do not keep C and C++ streams in sync

	string treebank_file;

	po::options_description local("Parameters");
	local.add_options()
		("treebank_file,t", po::value<string>(&treebank_file),
		 "file from which to read the training treebank")
		;

	try {
		const po::variables_map& vm = parameter::read(local, argc, argv);

		// Make sure all required parameters were set
		// NB This happens automatically if ->default_value() is used.
		assert(vm.count("treebank_file"));

		assert(vm["treebank_file"].as<string>() == treebank_file);
	} catch (exception& e) {
		cerr << e.what() << "\n";
		assert(0);
	}

	Debug::open();
	Debug::log(1) << "\n";
	Debug::log(1) << "####################################\n";
	Debug::log(1) << "# Global parameters:\n";
	Debug::log(1) << parameter::to_string("\t");
	Debug::log(1) << "\n\n";

	srand48(parameter::seed());

	// Read in the word list
	read_words(parameter::vocabulary_filename());

	// Read in the label list
	read_labels(parameter::label_filename());

	Treebank treebank(treebank_file);

	// Choose a new example set for this new hypothesis.
	treebank.choose_new_parse_paths(parameter::seed());

	parameter::set_treebank_has_parse_paths(true);
	cout << treebank.to_string();

	Debug::log(1) << "\n\nDONE GENERATING TREEPATHS RANDOMLY.\n";

	return 0;
}
