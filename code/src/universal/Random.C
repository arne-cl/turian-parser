/*  $Id: Random.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Random.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "universal/Random.H"

#include <cstdlib>
#include <cassert>

/// Random number stream used by parse path generator.
Random ParsePathRandom;

/// Create a random number generator with seed 0.
/// \warning Unfortunately, we cannot use
/// parameter::seed() as the initializer, since
/// ParsePathRandom is constructed before we have
/// a chance to read in the parameters file.
/// To resolve this, ParsePathRandom should be a
/// Random* (which we would later "new" when we
/// need it, after reading the parameters).
Random::Random() {
	this->seed(0);
}

/// Create a random number generator with a given seed.
Random::Random(long int new_seed) {
	this->seed(new_seed);
}

/// Re-seed the random number generator,
/// and restart the stream with this new seed.
/// \note We follow the same convention as srand48().
/// From the srand48() manpage:
///	The initializer function srand48() sets the high order 32-bits of Xi to
///	the argument seedval. The low order 16-bits are set to the arbitrary
///	value 0x330E.
/// \todo Man, I hope this works as desired.
void Random::seed(long int new_seed) {
	original_xsubi[0] = (unsigned short int)(new_seed >> 16);
	original_xsubi[1] = (unsigned short int)(new_seed);
	original_xsubi[2] = 0x330E;

	this->restart();
}

/// Restart the random number stream at the
/// original seed.
void Random::restart() {
	xsubi[0] = original_xsubi[0];
	xsubi[1] = original_xsubi[1];
	xsubi[2] = original_xsubi[2];
}

/// Retrieve a random number between 0 and max-1 inclusive.
unsigned Random::get(unsigned max) const {
	assert(max > 0);
	double erand = erand48(xsubi);
	assert(erand >= 0. && erand < 1.);
	unsigned ret = (unsigned)(erand * (double)max);
	assert(ret >= 0 && ret < max);
	return ret;
}

/// Perform a binary trial.
/// \param likelihood Likelihood of the trial being successful.
/// \invariant 0 <= likelihood <= 1
/// \return True iff the trial succeeded.
/// \todo What's the proper terminology for what this method does?
bool Random::trial(double likelihood) const {
	double erand = erand48(xsubi);
	assert(erand >= 0. && erand < 1.);
	assert(likelihood >= 0. && likelihood <= 1.);
	return (likelihood > erand);
}
