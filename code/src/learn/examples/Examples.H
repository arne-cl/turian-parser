/*  $Id: Examples.H 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Examples.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Examples
 *  \brief Templatized-container of Example%s.
 *
 *  \todo Unify Examples and ExamplePtrs into one class, which can detect
 *  whether the template parameter type is a pointer or not.
 *
 */

#ifndef __Examples_H__
#define  __Examples_H__

#include <vector>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class Weights;

template <typename EXAMPLE>
class Examples : public vector<EXAMPLE> {
public:
	Examples() : vector<EXAMPLE>() { }

	template<typename EXAMPLE2> Examples(const Examples<EXAMPLE2>& e);

	/// Compute the unpenalized loss over this Example set.
	double unpenalized_loss() const;

	/// Compute the total initial Weights of this Example set.
	Weights initial_weight() const;

	/// Compute the total current Weights of this Example set.
	Weights current_weight() const;

	/// Add confidence conf to this Example set.
	void add_confidence(double conf);
};

template <typename EXAMPLE>
class ExamplePtrs : public vector<EXAMPLE*> {
public:
	ExamplePtrs() : vector<EXAMPLE*>() { }

	template<typename EXAMPLE2> ExamplePtrs<EXAMPLE>(const ExamplePtrs<EXAMPLE2>& e);

	/// Compute the unpenalized loss over this Example set.
	double unpenalized_loss() const;

	/// Compute the total initial Weights of this Example set.
	Weights initial_weight() const;

	/// Compute the total current Weights of this Example set.
	Weights current_weight() const;

	/// Add confidence conf to this Example set.
	void add_confidence(double conf);
};

#endif /* __Examples_H__ */
