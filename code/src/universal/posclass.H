/*  $Id: posclass.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file posclass.H
 *  \brief Part-of-speech classes.
 *
 *  Internally, these are represented numerically. These methods allow
 *  one to convert back-and-forth between the numerical and string representations.
 *
 *  \todo Make this a class, built with the Singleton strategy?
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#ifndef __posclass_H__
#define  __posclass_H__

#include "universal/vocabulary.H"

#include <string>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

#include "universal/LIST.H"	// REMOVEME
//class LIST;

typedef unsigned Posclass;

const Posclass NO_POSCLASS = 0;

Posclass tag_to_posclass(const Label tag);

Posclass string_to_posclass(const string s);

string posclass_string(const Posclass c);

const LIST<Posclass>& all_posclass();

/// Is this a posclass?
/// Return false iff c is NO_POSCLASS or not a valid posclass.
bool is_posclass(const Posclass c);

Posclass Posclass_N();
Posclass Posclass_NP();

#endif /* __posclass_H__ */
