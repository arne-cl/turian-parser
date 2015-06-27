/*  $Id: AbstractExample.C 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file AbstractExample.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/examples/AbstractExample.H"

#include "learn/loss.H"

#include "universal/parameter.H"

#include <cmath>

AbstractExample::AbstractExample()
	: _initial_weight(1), _confidence(0), _have_confidence(false), _have_is_correct(false)
{
	this->sanity_check();
}

AbstractExample::AbstractExample(bool is_correct, float weight)
	: _initial_weight(weight), _confidence(0), _have_confidence(false), _is_correct(is_correct), _have_is_correct(true)
{
	this->sanity_check();
}

void AbstractExample::sanity_check() const {
	assert(_initial_weight > 0);
}

void AbstractExample::set_confidence(const double& confidence) {
	this->sanity_check();
	_confidence = confidence;
	_have_confidence = true;
}

/// Set the weight of this AbstractExample.
/// If _have_confidence, then set _initial_weight such that this->weight() == weight.
/// Otherwise, set _initial_weight to weight.
void AbstractExample::set_weight(const double& weight) {
	this->sanity_check();
	if (_have_confidence) {
		_initial_weight *= weight / this->weight();
		assert(fabs(this->weight() - weight) < parameter::small_epsilon());
	} else {
		_initial_weight = weight;
	}
}

/// \pre _have_confidence
double AbstractExample::loss() const {
	this->sanity_check();
	assert(_have_confidence);
	return loss_of(this->margin()) * _initial_weight;
}

/// Find the loss of the AbstractExample, given a confidence change dconf.
/// \pre _have_confidence
double AbstractExample::loss_dconf(double dconf) const {
	this->sanity_check();
	assert(_have_confidence);
	return loss_of(margin_of(this->confidence() + dconf, this->is_correct())) * _initial_weight;
}

double AbstractExample::weight() const {
	this->sanity_check();
	assert(_have_confidence);
	return _initial_weight * weight_of(this->margin());
}

/// \pre _have_confidence
double AbstractExample::margin() const {
	this->sanity_check();
	assert(_have_confidence);
	return margin_of(this->confidence(), this->is_correct());
}

/// \pre _have_confidence
double AbstractExample::confidence() const {
	this->sanity_check();
	assert(_have_confidence);
	return _confidence;
}
