/*  $Id: parser-with-hypothesis-parameters.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file parser-with-hypothesis-parameters.C
 *  \brief Parser with hypotheses given as command-line parameters.
 *
 *  Unlike the standard parser executable (which ties the label
 *  classifiers by l1 penalty or some such), this parser allows one to list
 *  explicitly the hypothesis files on the command-line. They can appear
 *  in any order.
 *
 *  \todo Allow user to choose whether parse40 or parse all, given command line parameters.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "learn/Classifier.H"
#include "parse/Parser.H"
#include "parse/Treebank.H"

#include "universal/globals.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/stats.H"

#include <cstdlib>

int main(int argc, char** argv) {
	stats::stats();

	if (argc < 3) {
		cerr << "Incorrect call.\nUSAGE: ./parser-with-hypothesis-parameters treebank parameters_file [hypothesis_files ...]\n";
		assert(0);
	}

	Debug::open();

	parameter::load_file(argv[2]);
	parameter::set_treebank_has_parse_paths(false);
	parameter::set_cache_example_confidences(false);
	Debug::log(1) << "\n";
	Debug::log(1) << "####################################\n";
	Debug::log(1) << "# Global parameters:\n";
	Debug::log(1) << parameter::to_string("\t");
	Debug::log(1) << "\n\n";

	LIST<string> hypotheses;
	for (int i = 3; i < argc; i++) {
		hypotheses.push_back(argv[i]);
	}

	Debug::log(1) << "Opening treebank: " << argv[1] << "\n";

	srand48(parameter::seed());

	ios::sync_with_stdio(false);	// Do not keep C and C++ streams in sync

	// Read in the word list
	read_words(parameter::vocabulary_filename());

	// Read in the label list
	read_labels(parameter::label_filename());

	Classifier classifier;
	classifier.load(hypotheses);

	Treebank treebank(argv[1]);
	treebank.start();

	unsigned cnt = 0;
	while(!treebank.is_done()) {
		Parser parser = treebank.next_sentence();
		parser.set_classifier(&classifier);
		parser.parse();
		cout << parser.parse_state().to_string() << "\n";
		cout.flush();
		cnt++;
		if (cnt % 10 == 0)
			Debug::log(1) << "Parsed " << cnt << " trees...\n";
		else
			Debug::log(3) << "Parsed " << cnt << " trees...\n";
	}
	Debug::log(1) << "Total of " << cnt << " trees parsed.\n";

	return 0;
}
