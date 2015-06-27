/*  $Id: Features.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Features.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/Features.H"

#include "learn/features/ItemFeatures.H"
#include "learn/features/LengthFeatures.H"
#include "learn/features/QuantFeatures.H"
#include "learn/features/ExistsFeatures.H"
#include "learn/features/ExistsTerminalFeatures.H"
#include "learn/features/ItemChildFeatures.H"
#include "learn/features/RangeFeatures.H"
#include "learn/features/TerminalItemFeatures.H"

#include "learn/examples/IntermediateExample.H"

#include "universal/parameter.H"
#include "universal/stats.H"
#include "universal/Debug.H"


Features* Features::_instance = NULL;

FeaturePtrs Features::_all = FeaturePtrs();
AutoVector<FeaturePtr> Features::_id_to_ptr = AutoVector<const Feature*>();

/// \todo This structure consumes a lot of memory.
/// Destroy this hash when we're done with it.
hash_map<string, ID<Feature>, hash<string>, equal_to<string> > Features::_string_map = hash_map<string, ID<Feature>, hash<string>, equal_to<string> >();

ID<Feature> EMPTY_FEATURE_ID;

Features::Features() {
	assert(_all.empty());

	if (parameter::use_item_features()) {
		const FeaturePtrs& l = (FeaturePtrs&)ItemFeatures::instance()->all();
		_all.insert(_all.end(), l.begin(), l.end());
	}

	if (parameter::use_length_features()) {
		const FeaturePtrs& l = (FeaturePtrs&)LengthFeatures::instance()->all();
		_all.insert(_all.end(), l.begin(), l.end());
	}

	if (parameter::use_quant_features()) {
		const FeaturePtrs& l = (FeaturePtrs&)QuantFeatures::instance()->all();
		_all.insert(_all.end(), l.begin(), l.end());
	}

	if (parameter::use_exists_features()) {
		const FeaturePtrs& l = (FeaturePtrs&)ExistsFeatures::instance()->all();
		_all.insert(_all.end(), l.begin(), l.end());
	}

	if (parameter::use_exists_terminal_features()) {
		const FeaturePtrs& l = (FeaturePtrs&)ExistsTerminalFeatures::instance()->all();
		_all.insert(_all.end(), l.begin(), l.end());
	}

	if (parameter::use_item_child_features()) {
		const FeaturePtrs& l = (FeaturePtrs&)ItemChildFeatures::instance()->all();
		_all.insert(_all.end(), l.begin(), l.end());
	}

	if (parameter::use_range_features()) {
		const FeaturePtrs& l = (FeaturePtrs&)RangeFeatures::instance()->all();
		_all.insert(_all.end(), l.begin(), l.end());
	}

	if (parameter::use_terminal_item_features()) {
		const FeaturePtrs& l = (FeaturePtrs&)TerminalItemFeatures::instance()->all();
		_all.insert(_all.end(), l.begin(), l.end());
	}

	for (FeaturePtrs::const_iterator f = _all.begin(); f != _all.end(); f++) {
		_id_to_ptr[(*f)->id()] = *f;
	}
	_id_to_ptr.lock();	// Lock against subsequent writes.
	assert(_all.size() == _id_to_ptr.size());

	this->create_string_map();

	Debug::log(2) << "Features::Features() built " << _all.size() << " features\n";

	EMPTY_FEATURE_ID.create();
	assert(!EMPTY_FEATURE_ID.empty());
}

/// \todo Slow this way, because of list copies. Faster to
/// pass around a reference.
FeatureIDs Features::get(const IntermediateExample& e) const {
	FeatureIDs l;

	if (parameter::use_item_features()) {
		FeatureIDs lnew = ItemFeatures::instance()->get(e);
		l.insert(l.end(), lnew.begin(), lnew.end());
	}

	if (parameter::use_length_features()) {
		FeatureIDs lnew = LengthFeatures::instance()->get(e);
		l.insert(l.end(), lnew.begin(), lnew.end());
	}

	if (parameter::use_quant_features()) {
		FeatureIDs lnew = QuantFeatures::instance()->get(e);
		l.insert(l.end(), lnew.begin(), lnew.end());
	}

	if (parameter::use_exists_features()) {
		FeatureIDs lnew = ExistsFeatures::instance()->get(e);
		l.insert(l.end(), lnew.begin(), lnew.end());
	}

	if (parameter::use_exists_terminal_features()) {
		FeatureIDs lnew = ExistsTerminalFeatures::instance()->get(e);
		l.insert(l.end(), lnew.begin(), lnew.end());
	}

	if (parameter::use_item_child_features()) {
		FeatureIDs lnew = ItemChildFeatures::instance()->get(e);
		l.insert(l.end(), lnew.begin(), lnew.end());
	}

	if (parameter::use_range_features()) {
		FeatureIDs lnew = RangeFeatures::instance()->get(e);
		l.insert(l.end(), lnew.begin(), lnew.end());
	}

	if (parameter::use_terminal_item_features()) {
		FeatureIDs lnew = TerminalItemFeatures::instance()->get(e);
		l.insert(l.end(), lnew.begin(), lnew.end());
	}

	/*
	FeatureIDs::const_iterator f;
	for (f = l.begin(); f != l.end(); f++) {
		Debug::log(2) << Features::from_id(*f).to_string() << "\n";
	}
	*/

