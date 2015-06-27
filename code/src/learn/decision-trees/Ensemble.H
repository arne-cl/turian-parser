/*  $Id: Ensemble.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Ensemble.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Ensemble
 *  \brief An ensemble of Tree%s.
 *
 *  An Ensemble confidence-rates Example%s with a particular Label.
 *  \warning Any non-const method must reset _use_full_examples to empty!
 *
 *  \todo This should be an abstract-base class. We want subclasses with
 *  virtual methods, templatized by the working (converted) Example type,
 *  as well as perhaps the input Example type.
 *
 */

#ifndef __Ensemble_H__
#define  __Ensemble_H__

#include "universal/Optional.H"	// MOVE THIS DOWN

#include "learn/decision-trees/Tree.H"

#include "universal/LIST.H"
#include "universal/AutoVector.H"

class Checkpoint;
template<typename EXAMPLE> class Examples;

class Ensemble {
public:
	/// Create an empty ensemble.
	Ensemble();

	void clear();

	const string to_string(const string s="") const;

	/// Load an ensemble from a file.
	/// \param filename The file from which to load the ensemble.
	void load(const string filename);

	/// Load an ensemble from a file, and set Example confidences.
	/// \param filename The file from which to load the ensemble.
	/// \param exmpls The Example%s whose confidences should be updated,
	/// and from whom the initial weights should be computed.
	template<typename EXAMPLE> void load(const string filename, Examples<EXAMPLE>& exmpls);

	/// Add a Tree to the ensemble.
	/// \param t The Tree to be added.
	/// \post _use_full_examples.empty()
	void add(const Tree& t);

	/// Set Example confidences.
	/// \param exmpls The Example%s whose confidences should be updated.
	template<typename EXAMPLE> void set_confidences(Examples<EXAMPLE>& exmpls) const;

	/// Sanity check that the initial Weights computed from exmpls
	/// match those read from the hypothesis file in this Ensemble.
	/// \todo It would be much more efficient if we move this to
	/// Ensemble::set_confidences.
	template<typename EXAMPLE> void verify_initial_weights(const Examples<EXAMPLE>& exmpls) const;

	/// Add the tree to the ensemble, choose confidences for
	/// all the tree's leaves using the example set, and update
	/// the example weights.
	/// \param tree The Tree to be added.
	/// \param exmpls The Example%s whose confidences should be updated.
	/// \post _use_full_examples.empty()
	template<typename EXAMPLE> void add_and_update(Tree& tree, Examples<EXAMPLE>& exmpls);
	
	unsigned size() const { return _trees.size(); }

	/// Find all leaf Node%s in this Ensemble.
	LIST<const Node*> leaves() const;

	/// Empirically determine whether it is faster to compute confidences over
	/// DenseFullExample%s or IntermediateExample%s.
	/// We use a sample of EXAMPLES_FOR_AUTOMAGIC_TUNING examples for
	/// this automagic tuning.
	/// \post Set ::_use_full_examples iff DenseFullExample%s are
	/// faster than IntermediateExample%s.
	/// \return The value of ::_use_full_examples.
	/// \todo Compute variance and add that to the mean time, to reduce the
	/// risk of bad worst-case times.
	template<typename EXAMPLE> const Optional<bool>& tune(const Examples<EXAMPLE>& exmpls) const;

	bool is_tuned() const { return !_use_full_examples.empty(); }

	/// Compute the confidence of an Example from scratch.
	/// If the Ensemble has been tuned (using tune()),
	/// we will convert the Example to the appropriate type before
	/// computing its confidence.
	template<typename EXAMPLE> double confidence_compute(const EXAMPLE& e) const;

private:
	/// Determine statistics of this Ensemble and a set of Examples,
	/// and return them in a string.
	template<typename EXAMPLE> string stats(const Examples<EXAMPLE>& exmpls) const;

	/// Compute the penalty over all Tree%s in this Ensemble.
	double penalty() const;


	/// Do an empirical test of how many confidence computations per second
	/// can be performed.
	/// \param test_set The examples to be used for the test.
	/// \param results The confidences computed will be put in this structure.
	/// \param seconds Time to be spent performing the test.
	template<typename EXAMPLE>
		double determine_examples_per_second(const Examples<const EXAMPLE*>& test_set,
				vector<double>& results,
				float seconds=1.*EXAMPLES_TO_CHECK_PER_TIME_CHECK/2) const;

	/// Convert from ORIGEXAMPLE to WORKEXAMPLE before computing confidences.
	template<typename ORIGEXAMPLE, typename WORKEXAMPLE>
		double confidence_compute_convert(const ORIGEXAMPLE& e) const;

	/// Given a Example that has been converted to the correct
	/// Example type, actually do the work of comput its confidence.
	template<typename EXAMPLE> double confidence_compute_work(const EXAMPLE& e) const;

	LIST<Tree> _trees;

	/// True iff DenseFullExample%s are faster than IntermediateExample%s.
	/// False iff IntermediateExample%s are faster than DenseFullExample%s.
	/// If empty, then Ensemble::tune() has not been called.
	/// \post _use_full_examples.empty()
	/// \warning Any non-const method must reset _use_full_examples to empty!
	mutable Optional<bool> _use_full_examples;
};

#endif /* __Ensemble_H__ */
