/*  $Id: head.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file head.H
 *  \brief Lexicalize Item%s.
 *
 *  Determine the head child of a parent item, given its label and the
 *  labels of its children.
 *
 *  Head rules are based upon Collins (1999, pg. 240).
 *  For modifications to Collins's rules, grep for MODIFICATION
 *
 *  \todo Add a dummy last element, s.t. we can check that head_rules_cnt is correct.
 *  \todo Check Bikel to ensure head rules are correct
 *  (i.e. that we want to look at the *label* of the children)
 *  \todo Make sure all modifications are in the documentation.
 *  \todo Modify coordinating conjunction special case, s.t.
 *  they can apply several times to find the *first* head in several coordinated conjunctions.
 *  \todo Make this a class, built with the Singleton strategy.
 *  \todo Add ':' and ',' to coordinating conjunction determination?
 *  If so, then also change this in ItemToken.C
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#ifndef __head_H__
#define  __head_H__

#include "universal/vocabulary.H"

#include <vector>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

/// Get the head of an item.
/// \param l The label of the parent item.
/// \param children The labels of the children items.
/// \return The index of the head-child.
unsigned head_get(const Label l, const vector<Label>& children);

#endif /* __head_H__ */
