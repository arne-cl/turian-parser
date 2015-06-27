/*  $Id: Weights.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Weights.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/Weights.H"
#include "learn/loss.H"

#include "universal/Debug.H"
#include "universal/parameter.H"

#include <cmath>

Weights::Weights() { }

Weights::Weights(const Double& neg, const Double& pos) {
	_w.first = neg;
	_w.second = pos;
}

Weights Weights::operator-(const Weights& w) const {
	return Weights(_w.first - w._w.first, _w.second - w._w.second);
}

/// Positive weight stored.
const Double& Weights::pos() const {
	return _w.second;
}

/// Negative weight stored.
const Double& Weights::neg() const {
	return _w.first;
}

/// Add weight.
/// \param w The weight to be added.
/// \param is_correct Whether we should add positive or negative example weight.
void Weights::add(const Double& w, bool is_correct) {
	if (is_correct) _w.second += w;
	else _w.first += w;
}

/// The sum of the positive and negative weight herein.
Double Weights::total() const {
	return this->pos() + this->neg();
}


/// The absolute difference between the positive and negative weight.
Double Weights::absolute_difference() const {
	if (this->pos() > this->neg())
		return this->pos() - this->neg();
	else
		return this->neg() - this->pos();
}

/// Clip the gain using the l1 penalty factor.
Double Weights::penalized_gain() const {
	return (this->gain() > parameter::l1_penalty_factor()) ? this->gain() : 0;
}
