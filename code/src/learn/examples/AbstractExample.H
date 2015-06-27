/*  $Id: AbstractExample.H 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file AbstractExample.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class AbstractExample
 *  \brief An Abstract training example.
 *
 *  Basically, stores weight and perhaps confidence and y-value.
 *
 *  \todo Without virtual functions, can we require that all subtypes
 *  implement method:
 *  const ID<Sentence>& sentence() const;
 *
 *  \todo We need specific Example type promotion rules.
 *  Only certain sort of conversions should be permitted.
 *
 */

#ifndef __AbstractExample_H__
#define  __AbstractExample_H__

#include "universal/Archive.H"

#include <ostream>
#include <sstream>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

//class Sentence;
//template<typename SENTENCE> class ID;

class AbstractExample {
public:
	/// Construct an example with no y-value
	AbstractExample();

	AbstractExample(bool is_correct, float weight=1);

	/// From which sentence is this AbstractExample derived?
	//const ID<Sentence>& sentence() const;

	/// Return a string version of this AbstractExample's actual type.
	static string name();

	double confidence() const;
	double loss() const;
	double weight() const;
	double margin() const;
	bool is_correct() const { this->sanity_check(); assert(_have_is_correct); return _is_correct; }

	float initial_weight() const { this->sanity_check(); return _initial_weight; }

	/// Find the loss of the AbstractExample, given a confidence change dconf.
	/// \pre _have_confidence
	double loss_dconf(double dconf) const;

	/// \todo REMOVEME
	bool have_is_correct() const { this->sanity_check(); return _have_is_correct; }

	bool have_confidence() const { return _have_confidence; }

	void set_confidence(const double& confidence);

	void set_initial_weight(const double& initial_weight) { _initial_weight = initial_weight; }

	/// Set the weight of this AbstractExample.
	/// If _have_confidence, then set _initial_weight such that this->weight() == weight.
	/// Otherwise, set _initial_weight to weight.
	void set_weight(const double& weight);

private:
	friend class boost::serialization::access;
	template <class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & _initial_weight;
		ar & _confidence;
		ar & _have_confidence;
		ar & _is_correct;
		ar & _have_is_correct;
	}

protected:
	void sanity_check() const;

	/// Initial weight of this AbstractExample.
	float _initial_weight;

	/// The confidence of this AbstractExample.
	/// \note Unused iff !_have_confidence
	float _confidence;

	/// Does we actually have this AbstractExample's confidence?
	bool _have_confidence;

	bool _is_correct;

	/// True iff this example has a y-value (is_correct = true or false).
	/// False iff this example has an undefined y-value. (This is used, e.g., when
	/// we are parsing new text.)
	bool _have_is_correct;
};

#endif /* __AbstractExample_H__ */
