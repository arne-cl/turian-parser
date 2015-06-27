/*  $Id: globals.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file globals.H
 *  \brief Global variables, what not, and miscellania.
 *
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#ifndef __globals_H__
#define  __globals_H__

#include <iostream>
#include <cassert>
#include <cmath>
#include <ext/hash_map>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

#include <boost/shared_ptr.hpp>

const unsigned NO_VALUE = (unsigned)-1;

/// Perform *slow* sanity checks that are not particularly critical (or, for that matter,
/// likely to fail).
/// Even if this is disabled, certain critical sanity checks will nonetheless be performed.
//#define SANITY_CHECKS

/// Where applicable, use logs for floating point operation.
//#define USE_LOGS

typedef enum { EXPONENTIAL_LOSS=1, LOGISTIC_LOSS=2 } loss_ty;

/// Floating-point type, for memory-intensive data-structures
//typedef double myfloat;
typedef float myfloat;

/// Length of buffers
enum {BUFSIZE = 1024};

/// WRITEME
const unsigned punctuation_cnt = 5;
const string punctuation_tags[punctuation_cnt] = {":", ",", ".", "``", "''"};

/// Different length values allowed for length features.
/// \note These should be in increasing order.
const unsigned LENGTH_VALUES_CNT = 16;
const unsigned LENGTH_VALUES[LENGTH_VALUES_CNT+1] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, (unsigned)-1 };
//const unsigned LENGTH_VALUES_CNT = 6;
//const unsigned LENGTH_VALUES[LENGTH_VALUES_CNT+1] = {0, 1, 2, 3, 4, 5, (unsigned)-1 };
//const unsigned LENGTH_VALUES_CNT = 8;
//const unsigned LENGTH_VALUES[LENGTH_VALUES_CNT+1] = {0, 1, 2, 3, 4, 6, 8, 10, (unsigned)-1 };
//const unsigned LENGTH_VALUES_CNT = 26;
//const unsigned LENGTH_VALUES[LENGTH_VALUES_CNT+1] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 54, 64, 80, 96, 112, 128, (unsigned)-1 };

const unsigned PRIME=7919;

/// We only verify the initial weights in all trees if sanity checks
/// are enabled (#ifdef SANITY_CHECKS).
/// Otherwise, we verify the initial weight in this many trees.
const unsigned TREES_TO_VERIFY_INITIAL_WEIGHT = 5;

/// When automagically tuning the program to use the Example type that
/// gives the fastest confidence computations, we'll use roughly this
/// many seconds to do the tuning.
const unsigned SECONDS_FOR_AUTOMAGIC_EXAMPLE_TUNING = 10;
/// During tuning, check the user time spent after each
/// batch of this many examples.
const unsigned EXAMPLES_TO_CHECK_PER_TIME_CHECK = 10;
/// Use at most this many Example%s to do tuning.
const unsigned MAX_EXAMPLES_FOR_AUTOMAGIC_TUNING = 10000;

#ifndef DOXYGEN
namespace __gnu_cxx {
        //! Template specialization of STL's hash object.
	template<> class hash<double> {
		public: size_t operator() (const double f) const {
			return (int(log(f)*PRIME));
		}
	};
}
#endif /* DOXYGEN */

/// From Victor Bazarov:
/// <X4OdnZQtTpcgJHfenZ2dnUVZ_tGdnZ2d@comcast.com>
template<class T, class U> struct same { enum { yes = 0 }; };
template<class T> struct same<T,T> { enum { yes = 1 }; }; 

#include "parameter.H"

#endif /* __globals_H__ */
