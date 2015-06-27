/*  $Id: tune-classifiers.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file tune-classifiers.C
 *  \brief Optimize the classifiers.
 *
 *  WRITEME.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "learn/decision-trees/Ensemble.H"

#include "learn/examples/FullExample.H"
#include "learn/examples/Generator.H"

#include "learn/Checkpoint.H"
#include "learn/Classifier.H"

#include "universal/globals.H"
#include "universal/stats.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/Archive.H"
#include "universal/parameter.H"

//#include "universal/newparameter.H"

#include <cassert>

void tune(Label label, string treebank_file) {
	Checkpoint::instance()->construct(label);
	Debug::log(2) << stats::resource_usage() << "\n";

	LIST<pair<Ensemble, double> > ensembles;

	// Load the last ensemble checkpointed
	// for this label.
	Checkpoint::instance()->load(ensembles, 0.);
	// After reading in the label's hypothesis file , free some memory.
	Features::instance()->free_string_map();

	// Read in the examples
	// Keep this variable around, since it stores the treebank.
	Generator exmpls(treebank_file, label);

	for (LIST<pair<Ensemble, double> >::const_iterator e = ensembles.begin();
			e != ensembles.end(); e++) {
		const Ensemble& ensemble = e->first;
		const double& l1 = e->second;
		if (ensemble.is_tuned()) continue;
		parameter::set_l1_penalty_factor(l1);
		ensemble.tune(exmpls);
		Debug::log(2) << "l1 = " << l1 << ", " << ensemble.size() << " trees\n";
		Debug::log(2) << stats::resource_usage() << "\n";
		Checkpoint::instance()->save(ensemble);
	}
}

int main(int argc, char **argv) {
	stats();

	ios::sync_with_stdio(false);	// Do not keep C and C++ streams in sync

	Label label;
	string treebank_file;

	try {
		parameter::read_label_and_treebank(label, treebank_file, argc, argv);
	} catch (exception& e) {
		cerr << e.what() << "\n";
		assert(0);
	}

	parameter::set_l1_penalty_factor(parameter::initial_l1_penalty_factor());

	try {
		tune(label, treebank_file);
	} catch (string s) {
		cerr << "CAUGHT EXCEPTION: " << s << "\n";
		cerr << "ABORTING...\n";
		cerr << stats::resource_usage() << "\n";
		return -1;
	}


	Debug::log(1) << "\n\nDONE TRAINING.\n";

	return 0;
}
