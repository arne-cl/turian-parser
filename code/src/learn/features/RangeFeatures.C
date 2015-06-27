/*  $Id: RangeFeatures.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file RangeFeatures.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/RangeFeatures.H"

#include "learn/examples/IntermediateExample.H"

#include "parse/ItemToken.H"

#include "universal/parameter.H"
#include "universal/posclass.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/Unique.H"

RangeFeatures* RangeFeatures::_instance = NULL;

list<RangeFeature> RangeFeatures::_all = list<RangeFeature>();
FeaturePtrs RangeFeatures::_all_ptrs = FeaturePtrs();
AutoVector<AutoVector<AutoVector<AutoVector<const RangeFeature*> > > > RangeFeatures::_cache = AutoVector<AutoVector<AutoVector<AutoVector<const RangeFeature*> > > >();

const RangeFeatures* RangeFeatures::instance() {
	if (!_instance) {
		_instance = new RangeFeatures;
		assert(_instance);
	}
	return _instance;
}

const FeaturePtrs& RangeFeatures::all() const {
	return _all_ptrs;
}
	
void RangeFeatures::cache_put(range_group_ty group, unsigned range, item_predicate_ty predicate, unsigned value, const RangeFeature* ptr) {
	_cache[group][range][predicate][value] = ptr;
}

const RangeFeature* RangeFeatures::cache_get(range_group_ty group, unsigned range, item_predicate_ty predicate, unsigned value) const {
	return _cache.at(group).at(range).at(predicate).at(value);
}

RangeFeatures::RangeFeatures() {
	assert(_all.empty());
	assert(_all_ptrs.empty());
	// Assert _cache is empty?
	
	RangeFeatures::construct();
	_cache.lock();

	sanity_check_uniq(_all);
	assert(_all.size() == _all_ptrs.size());

	Debug::log(2) << "RangeFeatures::RangeFeatures() built " << _all.size() << " features\n";
}

void RangeFeatures::construct() {
	LIST<range_group_ty>::const_iterator i;
	for (i = all_range_groups().begin(); i != all_range_groups().end(); i++)
		RangeFeatures::construct(*i);
}

void RangeFeatures::construct(range_group_ty group) {
	for (unsigned i = 1; i <= max_group_range(group); i++)
		RangeFeatures::construct(group, i);
}

void RangeFeatures::construct(range_group_ty group, unsigned range) {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		RangeFeatures::construct(group, range, *i);
}

void RangeFeatures::construct(range_group_ty group, unsigned range, item_predicate_ty predicate) {
	const LIST<unsigned>& l = all_predicate_values(predicate);
	for (LIST<unsigned>::const_iterator i = l.begin(); i != l.end(); i++)
		if (!is_binary_predicate(predicate) || *i != (unsigned)false || parameter::binary_predicates_can_have_false_value())
			RangeFeatures::new_feature(group, range, predicate, *i);
}

void RangeFeatures::new_feature(range_group_ty group, unsigned range, item_predicate_ty predicate, unsigned value) {
	RangeFeature f(group, range, predicate, value);
	f.newID();
	_all.push_back(f);
	_all_ptrs.push_back(&_all.back());
	cache_put(group, range, predicate, value, &_all.back());

	// Insert f into Unique<RangeFeature> to ensure that f is unique (i.e. it has not already been constructed).	
	Unique<RangeFeature>::insert(f);
}

/// Ensure that this feature is semantically valid.
void RangeFeature::sanity_check() const {
	sanity_check_range_group_ty(_group);
	sanity_check_item_predicate_ty(_predicate);
	sanity_check_predicate_and_value(_predicate, _value);

	assert(_range <= max_group_range(_group));
}

/// \todo Assert that headword doesn't contain doublequotes.
const string RangeFeature::to_string() const {
	this->sanity_check();

	string s;
	s += "some_item_within_";
	s += range_string(_group, _range);
	s += ".";
	s += predicate_value_string(_predicate, _value);
	return s;
}

bool RangeFeature::apply(const IntermediateExample& e) const {
#ifdef SANITY_CHECKS
	this->sanity_check();
#endif /* SANITY_CHECKS */

	for (unsigned i = 1; i <= _range; i++) {
		const ItemToken* item = get_range_group_offset_item(_group, i, e);
		if (!item) return false;
		else if (predicate_holds(_predicate, _value, *item)) return true;
	}

/*
	for (GroupIterator i(_group, e); !i.end(); i++)
		if (predicate_holds(_predicate, _value, *i))
			return true;
			*/

	return false;
}

FeatureIDs RangeFeatures::get_slow(const IntermediateExample& e) const {
	FeatureIDs l;

	const FeaturePtrs& allf = RangeFeatures::instance()->all();
	FeaturePtrs::const_iterator i;
	for (i = allf.begin(); i != allf.end(); i++) {
		const Feature& f = **i;
		if (f(e))
			l.push_back(f.id());
	}

	return l;
}

FeatureIDs RangeFeatures::get(const IntermediateExample& e) const {
	FeatureIDs l;

	RangeFeatures::get(e, l);

#ifdef SANITY_CHECKS
	FeatureIDs slow_l = this->get_slow(e);
	assert(l.size() == slow_l.size());
	hash_set<ID<Feature> > h(slow_l.begin(), slow_l.end());
	FeatureIDs::const_iterator i;
	for (i = l.begin(); i != l.end(); i++)
		assert(h.find(*i) != h.end());
#endif /* SANITY_CHECKS */

	return l;
}

/// \todo Use instance instead of direct call?
void RangeFeatures::get(const IntermediateExample& e, FeatureIDs& l) const {
	LIST<range_group_ty>::const_iterator i;
	for (i = all_range_groups().begin(); i != all_range_groups().end(); i++)
		RangeFeatures::get(e, l, *i);
}

/// \todo This can be optimized quite a bit.
void RangeFeatures::get(const IntermediateExample& e, FeatureIDs& l, range_group_ty group) const {
	LIST<int>::const_iterator i;
	for (unsigned range = 1; range <= max_group_range(group); range++)
		RangeFeatures::get(e, l, group, range);
}

void RangeFeatures::get(const IntermediateExample& e, FeatureIDs& l, range_group_ty group, unsigned range) const {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		RangeFeatures::get(e, l, group, range, *i);
}

void RangeFeatures::get(const IntermediateExample& e, FeatureIDs& l, range_group_ty group, unsigned range, item_predicate_ty predicate) const {
	ItemTokens items;

	unsigned values_size = max_predicate_value(predicate)+1;
	vector<bool> exists_features(values_size, false);
	deque<unsigned> value_list;

	unsigned v;
	for (unsigned i = 1; i <= range; i++) {
		const ItemToken* item = get_range_group_offset_item(group, i, e);
		if (!item) break;

		v = get_predicate_value(predicate, *item);
		if (is_binary_predicate(predicate) && v == (unsigned)false && !parameter::binary_predicates_can_have_false_value()) continue;
		exists_features.at(v) = true;
		/// \todo Better to use true size rather than range, which is an overestimate
		if (range*2 < values_size)
			value_list.push_back(v);
	}

	/// \todo Better to use true size rather than range, which is an overestimate
	if (range*2 < values_size) {
		deque<unsigned>::const_iterator i;
		for (i = value_list.begin(); i != value_list.end(); i++)
			if (exists_features.at(*i)) {
				l.push_back(cache_get(group, range, predicate, *i)->id());
				exists_features.at(*i) = false;
			}
	} else {
		unsigned i;
		for (i = 0; i < exists_features.size(); i++)
			if (exists_features.at(i))
				l.push_back(cache_get(group, range, predicate, i)->id());
	}
}
