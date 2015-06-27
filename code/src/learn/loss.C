/*  $Id: loss.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file loss.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/loss.H"

#include "learn/examples/Example.H"
#include "learn/examples/FullExample.H"
#include "learn/examples/Examples.H"

#include "learn/Weights.H"

#include "universal/parameter.H"
#include "universal/Debug.H"

#include <cmath>

/// Find the margin of a prediction.
/// \param confidence The confidence of the prediction.
/// \param is_correct Whether the prediction is correct or not.
/// \return The margin of this prediction.
double margin_of(double confidence, bool is_correct) {
	return confidence * (is_correct ? 1 : -1);
}

/// Convert a margin to a loss.
/// \todo Are these conf->loss conversions correct?
/// \param margin The margin of the prediction.
/// \return The loss of this example.
double loss_of(double margin) {
	switch (parameter::loss_type()) {
		case EXPONENTIAL_LOSS:
			return exp(-margin);
			break;
		case LOGISTIC_LOSS:
			return log(1+exp(-margin));
			break;
		default:
			assert(0);
	}
	assert(0); return 0;
}

/// Convert an example's margin to a weight.
/// \todo Are these margin->weight conversions correct?
/// \param margin The margin of the example.
/// \return The weight of this example.
double weight_of(double margin) {
	switch (parameter::loss_type()) {
		case EXPONENTIAL_LOSS:
			return exp(-margin);
		case LOGISTIC_LOSS:
			// Based upon Schapire et al (2002):
			// "Incorporating Prior Knowledge Into Boosting"
			return 1./(1. + exp(margin));
		default:
			assert(0);
			return 0;
	}
}



/// Compute the penalized loss for some confidence over some set of examples,
/// potentially updating Z_cache and the number of cache_misses.
template<typename EXAMPLE> static double Z(double confidence, const ExamplePtrs<EXAMPLE>& examples, hash_map<double, double>& Z_cache) {
	if (Z_cache.find(confidence) != Z_cache.end()) {
		Debug::log(5) << "CACHE HIT\n";
		return Z_cache[confidence];
	}

	double penalty = fabs(confidence) * parameter::l1_penalty_factor();

	double unpenalized_loss = 0;
	for (typename ExamplePtrs<EXAMPLE>::const_iterator e = examples.begin(); e != examples.end(); e++)
		unpenalized_loss += (*e)->loss_dconf(confidence);

	double total_loss = unpenalized_loss + penalty;

	Z_cache[confidence] = total_loss;

	Debug::log(3) << "Z(" << confidence << ") = " << total_loss << " = " << unpenalized_loss << " (unpenalized loss) + " << penalty << " (penalty)\n";
	return total_loss;
}

/// Choose the delta confidence update that minimizes penalized loss
/// over this set of examples.
/// We do this with a line search.
template<typename EXAMPLE> double minimize_loss(const ExamplePtrs<EXAMPLE>& examples) {
	hash_map<double, double> Z_cache;

	// We only do normalization during decision tree building,
	// so the parameter value is irrelevant here.
	//assert(!parameter::normalize_feature_counts());

	Weights w;
	for (typename ExamplePtrs<EXAMPLE>::const_iterator e = examples.begin(); e != examples.end(); e++)
		w.add((*e)->weight(), (*e)->is_correct());
	// If the gain does not beat the l1 penalty, then just return
	// confidence 0.
	// \todo Sanity-check that we get confidence 0 if we do the work.
	if (w.gain()() <= parameter::l1_penalty_factor())
		return 0;

	int sign = +1;
	if (w.pos() < w.neg()) sign = -1;	// The confidence will be negative

	double step = 1;
	double c = 0;
	while (fabs(step) > parameter::confidence_epsilon()) {
		if (Z(c, examples, Z_cache) < Z(c+step, examples, Z_cache)) {
			step *= -0.5;
		}
		c += step;

		// Make sure that the confidence has the correct sign
		if (sign * c < 0) {
			c = 0;
			step *= -0.5;
		}
	}

	double best_conf = 0;
	double best_Z = Z(0, examples, Z_cache);
	assert(!Z_cache.empty());
	for (hash_map<double, double>::const_iterator z = Z_cache.begin();
			z != Z_cache.end(); z++) {
		if (best_Z > z->second) {
			best_conf = z->first;
			best_Z = z->second;
		}
	}

	double penalty = fabs(best_conf) * parameter::l1_penalty_factor();
	double unpenalized_loss = best_Z - penalty;
	assert(unpenalized_loss >= 0);

	Debug::log(3) << "Best conf " << best_conf << " has penalized loss " << best_Z << " = " << unpenalized_loss << " (unpenalized loss) + " << penalty << " (penalty)\n";
	Debug::log(3) << Z_cache.size() << " cache misses in line search\n";

	if (best_conf == 0) {
		cerr << "\n\n\nWARNING! best_conf = 0!!!!!!!!!!!\n";
		cerr << w << "\n";
		for (hash_map<double, double>::const_iterator z = Z_cache.begin();
				z != Z_cache.end(); z++)
			cerr << z->first << " confidence has loss " << z->second << "\n";
		cerr << "\n";
	}


	return best_conf;
}

// TEMPLATE INSTANTIATIONS
template double minimize_loss<Example>(ExamplePtrs<Example> const&);
template double minimize_loss<SparseFullExample>(ExamplePtrs<SparseFullExample> const&);
