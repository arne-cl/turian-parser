/*  $Id: loss.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file loss.H
 *  \brief Loss computation methods.
 *
 *  Methods for computing losses, margins, and example weights.
 *  \note Boosting specific.
 *
 *  \todo Inline some of these functions?
 *  \todo Rename "loss" to "gain" where applicable, to avoid confusion.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#ifndef __loss_H__
#define  __loss_H__

#include <vector>

#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

/// Find the margin of a prediction.
/// \param conf The confidence of the prediction.
/// \param is_correct Whether the prediction is correct or not.
/// \return The margin of this prediction.
double margin_of(double confidence, bool is_correct);

/// Convert a margin to a loss.
/// \todo Are these conf->loss conversions correct?
/// \param margin The margin of the prediction.
/// \return The loss of this example.
double loss_of(double margin);

/// Convert an example's margin to a weight.
/// \todo Are these margin->weight conversions correct?
/// \param margin The margin of the example.
/// \return The weight of this example.
double weight_of(double margin);

template<typename EXAMPLE> class ExamplePtrs;
/// Choose the delta confidence update that minimizes penalized loss
/// over this set of examples.
/// We do this with a line search.
/// \todo Use const vector<const Example*>& parameter, not const vector<Example*>&
//double minimize_loss(const vector<const Example*>& examples);
template<typename EXAMPLE> double minimize_loss(const ExamplePtrs<EXAMPLE>& examples);

#endif /* __loss_H__ */
