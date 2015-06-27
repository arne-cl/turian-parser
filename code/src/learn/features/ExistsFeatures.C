/*  $Id: ExistsFeatures.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ExistsFeatures.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/ExistsFeatures.H"
#include "learn/features/GroupIterator.H"

#include "learn/examples/IntermediateExample.H"

#include "parse/ItemToken.H"

#include "universal/parameter.H"
#include "universal/posclass.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/Unique.H"

ExistsFeatures* ExistsFeatures::_instance = NULL;

list<ExistsFeature> ExistsFeatures::_all = list<ExistsFeature>();
FeaturePtrs ExistsFeatures::_all_ptrs = FeaturePtrs();
AutoVector<AutoVector<AutoVector<const ExistsFeature*> > > ExistsFeatures::_cache = AutoVector<AutoVector<AutoVector<const ExistsFeature*> > >();

const ExistsFeatures* ExistsFeatures::instance() {
	if (!_instance) {
		_instance = new ExistsFeatures;
		assert(_instance);
	}
	return _instance;
}

const FeaturePtrs& ExistsFeatures::all() const {
	return _all_ptrs;
}

void ExistsFeatures::cache_put(item_group_ty group, item_predicate_ty predicate, unsigned value, const ExistsFeature* ptr) {
	_cache[group][predicate][value] = ptr;
}

const ExistsFeature* ExistsFeatures::cache_get(item_group_ty group, item_predicate_ty predicate, unsigned value) const {
	return _cache.at(group).at(predicate).at(value);
}

ExistsFeatures::ExistsFeatures() {
	assert(_all.empty());
	assert(_all_ptrs.empty());
	// Assert _cache is empty?
	
	ExistsFeatures::construct();
	_cache.lock();

	sanity_check_uniq(_all);
	assert(_all.size() == _all_ptrs.size());

	Debug::log(2) << "ExistsFeatures::ExistsFeatures() built " << _all.size() << " features\n";
}

void ExistsFeatures::construct() {
	LIST<item_group_ty>::const_iterator i;
	for (i = all_item_groups().begin(); i != all_item_groups().end(); i++) {
		if (*i == ALL_ITEMS) continue;
		if (!parameter::context_groups_for_exists_features() &&
				(*i == ALL_LCONTEXT_ITEMS || *i == ALL_RCONTEXT_ITEMS))
			continue;
		ExistsFeatures::construct(*i);
	}
}

void ExistsFeatures::construct(item_group_ty group) {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		ExistsFeatures::construct(group, *i);
}

void ExistsFeatures::construct(item_group_ty group, item_predicate_ty predicate) {
	const LIST<unsigned>& l = all_predicate_values(predicate);
	for (LIST<unsigned>::const_iterator i = l.begin(); i != l.end(); i++)
		if (!is_binary_predicate(predicate) || *i != (unsigned)false || parameter::binary_predicates_can_have_false_value())
			ExistsFeatures::new_feature(group, predicate, *i);
}

void ExistsFeatures::new_feature(item_group_ty group, item_predicate_ty predicate, unsigned value) {
	ExistsFeature f(group, predicate, value);
	f.newID();
	_all.push_back(f);
	_all_ptrs.push_back(&_all.back());
	cache_put(group, predicate, value, &_all.back());

	// Insert f into Unique<ExistsFeature> to ensure that f is unique (i.e. it has not already been constructed).	
	Unique<ExistsFeature>::insert(f);
}

/// Ensure that this feature is semantically valid.
void ExistsFeature::sanity_check() const {
	sanity_check_item_group_ty(_group);
	sanity_check_item_predicate_ty(_predicate);

	sanity_check_predicate_and_value(_predicate, _value);

	assert(_group != ALL_ITEMS);

	if (!parameter::context_groups_for_exists_features()) {
		assert(_group != ALL_LCONTEXT_ITEMS);
		assert(_group != ALL_RCONTEXT_ITEMS);
	}
}

/// \todo Assert that headword doesn't contain doublequotes.
const string ExistsFeature::to_string() const {
	this->sanity_check();

	string s;
	s += "some_item";
	s += "(" + item_group_string(_group) + ")";
	s += ".";
	s += predicate_value_string(_predicate, _value);

	return s;
}

bool ExistsFeature::apply(const IntermediateExample& e) const {
#ifdef SANITY_CHECKS
	this->sanity_check();
#endif /* SANITY_CHECKS */

	for (GroupIterator i(_group, e); !i.end(); i++)
		if (predicate_holds(_predicate, _value, *i))
			return true;

	return false;
}

