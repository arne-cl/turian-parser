/*  $Id: Example.C 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Example.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/examples/Example.H"

#include "learn/Weights.H"
#include "learn/loss.H"

#include "parse/Action.H"
#include "parse/State.H"
#include "parse/ItemToken.H"

#include "universal/Debug.H"
#include "universal/stats.H"

Example::Example(const State* state, const Action& action)
	: AbstractExample(), _state(state)
{
	this->construct(action);
	this->sanity_check();
}

Example::Example(const State* state, const Action& action, bool is_correct, float weight)
	: AbstractExample(is_correct, weight), _state(state)
{
	this->construct(action);
	this->sanity_check();
}

void Example::sanity_check() const {
	assert(_state);
	this->AbstractExample::sanity_check();
}

void Example::construct(const Action& action) {
	assert(_state);
	const pair<unsigned, unsigned> p = _state->frontier().indices_of(action);
	unsigned head = ItemToken(action).head();

	_start = p.first;
	_end = p.second;
	_head = head;

	// Make sure we don't lose any information during type conversion.
	assert(_start == p.first);
	assert(_end == p.second);
	assert(_head == head);
	assert(_start <= _start + _head && _start + _head <= _end);
}

/// From which sentence is this Example derived?
const ID<Sentence>& Example::sentence() const {
	this->sanity_check();
	return this->state().sentence();
}

const ItemTokens& Example::frontier() const {
	this->sanity_check();
	return this->state().frontier();
}