#ifdef SANITY_CHECKS
	sanity_check_uniq(l);
#endif /* SANITY_CHECKS */

	return l;
}

/// Return the number of features.
unsigned Features::count() const {
	assert(!_all.empty());
	return _all.size();
}

/// \todo Maybe return a ptr?
const Feature& Features::from_id(const ID<Feature>& id) const {
	return *_id_to_ptr.at(id);
}

ID<Feature> Features::of_string(const string s) const {
	this->create_string_map();
	if (_string_map.find(s) == _string_map.end()) {
		cerr << "Couldn't find feature: " << s << "\n";
		assert(0);
	}
	assert(_string_map.find(s) != _string_map.end());
	return _string_map.find(s)->second;
/*
	FeaturePtr match = NULL;

	const FeaturePtrs& allf = Features::instance()->all();
	FeaturePtrs::const_iterator f;
	for (f = allf.begin(); f != allf.end(); f++) {
		if ((*f)->to_string() == s) {
			assert(!match);
			match = *f;
		}
	}
	assert(match);
	return match;
*/
}

const Features* Features::instance() {
	if (!_instance) {
		_instance = new Features;
		assert(_instance);
	}
	return _instance;
}

/// Create the string -> ID<Feature> map.
void Features::create_string_map() const {
	if (!_string_map.empty()) {
		assert(_all.size() == _string_map.size());
		return;
	}

	Debug::log(2) << "Creating string map...\n";
	Debug::log(2) << "(before) " << stats::resource_usage() << "\n";

	for (FeaturePtrs::const_iterator f = _all.begin(); f != _all.end(); f++) {
		_string_map[(*f)->to_string()] = (*f)->id();
	}

	Debug::log(2) << "Done. String map bucket count " << _string_map.bucket_count() << " (" << _all.size() << " entries)\n";
	Debug::log(2) << "(after) " << stats::resource_usage() << "\n";
}

/// Free the string -> ID<Feature> map.
void Features::free_string_map() const {
	if (_string_map.empty()) return;

	Debug::log(2) << "Freeing string map...\n";
	Debug::log(2) << "(before) " << stats::resource_usage() << "\n";

	// Under the g++'s I've tested, this hack can be used to shrink the
	// capacity of a vector to 0.
	// cf. <1106167657.974039.305900@c13g2000cwb.googlegroups.com>
	// and subsequent discussion.
	
	// Postscript: I've learned the hard way that, at the very least,
	// gcc 4.0.1 on Solaris will leak memory unless you first clear
	// the vector.
	_string_map.clear();
	
	hash_map<string, ID<Feature>, hash<string>, equal_to<string> >().swap(_string_map);

	Debug::log(2) << "(after) " << stats::resource_usage() << "\n";
}
