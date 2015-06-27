/*  $Id: decompose-action-confidence.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file decompose-action-confidence.C
 *  \brief Explain why an action in some state gets the confidence it does.
 *
 *  WRITEME.
 *
 *  \todo Put information about having to read actions from stdin in a
 *  more noticeable message.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "learn/examples/Example.H"

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

	string tree_file;
	unsigned step_number;
	double min_l1 = 0;

	Debug::open();

	/// \todo Put information about having to read actions from stdin in a
	/// more noticeable message.
	po::options_description local("Parameters for decompose-action-confidence (reading actions from stdin)");
	local.add_options()
		("tree_file,t", po::value<string>(&tree_file),
		 "file from which to read the tree whose action decompose")
		("step_number,s", po::value<unsigned>(&step_number),
		 "number of steps to perform")
		("min_l1", po::value<double>(&min_l1),
		 "minimum l1 penalty for any classifier hypothesis [optional]")
		;

	try {
		const po::variables_map& vm = parameter::read(local, argc, argv);

		// Make sure all required parameters were set
		// NB This happens automatically if ->default_value() is used.
		assert(vm.count("tree_file"));
		assert(vm.count("step_number"));

		assert(vm["tree_file"].as<string>() == tree_file);
		assert(vm["step_number"].as<unsigned>() == step_number);
		assert(vm["min_l1"].as<double>() == min_l1);

		if (vm.count("min_l1")) {
			assert(min_l1 >= 0);
			Debug::log(1) << "Parsing with min l1 = " << min_l1 << "\n";
		} else {
			Debug::log(1) << "Parsing with indefinite number of iterations\n";
		}
	} catch (exception& e) {
		cerr << e.what() << "\n";
		assert(0);
	}

	parameter::set_treebank_has_parse_paths(false);
	parameter::set_cache_example_confidences(false);
	assert(!parameter::checkpoint_steps());

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


	Classifier classifier;
	classifier.load(min_l1);

	Treebank treebank(tree_file);
	treebank.start();
	assert(treebank.size() == 1);

	while(!treebank.is_done()) {
		Parser parser = treebank.next_sentence();
		parser.set_classifier(&classifier);
		parser.parse_steps(step_number - 1);
		assert(!parser.parse_state().is_final());
		Debug::log(0) << "Advanced to step #" << step_number << ":\n";
		Debug::log(0) << parser.parse_state().to_string() << "\n";

		parameter::set_trace_confidence_paths(true);
		while (!cin.eof()) {
			string lbl, tmp1, tmp2;
			unsigned left, right;
			cin >> lbl >> tmp1 >> left >> tmp2 >> right >> ws;
			assert(tmp1 == "<-");
			assert(tmp2 == "..");

			Span span(left, right);
			Label l = string_to_label(lbl);
			Action action(l, parser.parse_state().locate_span(span));

			Debug::log(0) << "Decomposing confidence of action " << action.to_string() << "\n";
			Example e(parser.parse_state(), action);

			double conf = classifier.confidence(action.label(), e);
			Debug::log(0) << "Action " << action.to_string() << " has total confidence = " << conf << "\n";

			/*
			   Debug::log(0) << "\n\nExample features:\n";
			   FeatureIDs flist = Features::instance()->get(e);
			   for (FeatureIDs::iterator f = flist.begin(); f != flist.end(); f++)
			   Debug::log(0) << Features::instance()->from_id(*f).to_string() << "\n";
			   */
		}
		parameter::set_trace_confidence_paths(false);
	}
}
