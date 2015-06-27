/*  $Id: Classifier.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Classifier.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Classifier
 *  \brief Assigns confidence values to candidate inferences.
 *
 *  A classifier can evaluate any candidate Inference,
 *  i.e.\ a Action, State tuple which is represented as an Example not needing a y-value.
 *
 *  A classifier is used for all Inference%s, regardless of their Label%s.
 *  It stores different Ensemble%s, one for each constituent Label.
 *
 */

#ifndef __Classifier_H__
#define  __Classifier_H__

#include "parse/Action.H"
#include "parse/ItemToken.H"

#include "learn/decision-trees/Ensemble.H"

#include "universal/vocabulary.H"
#include "universal/LIST.H"

#include <map>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

class Example;

class Classifier {
public:
	/// Construct an empty classifier.
	Classifier() { learners.clear(); }

	/// Load a Classifier.
	/// Read one Ensemble for each Label.
	/// \param min_l1 The minimum l1 hypothesis file we can read.
	/// 0 iff we can read an indefinite number of iterations.
	void load(double min_l1=0);

	/// Load a classifier from a list of filenames.
	void load(const LIST<string>& filenames);

	/// Confidence-rate an Inference.
	template<typename EXAMPLE> double confidence(const Label l, const EXAMPLE& e) const;

	/// Do a parallel update over each Ensemble in the Classifier.
	void parallel_update();

	/// Zero all confidences herein.
	void zero_confidences();

private:
	/// Get the Ensemble for a certain Label.
	/// \param l The Label whose Ensemble we wish to retrieve.
	/// \return The Ensemble corresponding to this Label.
	const Ensemble& get_ensemble(const Label l) const;

	/// Set the Ensemble for a certain Label.
	/// \param l The Label whose Ensemble we wish to set,
	/// which Ensemble need not yet exist in the Classifier.
	/// \param e The new Ensemble for this label.
	void set_ensemble(const Label l, const Ensemble& e);

	map<Label, Ensemble> learners;
};

#endif /* __Classifier_H__ */
