/*  $Id: train.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file train.C
 *  \brief Train the parser.
 *
 *  WRITEME.
 *
 *  \todo Allow command-line label parameter to be given either
 *  as a numerical value (e.g. 3) or as a string (e.g. NP)
 *  \todo Somehow jump to the proper point in the checkpoint.
 *  \todo The noise parameter should be adjusted somewhere in train();
 *
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

template<typename EXAMPLE> void train_loop(Ensemble& ensemble, Examples<EXAMPLE>& exmpls) {
	Debug::log(2) << "\n";
	Debug::log(2) << "Training on Examples<" << EXAMPLE::name() << ">...\n";
	Debug::log(2) << "\tFYI, sizeof(" << EXAMPLE::name() << ") = " << sizeof(EXAMPLE) << " bytes\n";

	ensemble.verify_initial_weights(exmpls);
	ensemble.set_confidences(exmpls);

	unsigned iteration = 1;

	// Each iteration of the while() loop corresponds to one iteration
	// of boosting. Specifically, each iteration we add one hypothesis
	// to the ensemble.
	while (iteration <= parameter::max_iterations()) {
		if (parameter::l1_penalty_factor() < parameter::final_l1_penalty_factor()) {
			Debug::log(1) << "\nl1_penalty_factor = " << parameter::l1_penalty_factor() << " is less than minimum " << parameter::final_l1_penalty_factor() << "\n";
			Debug::log(1) << "Terminating training.\n";
			break;
		}

		Debug::log(1) << "\n";
		Debug::log(1) << "Beginning outer loop iteration #" << iteration << " at " << stats::current_time() << "...\n";
		Debug::log(1) << "(l1_penalty_factor currently " << parameter::l1_penalty_factor() << ")\n";

		// Build a decision tree from the example source.
		// Note that the decision tree is unweighted, i.e. the leaf confidences
		// are all zero.
		Tree tree(exmpls);

		if (parameter::l1_penalty_factor() < parameter::final_l1_penalty_factor()) {
			Debug::log(1) << "l1_penalty_factor=" << parameter::l1_penalty_factor() << " < final_l1_penalty_factor="  <<  parameter::final_l1_penalty_factor() << "\n";
			Debug::log(1) << "Saving checkpoint and terminating training...\n";

			// Then save the ensemble to the checkpoint.
			Checkpoint::instance()->save(ensemble);

			Debug::log(1) << "Terminating training...\n";
			break;
		}

		// Add the tree to the ensemble, choose confidences for
		// all the tree's leaves using the example set, and update
		// the example weights.
		ensemble.add_and_update(tree, exmpls);

		Debug::log(1) << "...finished iteration #" << iteration << "\n";
		Debug::log(1) << "\n";

		// Then save the ensemble to the checkpoint.
		Checkpoint::instance()->save(ensemble);

		iteration++;


		Debug::log(2) << stats::resource_usage() << "\n";
	}
}

/// Train an ensemble of classifiers.
/// Train goes on indefinitely, checkpointing at each step of the way.
/// \param label The label which ensemble we are training.
/// \param treebank_file The treebank containing the training sentences.
void train(Label label, string treebank_file) {
	Checkpoint::instance()->construct(label);
	Debug::log(2) << stats::resource_usage() << "\n";

	Ensemble ensemble;

	// Load the last ensemble checkpointed
	// for this label.
	Checkpoint::instance()->load(ensemble, 0.);
	// After reading in the label's hypothesis file , free some memory.
	Features::instance()->free_string_map();

	if (parameter::cache_example_feature_vectors_before_training()) {
		Examples<SparseFullExample> exmpls(Generator(treebank_file, label));
		train_loop(ensemble, exmpls);
	} else {
		// Read in the examples
		// Keep this variable around, since it stores the treebank.
		Generator exmpls(treebank_file, label);
		train_loop(ensemble, exmpls);
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
		train(label, treebank_file);
	} catch (string s) {
		cerr << "CAUGHT EXCEPTION: " << s << "\n";
		cerr << "ABORTING...\n";
		cerr << stats::resource_usage() << "\n";
		return -1;
	}


	Debug::log(1) << "\n\nDONE TRAINING.\n";

	return 0;
}
