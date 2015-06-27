/*  $Id: Classifier.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Classifier.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/Classifier.H"

#include "learn/Checkpoint.H"

#include "universal/globals.H"
#include "universal/Debug.H"


/// Get the Ensemble for a certain Label.
/// \param l The Label whose Ensemble we wish to retrieve.
/// \return The Ensemble corresponding to this Label.
const Ensemble& Classifier::get_ensemble(const Label l) const {
	assert(learners.find(l) != learners.end());
	return learners.find(l)->second;
}

/// Set the Ensemble for a certain Label.
/// \param l The Label whose Ensemble we wish to set,
/// which Ensemble need not yet exist in the Classifier.
/// \param e The new Ensemble for this label.
void Classifier::set_ensemble(const Label l, const Ensemble& e) {
	// FIXME: Don't use [] operator
	learners[l] = e;
}


/// Load a Classifier.
/// Read one Ensemble for each Label.
/// \param min_l1 The minimum l1 hypothesis file we can read.
/// 0 iff we can read an indefinite number of iterations.
void Classifier::load(double min_l1) {
	learners.clear();

	LIST<Label>::const_iterator l;
	for (l = all_constituent_labels().begin(); l != all_constituent_labels().end(); l++) {
		Ensemble e;
		Checkpoint::instance()->construct(*l);

		// If no Checkpoint exists for this label, then don't load an empty ensemble.
		if (!Checkpoint::instance()->exists()) continue;

		Checkpoint::instance()->load(e, min_l1);
		this->set_ensemble(*l, e);
	}
}

/// Load a classifier from a list of filenames.
void Classifier::load(const LIST<string>& filenames) {
	string hyp("hypothesis.label-");
	for (LIST<string>::const_iterator f = filenames.begin(); f != filenames.end(); f++) {
		string l = "";
		unsigned i = hyp.length();
		while ((*f)[i] != '.') {
			l += (*f)[i];
			i++;
		}
		Debug::log(2) << label_string(string_to_label(l)) << " label has hypothesis: " << *f << "\n";

		Ensemble e;
		e.load(*f);
		this->set_ensemble(string_to_label(l), e);
	}
}

/// Confidence-rate an Inference.
template<typename EXAMPLE> double Classifier::confidence(const Label l, const EXAMPLE& e) const {
	//if (learners.size() == 0) return 0;
	assert(!learners.empty());

	assert(!e.have_confidence());

	// If no classifier exists for this label, then just give it a very negative confidence.
	if (learners.find(l) == learners.end()) return -99999;

	return this->get_ensemble(l).confidence_compute(e);
}

// TEMPLATE INSTANTIATIONS
template double Classifier::confidence<Example>(unsigned int, Example const&) const;
