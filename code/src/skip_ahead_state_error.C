/*  $Id: skip_ahead_state_error.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file skip_ahead_state_error.C
 *  \brief Estimate the error in random skip-ahead states.
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

	unsigned cnt = 0;
	unsigned rclcnt = 0;
	unsigned prccnt = 0;

	/// \todo Make this a parameter
	unsigned passes = 1000;
	for (unsigned i = 0; i < passes; i++) {
		Parser parser = treebank.sample_sentence();
		parser.set_classifier(&classifier);
		pair<bool, bool> p = parser.skip_ahead_state_error();
		Debug::log(3) << "dPRC = " << p.first << "\n";
		if (!(parameter::parse_right_to_left() || parameter::parse_left_to_right()))
			Debug::log(3) << "dRCL = " << p.second << "\n";
		rclcnt += p.first;
		prccnt += p.second;
		cnt++;
		if (cnt % 10 == 0) {
			Debug::log(2) << "LPRC (derivation-sensitive labelled-bracketing accuracy)   " << 100.*prccnt/cnt << "% (" << prccnt << " / " << cnt << ") trees...\n";
			if (!(parameter::parse_right_to_left() || parameter::parse_left_to_right()))
				Debug::log(2) << "LRCL (derivation-insensitive labelled-bracketing accuracy) " << 100.*rclcnt/cnt << "% (" << rclcnt << " / " << cnt << ") trees...\n";
		} else {
			Debug::log(3) << "LPRC (derivation-sensitive labelled-bracketing accuracy)   " << 100.*prccnt/cnt << "% (" << prccnt << " / " << cnt << ") trees...\n";
			if (!(parameter::parse_right_to_left() || parameter::parse_left_to_right()))
				Debug::log(3) << "LRCL (derivation-insensitive labelled-bracketing accuracy) " << 100.*rclcnt/cnt << "% (" << rclcnt << " / " << cnt << ") trees...\n";
		}
	}
	Debug::log(1) << "LPRC (derivation-sensitive labelled-bracketing accuracy)   " << 100.*prccnt/cnt << "% (" << prccnt << " / " << cnt << ") trees...\n";
	if (!(parameter::parse_right_to_left() || parameter::parse_left_to_right()))
		Debug::log(1) << "LRCL (derivation-insensitive labelled-bracketing accuracy) " << 100.*rclcnt/cnt << "% (" << rclcnt << " / " << cnt << ") trees...\n";

	return 0;
}
