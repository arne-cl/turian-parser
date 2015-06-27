/*  $Id: FullExample.C 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file FullExample.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/examples/FullExample.H"
#include "learn/examples/IntermediateExample.H"

template<typename FEATURE_HASH>
FullExample<FEATURE_HASH>::FullExample(const Example& e) : AbstractExample(e) {
	this->construct(IntermediateExample(e));
}

template<typename FEATURE_HASH>
FullExample<FEATURE_HASH>::FullExample(const IntermediateExample& e) : AbstractExample(e) {
	this->construct(e);
}

template<typename FEATURE_HASH>
void FullExample<FEATURE_HASH>::construct(const IntermediateExample& e) {
	assert(!_fset);
	FeatureIDs flist = e.flist();
	//_fset = boost::shared_ptr<FEATURE_HASH>(new FEATURE_HASH(flist.begin(), flist.end(), flist.size()));
	_fset = boost::shared_ptr<FEATURE_HASH>(new FEATURE_HASH(flist.size()));
	this->construct_help();
	_fset->insert(flist.begin(), flist.end());
	assert(_fset);
	assert(!_fset->empty());
	assert(this->fset().size() == flist.size());
	_sentence = e.sentence();
	assert(!_sentence.empty());
}
template<> void DenseFullExample::construct_help() { _fset->set_empty_key(EMPTY_FEATURE_ID); }
template<> void SparseFullExample::construct_help() { }



template<typename FEATURE_HASH>
bool FullExample<FEATURE_HASH>::has_feature(ID<Feature> f) const {
	return this->fset().find(f) != this->fset().end();
}
template<typename FEATURE_HASH>
bool FullExample<FEATURE_HASH>::has_feature(const Feature* f) const {
	return this->has_feature(f->id());
}

template<> string SparseFullExample::name() { return "SparseFullExample"; }
template<> string DenseFullExample::name() { return "DenseFullExample"; }



template<typename FEATURE_HASH> template<typename FEATURE_HASH2>
FullExample<FEATURE_HASH>::FullExample(const FullExample<FEATURE_HASH2>& e) :
	AbstractExample(e),
	_sentence(e._sentence),
	_fset(e._fset) { }

template<> template<>
DenseFullExample::FullExample(const SparseFullExample& e) {
	Debug::warning(__FILE__, __LINE__, "Why are you doing this conversion?");
	assert(0);
}
template<> template<>
SparseFullExample::FullExample(const DenseFullExample& e) {
	Debug::warning(__FILE__, __LINE__, "Why are you doing this conversion?");
	assert(0);
}


// TEMPLATE INSTANTIATIONS
template SparseFullExample::FullExample(const Example& e);
template SparseFullExample::FullExample(const IntermediateExample& e);
template bool SparseFullExample::has_feature(const Feature* f) const;
template DenseFullExample::FullExample(const Example& e);
template DenseFullExample::FullExample(const IntermediateExample& e);
template bool DenseFullExample::has_feature(const Feature* f) const;
