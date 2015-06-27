/*  $Id: Weights.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Weights.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Weights
 *  \brief A pair of weights.
 *
 *  Typically, cumulative (total) Example weight that falls into some Node/BuildNode, broken down by y-value.
 *  The first value is negative y-value weight.
 *  The second value is positive y-value weight.
 *
 *  \todo Rename "loss" to "gain" where applicable, to avoid confusion.
 *  \todo Better documentation.
 *
 */

#ifndef __Weights_H__
#define  __Weights_H__

#include "universal/Double.H"

#include <vector>
#include <ostream>

#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class debugstream;
//class ostream;

class Weights {
public:
	Weights();

	Weights(const Double& neg, const Double& pos);

	Weights operator-(const Weights& w) const;

	/// Positive weight stored.
	const Double& pos() const;

	/// Negative weight stored.
	const Double& neg() const;

	/// Add weight.
	/// \param w The weight to be added.
	/// \param is_correct Whether we should add positive or negative example weight.
	void add(const Double& w, bool is_correct);

	/// The sum of the positive and negative weight herein.
	Double total() const;

	/// The absolute difference between the positive and negative weight.
	Double absolute_difference() const;

	Double gain() const { return this->absolute_difference(); }

	/// Clip the gain using the l1 penalty factor.
	Double penalized_gain() const;

	friend ostream& operator<<(ostream &o, const Weights &w) {
		o << "[+" << w.pos()() << ", -" << w.neg()() << "]";
		return o;
	}
	friend debugstream& operator<<(debugstream &o, const Weights &w) {
		o << "[+" << w.pos()() << ", -" << w.neg()() << "]";
		return o;
	}

	/// Get the 2 norm (?) Collins scale.
	/// (sqrt(pos weight) - sqrt(neg weight))^2
	/// \note pg. 272 of Bregman distances paper.
	Double collins_scale() const;

private:
	/// The unpenalized loss of a confidence-rating for these Weights.
	Double unpenalized_loss() const;

	pair<Double, Double> _w;
};

#endif /* __Weights_H__ */
