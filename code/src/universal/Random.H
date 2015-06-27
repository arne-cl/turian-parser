/*  $Id: Random.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Random.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Random
 *  \brief A random number stream.
 *
 *  This class can be used to create a random number stream,
 *  which is seeded independently and in isolation from the global srand48() seed
 *  and other Random instances.
 *
 *  \todo Rewrite, using stronger random number routines from Boost.
 *  \todo Replace all instances of drand48 in our implementation with a method from a Random object.
 *  \todo Make our programs deterministic, storing their seeds such that
 *  different executions choose the same random numbers every time.
 *  \todo Don't make all these methods const; Rather, instances of Random
 *  should typically be mutable.
 *
 */

#ifndef __Random_H__
#define  __Random_H__

#include <deque>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

typedef unsigned short int int48[3];

class Random {
public:
	/// Create a random number generator with seed 0.
	/// \warning Unfortunately, we cannot use
	/// parameter::seed() as the initializer, since
	/// ParsePathRandom is constructed before we have
	/// a chance to read in the parameters file.
	/// To resolve this, ParsePathRandom should be a
	/// Random* (which we would later "new" when we
	/// need it, after reading the parameters).
	Random();
	
	/// Create a random number generator with a given seed.
	Random(long int new_seed);

	/// Re-seed the random number generator,
	/// and restart the stream with this new seed.
	/// \note We follow the same convention as srand48().
	/// From the srand48() manpage:
	///	The initializer function srand48() sets the high order 32-bits of Xi to
	///	the argument seedval. The low order 16-bits are set to the arbitrary
	///	value 0x330E.
	/// \todo Man, I hope this works as desired.
	void seed(long int new_seed);

	/// Restart the random number stream at the
	/// original seed.
	void restart();

	/// Retrieve a random number between 0 and max-1 inclusive.
	unsigned get(unsigned max) const;

	/// Perform a binary trial.
	/// \param likelihood Likelihood of the trial being successful.
	/// \invariant 0 <= likelihood <= 1
	/// \return True iff the trial succeeded.
	/// \todo What's the proper terminology for what this method does?
	bool trial(double likelihood) const;

private:
	int48 original_xsubi;
	mutable int48 xsubi;
};

/// Random number stream used by parse path generator.
extern Random ParsePathRandom;

#endif /* __Random_H__ */
