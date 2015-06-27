/*  $Id: Example.H 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Example.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Example
 *  \brief A single training example which doesn't take up much memory.
 *
 *  A low-memory training example.
 *
 *  \todo Create an Inference class, where an Inference is an Example with no y-value.
 *  \todo Rename as Example.
 *
 */

#ifndef __Example_H__
#define  __Example_H__

#include "learn/examples/AbstractExample.H"

#include "universal/Archive.H"

#include <ostream>
#include <sstream>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class Action;
class Feature;
class ItemTokens;
class State;
class Sentence;
class Weights;
template<typename SENTENCE> class ID;

class Example : public AbstractExample {
//	friend class IntermediateExample;
public:
	Example() : AbstractExample(), _state(NULL) { }

	/// Construct an example with no y-value
	Example(const State* state, const Action& action);

	Example(const State* state, const Action& action, bool is_correct, float weight=1);

	const State& state() const { this->sanity_check(); return(*_state); }
	const ItemTokens& frontier() const;
	unsigned head() const { this->sanity_check(); return(_head); }

	static string name() { return "Example"; }

	/// From which sentence is this AbstractExample derived?
	const ID<Sentence>& sentence() const;

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & _state;
		ar & _initial_weight;
		ar & _confidence;
		ar & _have_confidence;
		ar & _is_correct;
		ar & _have_is_correct;
		ar & _head;
		ar & _start;
		ar & _end;
	}

protected:
	void sanity_check() const;

	void construct(const Action& action);

	const State* _state;

	/// The index of the head in the span.
	/// \todo Don't store this?
	uint8_t _head;

	uint16_t _start, _end;
};

#endif /* __Example_H__ */
