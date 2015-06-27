/*  $Id: make-treepaths-with-parser-and-oracle.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file make-treepaths-with-parser-and-oracle.C
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
#include "learn/Classifier.H"

#include "universal/globals.H"
#include "universal/vocabulary.H"
#include "universal/stats.H"
#include "universal/Debug.H"

#include <cassert>

int main(int argc, char **argv) {
	stats();

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

	// Choose a new example set for this new hypothesis.
	treebank.choose_new_parse_paths(classifier, parameter::seed());

	parameter::set_treebank_has_parse_paths(true);
	cout << treebank.to_string();

	return 0;
}
