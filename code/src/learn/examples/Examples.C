/*  $Id: Examples.C 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Examples.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/examples/Examples.H"

#include "learn/examples/Example.H"
#include "learn/examples/FullExample.H"
#include "learn/examples/IntermediateExample.H"

#include "learn/Weights.H"

#include "universal/Debug.H"
#include "universal/stats.H"

template<typename EXAMPLE> template<typename EXAMPLE2>
Examples<EXAMPLE>::Examples(const Examples<EXAMPLE2>& e) {
	Debug::log(2) << "Examples<" << EXAMPLE::name() << ">::Examples(Examples<" << EXAMPLE2::name() << "> [" << e.size() << "])...\n";
	Debug::log(3) << stats::resource_usage() << "\n";
	this->reserve(e.size());
	this->insert(this->begin(), e.begin(), e.end());
	assert(this->size() == e.size());
	Debug::log(2) << "...Examples<" << EXAMPLE::name() << ">::Examples(Examples<" << EXAMPLE2::name() << "> [" << e.size() << "])\n";
	Debug::log(2) << stats::resource_usage() << "\n";
}

template<typename EXAMPLE> double Examples<EXAMPLE>::unpenalized_loss() const {
	double l = 0;
	for (typename Examples<EXAMPLE>::const_iterator e = this->begin(); e != this->end(); e++)
		l += e->loss();
	return l;
}

template<typename EXAMPLE> Weights Examples<EXAMPLE>::initial_weight() const {
	Weights w;
	for (typename Examples<EXAMPLE>::const_iterator e = this->begin(); e != this->end(); e++)
		w.add(e->initial_weight(), e->is_correct());
	return w;
}

template<typename EXAMPLE> Weights Examples<EXAMPLE>::current_weight() const {
	Weights w;
	for (typename Examples<EXAMPLE>::const_iterator e = this->begin(); e != this->end(); e++)
		w.add(e->weight(), e->is_correct());
	return w;
}

template<typename EXAMPLE> void Examples<EXAMPLE>::add_confidence(double conf) {
	for (typename Examples<EXAMPLE>::iterator e = this->begin(); e != this->end(); e++)
		e->set_confidence(e->confidence() + conf);
}


template<typename EXAMPLE> double ExamplePtrs<EXAMPLE>::unpenalized_loss() const {
	double l = 0;
	for (typename ExamplePtrs<EXAMPLE>::const_iterator e = this->begin(); e != this->end(); e++)
		l += (*e)->loss();
	return l;
}

template<typename EXAMPLE> Weights ExamplePtrs<EXAMPLE>::initial_weight() const {
	Weights w;
	for (typename ExamplePtrs<EXAMPLE>::const_iterator e = this->begin(); e != this->end(); e++)
		w.add((*e)->initial_weight(), (*e)->is_correct());
	return w;
}

template<typename EXAMPLE> Weights ExamplePtrs<EXAMPLE>::current_weight() const {
	Weights w;
	for (typename ExamplePtrs<EXAMPLE>::const_iterator e = this->begin(); e != this->end(); e++)
		w.add((*e)->weight(), (*e)->is_correct());
	return w;
}

template<typename EXAMPLE> void ExamplePtrs<EXAMPLE>::add_confidence(double conf) {
	for (typename ExamplePtrs<EXAMPLE>::iterator e = this->begin(); e != this->end(); e++)
		(*e)->set_confidence((*e)->confidence() + conf);
}

// TEMPLATE INSTANTIATIONS
template Examples<IntermediateExample>::Examples(Examples<Example> const&);
template Examples<DenseFullExample>::Examples(Examples<Example> const&);
template Examples<SparseFullExample>::Examples(Examples<Example> const&);
template double Examples<Example>::unpenalized_loss() const;
template double Examples<DenseFullExample>::unpenalized_loss() const;
template double Examples<SparseFullExample>::unpenalized_loss() const;
template Weights Examples<Example>::current_weight() const;
template Weights Examples<DenseFullExample>::current_weight() const;
template Weights Examples<SparseFullExample>::current_weight() const;

template void ExamplePtrs<Example>::add_confidence(double);
template void ExamplePtrs<DenseFullExample>::add_confidence(double);
template void ExamplePtrs<SparseFullExample>::add_confidence(double);
template double ExamplePtrs<Example>::unpenalized_loss() const;
template double ExamplePtrs<DenseFullExample>::unpenalized_loss() const;
template double ExamplePtrs<SparseFullExample>::unpenalized_loss() const;
template Weights ExamplePtrs<Example>::initial_weight() const;
template Weights ExamplePtrs<DenseFullExample>::initial_weight() const;
template Weights ExamplePtrs<SparseFullExample>::initial_weight() const;
