/*  $Id: parser-search-greedy-completion.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file parser-search.C
 *  \brief Parse the treebank, using search over additive loss.
 *
 *  \todo Prune the #include list.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "parse/Parser.H"
#include "parse/Treebank.H"

#include "learn/Classifier.H"

#include "universal/globals.H"
#include "universal/vocabulary.H"
#include "universal/stats.H"
#include "universal/Debug.H"

#include <cstdlib>

int main(int argc, char** argv) {
	stats::stats();

	ios::sync_with_stdio(false);	// Do not keep C and C++ streams in sync

	string treebank_file;
	double min_l1;
	try {
		parameter::read_treebank_maybe_min_l1(treebank_file, min_l1, argc, argv);
	} catch (exception& e) {
		cerr << e.what() << "\n";
		assert(0);
	}


	Classifier classifier;
	classifier.load(min_l1);

	Treebank treebank(treebank_file);
	treebank.start();

	unsigned cnt = 0;
	Debug::log(2) << stats::resource_usage() << "\n";
	while(!treebank.is_done()) {
		Parser parser = treebank.next_sentence();
		parser.set_classifier(&classifier);
		//cout << parser.to_string() << "\n";
		//parser.parse();
		//parser.parse_search();
		parser.parse_search_greedy_initial();
		cout << parser.parse_state().to_string() << "\n";
		cout.flush();
		cnt++;
		if (cnt % 10 == 0) {
			Debug::log(1) << "Parsed " << cnt << " trees at " << stats::current_time() << "...\n";
			Debug::log(1) << stats::resource_usage() << "\n\n";
		} else {
			Debug::log(3) << "Parsed " << cnt << " trees at " << stats::current_time() << "...\n";
			Debug::log(3) << stats::resource_usage() << "\n\n";
		}
	}
	Debug::log(1) << "Total of " << cnt << " trees parsed at " << stats::current_time() << ".\n";
	Debug::log(1) << stats::resource_usage() << "\n";

	return 0;
}
