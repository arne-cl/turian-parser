/*  $Id: parallel-update.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file parallel-update.C
 *  \brief Update confidence-ratings using parallel update.
 *
 *  WRITEME.
 *
 *  \todo Somehow jump to the proper point in the checkpoint.
 *
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "learn/decision-trees/Ensemble.H"
#include "learn/CheckpointSteps.H"
#include "learn/Examples.H"
#include "learn/Classifier.H"

#include "universal/globals.H"
#include "universal/stats.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/parameter.H"

#include <cassert>

/// Train an ensemble of classifiers.
/// Train goes on indefinitely, checkpointing at each step of the way.
/// \param label The label which ensemble we are training.
/// \param treebank_file The treebank containing the training sentences.
/// \todo CLEAR ENSEMBLE WEIGHTS
void parallel_update(Label label, string treebank_file) {
	CheckpointSteps::instance()->construct(label);

	Classifier classifier;
	Ensemble ensemble;

	Examples exmpls;

	// Otherwise, just load the last ensemble checkpointed
	// for this label.
	unsigned steps = CheckpointSteps::instance()->load(ensemble);

	stats::set_steps(steps);
	if (steps == 0) {
		// Clear the confidence values in the ensemble.
		ensemble.zero_confidences();

		// Then save the ensemble to the checkpoint.
		CheckpointSteps::instance()->save(ensemble);
	}

	// Open an example source
	exmpls.load(treebank_file, &ensemble, label);

	while (stats::steps() <= parameter::max_iterations()) {
		Debug::log(1) << "\n";
		Debug::log(1) << "Beginning outer loop iteration #" << stats::steps()+1 << " at " << stats::current_time() << "...\n";

		// Choose a new example set for this new hypothesis.
		parameter::set_treebank_has_parse_paths(false);
		exmpls.choose_new_parse_paths(stats::steps());
		parameter::set_treebank_has_parse_paths(true);

		// Update the ensemble used by the example source to weight examples.
		exmpls.set_ensemble(&ensemble);

		Example e;
		exmpls.start(1);
		unsigned i = 0;
		while(!exmpls.is_done()) {
			exmpls.get(e);
			i++;
			if (i % 100000 == 0)
				Debug::log(2) << "Done with " << i << " examples in iteration #" << stats::steps() << "\n";
			else if (i % 10000 == 0)
				Debug::log(3) << "Done with " << i << " examples in iteration #" << stats::steps() << "\n";
		}

		if (!parameter::parallel_update_based_upon_weight_threshold())
			ensemble.parallel_update();

		Debug::log(1) << "...finished iteration #" << stats::steps() << "\n";
		Debug::log(1) << "\n";

		stats::steps_add();

		// Then save the ensemble to the checkpoint.
		CheckpointSteps::instance()->save(ensemble);
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

	parallel_update(label, treebank_file);

	Debug::log(1) << "\n\nDONE TRAINING.\n";

	return 0;
}