FeatureIDs ExistsFeatures::get_slow(const IntermediateExample& e) const {
	FeatureIDs l;

	const FeaturePtrs& allf = ExistsFeatures::instance()->all();
	FeaturePtrs::const_iterator i;
	for (i = allf.begin(); i != allf.end(); i++) {
		const Feature& f = **i;
		if (f(e))
			l.push_back(f.id());
	}

	return l;
}

FeatureIDs ExistsFeatures::get(const IntermediateExample& e) const {
	FeatureIDs l;

	ExistsFeatures::get(e, l);

#ifdef SANITY_CHECKS
	FeatureIDs slow_l = this->get_slow(e);

/*
	if (l.size() != slow_l.size()) {
		Debug::log(1) << "l size = " << l.size() << "\n";
		Debug::log(1) << "slowl size = " << slow_l.size() << "\n";
		hash_set<ID<Feature> > h(l.begin(), l.end());
		hash_set<ID<Feature> > h_slow(slow_l.begin(), slow_l.end());
		
		hash_set<ID<Feature> >::const_iterator i;
		for (i = h.begin(); i != h.end(); i++)
			if (h_slow.find(*i) == h_slow.end()) {
				Debug::log(1) << "Not in slow: " << Features::instance()->from_id(*i).to_string() << "\n";
			}
		for (i = h_slow.begin(); i != h_slow.end(); i++)
			if (h.find(*i) == h.end()) {
				Debug::log(1) << "Not in fast: " << Features::instance()->from_id(*i).to_string() << "\n";
			}
	}
	*/
	
	assert(l.size() == slow_l.size());
	hash_set<ID<Feature> > h(slow_l.begin(), slow_l.end());
	FeatureIDs::const_iterator i;
	for (i = l.begin(); i != l.end(); i++)
		assert(h.find(*i) != h.end());
#endif /* SANITY_CHECKS */

	return l;
}

void ExistsFeatures::get(const IntermediateExample& e, FeatureIDs& l) const {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		ExistsFeatures::get(e, l, *i);
}

void ExistsFeatures::get(const IntermediateExample& e, FeatureIDs& l, item_predicate_ty predicate) const {
	LIST<item_group_ty>::const_iterator i;
	for (i = all_item_groups().begin(); i != all_item_groups().end(); i++) {
		if (*i == ALL_ITEMS) continue;
		if (!parameter::context_groups_for_exists_features() &&
				(*i == ALL_LCONTEXT_ITEMS || *i == ALL_RCONTEXT_ITEMS))
			continue;
		ExistsFeatures::get(e, l, predicate, *i);
	}
}

void ExistsFeatures::get(const IntermediateExample& e, FeatureIDs& l, item_predicate_ty predicate, item_group_ty group) const {
	ItemTokens items;

	unsigned values_size = max_predicate_value(predicate)+1;
	vector<bool> exists_features(values_size, false);
	deque<unsigned> value_list;

	unsigned v;
	GroupIterator item(group, e);
	int size = item.size();
	for (; !item.end(); item++) {
		v = get_predicate_value(predicate, *item);
		if (is_binary_predicate(predicate) && v == (unsigned)false && !parameter::binary_predicates_can_have_false_value()) continue;
		exists_features.at(v) = true;
		if (size*2 < (int)values_size)
			value_list.push_back(v);
	}

	if (size*2 < (int)values_size) {
		deque<unsigned>::const_iterator i;
		for (i = value_list.begin(); i != value_list.end(); i++)
			if (exists_features.at(*i)) {
				l.push_back(cache_get(group, predicate, *i)->id());
				exists_features.at(*i) = false;
			}
	} else {
		unsigned i;
		for (i = 0; i < exists_features.size(); i++)
			if (exists_features.at(i))
				l.push_back(cache_get(group, predicate, i)->id());
	}
}
