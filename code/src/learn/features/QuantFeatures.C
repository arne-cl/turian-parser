/*  $Id: QuantFeatures.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file QuantFeatures.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/QuantFeatures.H"

#include "learn/examples/IntermediateExample.H"

#include "parse/ItemToken.H"

#include "universal/parameter.H"
#include "universal/posclass.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/Unique.H"

QuantFeatures* QuantFeatures::_instance = NULL;

list<QuantFeature> QuantFeatures::_all = list<QuantFeature>();
FeaturePtrs QuantFeatures::_all_ptrs = FeaturePtrs();
AutoVector<AutoVector<AutoVector<AutoVector<const QuantFeature*> > > > QuantFeatures::_cache = AutoVector<AutoVector<AutoVector<AutoVector<const QuantFeature*> > > >();

const QuantFeatures* QuantFeatures::instance() {
	if (!_instance) {
		_instance = new QuantFeatures;
		assert(_instance);
	}
	return _instance;
}

const FeaturePtrs& QuantFeatures::all() const {
	return _all_ptrs;
}

void QuantFeatures::cache_put(quant_ty quant, item_group_ty group, item_predicate_ty predicate, unsigned value, const QuantFeature* ptr) {
	_cache[quant][group][predicate][value] = ptr;
}

const QuantFeature* QuantFeatures::cache_get(quant_ty quant, item_group_ty group, item_predicate_ty predicate, unsigned value) const {
	return _cache.at(quant).at(group).at(predicate).at(value);
}

QuantFeatures::QuantFeatures() {
	assert(_all.empty());
	assert(_all_ptrs.empty());
	// Assert _cache is empty?
	
	QuantFeatures::construct();
	_cache.lock();

	sanity_check_uniq(_all);
	assert(_all.size() == _all_ptrs.size());

	Debug::log(2) << "QuantFeatures::QuantFeatures() built " << _all.size() << " features\n";
}

void QuantFeatures::construct() {
	QuantFeatures::construct(THERE_EXISTS);
	QuantFeatures::construct(NOT_ALL);
}

void QuantFeatures::construct(quant_ty quant) {
	LIST<item_group_ty>::const_iterator i;
	for (i = all_item_groups().begin(); i != all_item_groups().end(); i++)
		QuantFeatures::construct(quant, *i);
}

void QuantFeatures::construct(quant_ty quant, item_group_ty group) {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		QuantFeatures::construct(quant, group, *i);
}

void QuantFeatures::construct(quant_ty quant, item_group_ty group, item_predicate_ty predicate) {
	const LIST<unsigned>& l = all_predicate_values(predicate);
	for (LIST<unsigned>::const_iterator i = l.begin(); i != l.end(); i++)
		if (!is_binary_predicate(predicate) || *i != (unsigned)false || parameter::binary_predicates_can_have_false_value())
			QuantFeatures::new_feature(quant, group, predicate, *i);
}

void QuantFeatures::new_feature(quant_ty quant, item_group_ty group, item_predicate_ty predicate, unsigned value) {
	QuantFeature f(quant, group, predicate, value);
	f.newID();
	_all.push_back(f);
	_all_ptrs.push_back(&_all.back());
	cache_put(quant, group, predicate, value, &_all.back());

	// Insert f into Unique<QuantFeature> to ensure that f is unique (i.e. it has not already been constructed).	
	Unique<QuantFeature>::insert(f);
}

/// Ensure that this feature is semantically valid.
void QuantFeature::sanity_check() const {
	sanity_check_quant_ty(_quant);
	sanity_check_item_group_ty(_group);
	sanity_check_item_predicate_ty(_predicate);

	sanity_check_predicate_and_value(_predicate, _value);
}

/// \todo Assert that headword doesn't contain doublequotes.
const string QuantFeature::to_string() const {
	this->sanity_check();

	string s;
	switch (_quant) {
		case THERE_EXISTS: s += "some_item"; break;
		case NOT_ALL: s += "not_all_items"; break;
		default: assert(0);
	}
	s += "(" + item_group_string(_group) + ")";
	s += ".";
	s += predicate_value_string(_predicate, _value);

	return s;
}

bool QuantFeature::apply(const IntermediateExample& e) const {
#ifdef SANITY_CHECKS
	this->sanity_check();
#endif /* SANITY_CHECKS */

	ItemTokens items;
	get_group_items(_group, e, items);

	ItemTokens::const_iterator i;
	for (i = items.begin(); i != items.end(); i++) {
		bool this_holds = predicate_holds(_predicate, _value, **i);

		switch (_quant) {
			case THERE_EXISTS:
				if (this_holds)
					return true;
				break;

			case NOT_ALL:
				if (!this_holds)
					return true;
				break;

			default: assert(0);
		}
	}

	return false;
}

FeatureIDs QuantFeatures::get_slow(const IntermediateExample& e) const {
	FeatureIDs l;

	const FeaturePtrs& allf = QuantFeatures::instance()->all();
	FeaturePtrs::const_iterator i;
	for (i = allf.begin(); i != allf.end(); i++) {
		const Feature& f = **i;
		if (f(e))
			l.push_back(f.id());
	}

	return l;
}

FeatureIDs QuantFeatures::get(const IntermediateExample& e) const {
	FeatureIDs l;

	QuantFeatures::get(e, l);

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

void QuantFeatures::get(const IntermediateExample& e, FeatureIDs& l) const {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		QuantFeatures::get(e, l, *i);
}

void QuantFeatures::get(const IntermediateExample& e, FeatureIDs& l, item_predicate_ty predicate) const {
	LIST<item_group_ty>::const_iterator i;
	for (i = all_item_groups().begin(); i != all_item_groups().end(); i++)
		QuantFeatures::get(e, l, predicate, *i);
}

void QuantFeatures::get(const IntermediateExample& e, FeatureIDs& l, item_predicate_ty predicate, item_group_ty group) const {
	ItemTokens items;
	get_group_items(group, e, items);

	unsigned values_size = max_predicate_value(predicate)+1;
	vector<bool> exists_features(values_size, false), not_all_features(values_size, false);

	ItemTokens::iterator item;
	for (item = items.begin(); item != items.end(); item++)
		QuantFeatures::get(**item, predicate, exists_features, not_all_features);

	unsigned i;
	for (i = 0; i < exists_features.size(); i++) {
		if (exists_features.at(i))
			l.push_back(cache_get(THERE_EXISTS, group, predicate, i)->id());
		if (not_all_features.at(i))
			l.push_back(cache_get(NOT_ALL, group, predicate, i)->id());
	}
}

/// \todo Do we really want special case handling for boolean predicates?
/// Reason in favor of special case handling for boolean predicates:
///	- We don't consider redundant features. Only one suffices.
/// Reason against special case handling for boolean predicates:
///	- Cleaner, simpler code.
void QuantFeatures::get(const ItemToken& item, item_predicate_ty predicate, vector<bool>& exists_features, vector<bool>& not_all_features) const {
	const LIST<unsigned>& all = all_predicate_values(predicate);
	unsigned value = get_predicate_value(predicate, item);

	for (LIST<unsigned>::const_iterator i = all.begin(); i != all.end(); i++) {
		if (!is_binary_predicate(predicate) || *i != (unsigned)false || parameter::binary_predicates_can_have_false_value()) {
			if (*i == value)
				exists_features.at(*i) = true;
			else
				not_all_features.at(*i) = true;
		}
	}
}
