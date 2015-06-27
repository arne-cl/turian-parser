/*  $Id: Sampler.H 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Sampler.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Sampler
 *  \brief 
 *
 *  The sampling method is based upon "Sampling to estimate arbitrary
 *  subset sums" (Duffield et al.):
 *  "we consider a [stream] of items i=0,...,n-1 with weights wi. For each
 *  item i, we generate a random number ri in (0,1) and create a priority
 *  qi=wi/ri. The sample S consists of the k highest priority items. Let
 *  t be the (k+1)th highest priority. Each sampled item i in S gets
 *  a weight estimate Wi=max{wi,t}, while non-sampled items get weight
 *  estimate Wi=0."
 *
 *  \todo We may even be able to get away with keeping the Example
 *  Feature vectors around, uncompressed. (HeavyExample)
 *  \todo Design an Sampler iterator (perhaps using
 *  the Boost Iterators library).
 *  \todo Use a Random type for random numbers.
 *
 */

#ifndef __Sampler_H__
#define  __Sampler_H__

#include "learn/examples/Examples.H"
#include "universal/Double.H"

#include <deque>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

template<typename EXAMPLE> class Sampler : public Examples<EXAMPLE> {
public:
	/// Sample a set of at most n examples.
	/// \todo Use Random instance to choose random numbers for std::random_shuffle.
	Sampler(const Examples<EXAMPLE>& exmpls, unsigned n);

	virtual ~Sampler() { }

private:
	/// Sample a set of examples.
	/// The sampling method is based upon "Sampling to estimate
	/// arbitrary subset sums" (Duffield et al.):
	/// "we consider a [stream] of items i=0,...,n-1 with weights
	/// wi. For each item i, we generate a random number ri in
	/// (0,1) and create a priority qi=wi/ri. The sample S consists
	/// of the k highest priority items. Let t be the (k+1)th
	/// highest priority. Each sampled item i in S gets a weight
	/// estimate Wi=max{wi,t}, while non-sampled items get weight
	/// estimate Wi=0."
	/// \arg yvalue Required y-value for each example sampled.
	/// \arg maxn The maximum number of examples to sample.
	/// \post ::_examples will contain at most maxn yvalue'd examples.
	void sample(const Examples<EXAMPLE>& exmpls, bool yvalue, unsigned maxn);
	void sample(const Examples<EXAMPLE>& exmpls, unsigned maxn);
};

#endif /* __Sampler_H__ */
