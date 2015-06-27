/*  $Id: count-errors.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file count-errors.C
 *  \brief Find the 0/1 error and the loss of all the hypotheses in a checkpoint.
 *
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "learn/Checkpoint.H"
#include "learn/Ensemble.H"
#include "learn/Classifier.H"
#include "learn/loss.H"

#include "universal/globals.H"
#include "universal/stats.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"

#include <cassert>

/// Find the 0/1 error and the loss of all the hypotheses in a checkpoint.
/// \param label The label which ensemble we are find-0-1-erroring.
/// \param treebank_file The treebank containing the find-0-1-erroring sentences.
void count_errors(Label label, char* treebank_file) {
	Checkpoint::instance()->construct(label);

	Classifier classifier;
	Ensemble ensemble;

	Checkpoint::instance()->load(ensemble);

	// Open an example source
	vector<Example> Generator::Generator(treebank_file, ensemble, label);

	exmpls.start();

	Example e;
	unsigned totcnt = 0;
	vector<unsigned> goodcnt(ensemble.trees().size(), 0);
	vector<double> loss(ensemble.trees().size(), 0.);
	while(!exmpls.is_done()) {
		exmpls.get(e);

	        LIST<Tree>::const_iterator t;
		double conf = 0;
		unsigned cnt;
		for (t = ensemble.trees().begin(), cnt = 0; t != ensemble.trees().end(); t++, cnt++) {
			conf += t->confidence(e);

			double margin = margin_of(conf, e.is_correct());
			if (margin > 0) goodcnt.at(cnt)++;
			loss.at(cnt) += loss_of(margin);
		}

		assert(parameter::noise() == 0);

		totcnt++;
		if (totcnt % 100000 == 0)
		Debug::log(2) << "\tRead " << totcnt << " examples in Tree::weight()\n";
		else if (totcnt % 10000 == 0)
			Debug::log(3) << "\tRead " << totcnt << " examples in Tree::weight()\n";
		else if (totcnt % 1000 == 0)
			Debug::log(4) << "\tRead " << totcnt << " examples in Tree::weight()\n";
	}

	for (unsigned cnt = 0; cnt < goodcnt.size(); cnt++) {
//		cout << i->second << " " << 100.*goodcnt/totcnt << "% good\n";
		cout << "#trees = " << cnt << ", " << 100.*goodcnt.at(cnt)/totcnt << "% good, " << goodcnt.at(cnt) << " of " << totcnt << ", loss = " << loss.at(cnt) << "\n";
	}
}

int main(int argc, char **argv) {
	stats::stats();
	ios::sync_with_stdio(false);	// Do not keep C and C++ streams in sync

	Label label;
	string treebank_file;

	po::options_description local("Parameters");
	local.add_options()
		("label,l", po::value<Label>(&label),
		 "label to count errors (given numerically)")
		("treebank_file,t", po::value<string>(&treebank_file),
		 "file from which to read the treebank")
		;

	try {
		const po::variables_map& vm = parameter::read(local, argc, argv);
	} catch (exception& e) {
		cerr << e.what() << "\n";
		assert(0);
	}

	// Make sure all required parameters were set
	// NB This happens automatically if ->default_value() is used.
	assert(vm.count("label"));
	assert(vm.count("treebank_file"));

	assert(vm["label"].as<Label>() == label);
	assert(vm["treebank_file"].as<string>() == treebank_file);

	srand48(parameter::seed());

	// Read in the word list
	read_words(parameter::vocabulary_filename());

	// Read in the label list
	read_labels(parameter::label_filename());

	assert(is_constituent_label(label));

	Debug::open();
	Debug::log(1) << "\n";
	Debug::log(1) << "####################################\n";
	Debug::log(1) << "# Global parameters:\n";
	Debug::log(1) << parameter::to_string("\t");
	Debug::log(1) << "\n\n";
	Debug::log(1) << "\nCounting errors for " << label_string(label) << " (#" << label << ") classifier.\n";

	count_errors(label, treebank_file);

	Debug::log(1) << "\n\nDONE TRAINING.\n";

	return 0;
}
