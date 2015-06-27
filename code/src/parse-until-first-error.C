/*  $Id: parse-until-first-error.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file parse-until-first-error.C
 *  \brief Parses until first error.
 *
 *  \todo Prune the #include list.
 *  \todo Choose a classifier.
 *  \todo Allow inconsistency.
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
	unsigned errcnt = 0;
	while(!treebank.is_done()) {
		Parser parser = treebank.next_sentence();
		parser.set_classifier(&classifier);
		//cout << parser.to_string() << "\n";

		Parser consistent_parser = parser;
		consistent_parser.parse(true);
		string orig_string = consistent_parser.parse_state().to_string();
		if (parser.parse_until_first_error()) {
			assert(parser.parse_state().to_string() != parser.gold_state().to_string());
			cout << parser.parse_state().to_string() << "\n";
//			cout << parser.consistent_state().to_string() << "\n";
//			cout << parser.gold_state().to_string() << "\n";
//			cout << orig_string << "\n";
//			cout << "\n";
			cout.flush();
			errcnt++;
		} else {
			assert(parser.parse_state().to_string() == parser.gold_state().to_string());
			cout << "\n";
		}

		cnt++;
		if (cnt % 10 == 0)
			Debug::log(1) << "Parsed " << cnt << " trees (" << 100.*errcnt/cnt << "% of sentences have some error)...\n";
		else
			Debug::log(3) << "Parsed " << cnt << " trees (" << 100.*errcnt/cnt << "% of sentences have some error)...\n";
	}
	Debug::log(1) << "Total of " << cnt << " trees parsed (" << 100.*errcnt/cnt << "% of sentences have some error).\n";

	return 0;
}
