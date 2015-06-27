/*  $Id: IntermediateExample.H 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file IntermediateExample.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class IntermediateExample
 *  \brief An intermediate, slightly uncompressed, representation of an Example.
 *
 *  An IntermediateExample permits Feature operations. However, these operations are
 *  relatively slow, since they are computed on-the-fly.
 *
 *  \todo IntermediateExample (Inference) should also contain the (Action's) Label.
 *  \todo Rewrite such that an example is just the desired ItemToken in a state.
 *
 */

#ifndef __IntermediateExample_H__
#define  __IntermediateExample_H__

#include "learn/examples/Example.H"

#include "parse/ItemToken.H"

class Feature;
class FeatureIDs;

class IntermediateExample : public Example {
public:
	IntermediateExample(const Example& e);

	/// Convert the example to a string.
	/// \todo Output vectors
	string to_string(const string s="", const bool longform=false) const;

	const ItemTokens& span() const;
	const ItemTokens& lcontext() const;
	const ItemTokens& rcontext() const;

	/// \todo Cache feature lookups, for speed?
	/// \todo Make this virtual?
	bool has_feature(ID<Feature> f) const;
	bool has_feature(const Feature* f) const;

        /// Get all FeatureIDs over which this IntermediateExample holds.
	FeatureIDs flist() const;

	static string name() { return "IntermediateExample"; }

	/// The terminal item that is the head.
	const ItemToken& head_terminal() const;

protected:
	/// \todo Rewrite using copy constructor.
	/// \todo Construct an IntermediateExample *not* from an Action, but from the ItemToken that would be the yield of this Action.
	//void construct_from_state(const State& state, const Action& action);

	ItemTokens _span, _lcontext, _rcontext;

	/// The terminal item that is the head.
	const ItemToken* _head_terminal;
};

#endif /* __IntermediateExample_H__ */
