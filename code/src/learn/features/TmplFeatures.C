/*  $Id: TmplFeatures.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file TmplFeatures.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/TmplFeatures.H"

#include "learn/examples/IntermediateExample.H"

#include "universal/parameter.H"
#include "universal/Debug.H"
#include "universal/Unique.H"

#include <cassert>
#include <sstream>

TmplFeatures* TmplFeatures::_instance = NULL;

list<TmplFeature> TmplFeatures::_all = list<TmplFeature>();
FeaturePtrs TmplFeatures::_all_ptrs = FeaturePtrs();
AutoVector<AutoVector<AutoVector<AutoVector<const TmplFeature*> > > > TmplFeatures::_cache = AutoVector<AutoVector<AutoVector<AutoVector<const TmplFeature*> > > >();

const TmplFeatures* TmplFeatures::instance() {
	if (!_instance) {
		_instance = new TmplFeatures;
		assert(_instance);
	}
	return _instance;
}

const FeaturePtrs& TmplFeatures::all() const {
	return _all_ptrs;
}
	
void TmplFeatures::cache_put(..., const TmplFeature* ptr) {
	_cache[...] = ptr;
}

const TmplFeature* TmplFeatures::cache_get(...) const {
	return _cache.at(...);
}
	
TmplFeatures::TmplFeatures() {
	assert(_all.empty());
	assert(_all_ptrs.empty());
	// Assert _cache is empty?
	
	TmplFeatures::construct();
	_cache.lock();

	sanity_check_uniq(_all);
	assert(_all.size() == _all_ptrs.size());

	Debug::log(2) << "TmplFeatures::TmplFeatures() built " << _all.size() << " features\n";
}

/// \todo WRITEME
void TmplFeatures::construct() {
	assert(0);
}

/// \todo WRITEME
void TmplFeatures::new_feature(...) {
	assert(0);
	
	TmplFeature f(...);
	f.newID();
	_all.push_back(f);
	_all_ptrs.push_back(&_all.back());
	cache_put(..., &_all.back());

	// Insert f into Unique<TmplFeature> to ensure that f is unique (i.e. it has not already been constructed).	
	Unique<TmplFeature>::insert(f);
}

/// Ensure that this feature is semantically valid.
/// \todo WRITEME
void TmplFeature::sanity_check() const {
	assert(0);
}

/// \todo WRITEME
const string TmplFeature::to_string() const {
	this->sanity_check();

	ostringstream o;
	assert(0);
	return o.str();
}

/// \todo WRITEME
bool TmplFeature::apply(const IntermediateExample& e) const {
#ifdef SANITY_CHECKS
	this->sanity_check();
#endif /* SANITY_CHECKS */

	assert(0);
}

/// \todo At the end, do a slow, exhaustive sanity check.
FeatureIDs TmplFeatures::get(const IntermediateExample& e) const {
	FeatureIDs l;

	const FeaturePtrs& allf = TmplFeatures::instance()->all();
	FeaturePtrs::const_iterator i;
	for (i = allf.begin(); i != allf.end(); i++) {
		const Feature& f = **i;
		if (f(e))
			l.push_back(f.id());
	}

	return l;
}
