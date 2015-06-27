/*  $Id: ItemChildFeatures.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ItemChildFeatures.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/ItemChildFeatures.H"

#include "learn/examples/IntermediateExample.H"

#include "parse/ItemToken.H"

#include "universal/parameter.H"
#include "universal/posclass.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/Unique.H"

ItemChildFeatures* ItemChildFeatures::_instance = NULL;

list<ItemChildFeature> ItemChildFeatures::_all = list<ItemChildFeature>();
FeaturePtrs ItemChildFeatures::_all_ptrs = FeaturePtrs();
AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<const ItemChildFeature*> > > > > > > ItemChildFeatures::_cache = AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<const ItemChildFeature*> > > > > > >();

const ItemChildFeatures* ItemChildFeatures::instance() {
	if (!_instance) {
		_instance = new ItemChildFeatures;
		assert(_instance);
	}
	return _instance;
}

const FeaturePtrs& ItemChildFeatures::all() const {
	return _all_ptrs;
}
	
void ItemChildFeatures::cache_put(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate, unsigned value, const ItemChildFeature* ptr) {
	// Add parameter::max_offset() to offset s.t. we will never have a negative index.
	_cache[which][where][offset+parameter::max_offset()][child_where][child_offset+parameter::max_child_offset()][predicate][value] = ptr;
}

const ItemChildFeature* ItemChildFeatures::cache_get(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate, unsigned value) const {
	// Add parameter::max_offset() to offset s.t. we will never have a negative index.
	return _cache.at(which).at(where).at(offset+parameter::max_offset()).at(child_where).at(child_offset+parameter::max_child_offset()).at(predicate).at(value);
}

ItemChildFeatures::ItemChildFeatures() {
	assert(_all.empty());
	assert(_all_ptrs.empty());
	// Assert _cache is empty?
	
	ItemChildFeatures::construct();
	_cache.lock();

	sanity_check_uniq(_all);
	assert(_all.size() == _all_ptrs.size());

	Debug::log(2) << "ItemChildFeatures::ItemChildFeatures() built " << _all.size() << " features\n";
}

void ItemChildFeatures::construct() {
//	LIST<which_span_ty>::const_iterator i;
//	for (i = all_which_span().begin(); i != all_which_span().end(); i++)
//		ItemChildFeatures::construct(*i);
	ItemChildFeatures::construct(SPAN);
}

void ItemChildFeatures::construct(which_span_ty which) {
	LIST<span_where_ty>::const_iterator i;
	for (i = all_span_where(which).begin(); i != all_span_where(which).end(); i++)
		ItemChildFeatures::construct(which, *i);
}

void ItemChildFeatures::construct(which_span_ty which, span_where_ty where) {
	LIST<int>::const_iterator i;
	for (i = all_where_offsets(where).begin(); i != all_where_offsets(where).end(); i++) {
//		if (abs(*i) > 1)
//			continue;
		if (abs(*i) > 0)
			continue;
		ItemChildFeatures::construct(which, where, *i);
	}
}

void ItemChildFeatures::construct(which_span_ty which, span_where_ty where, int offset) {
	LIST<span_where_ty>::const_iterator i;
	for (i = all_span_where().begin(); i != all_span_where().end(); i++)
		ItemChildFeatures::construct(which, where, offset, *i);
}

void ItemChildFeatures::construct(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where) {
	LIST<int>::const_iterator i;
	for (i = all_child_where_offsets(child_where).begin(); i != all_child_where_offsets(child_where).end(); i++)
		ItemChildFeatures::construct(which, where, offset, child_where, *i);
}

void ItemChildFeatures::construct(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset) {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++) {
		if (!parameter::construct_redundant_features())
			if (*i == HEADWORD_PREDICATE) continue;
		ItemChildFeatures::construct(which, where, offset, child_where, child_offset, *i);
	}
}

void ItemChildFeatures::construct(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate) {
	const LIST<unsigned>& l = all_predicate_values(predicate);
	for (LIST<unsigned>::const_iterator i = l.begin(); i != l.end(); i++)
		if (!is_binary_predicate(predicate) || *i != (unsigned)false || parameter::binary_predicates_can_have_false_value())
			ItemChildFeatures::new_feature(which, where, offset, child_where, child_offset, predicate, *i);
}

void ItemChildFeatures::new_feature(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate, unsigned value) {
	ItemChildFeature f(which, where, offset, child_where, child_offset, predicate, value);
	f.newID();
	_all.push_back(f);
	_all_ptrs.push_back(&_all.back());
	cache_put(which, where, offset, child_where, child_offset, predicate, value, &_all.back());

	// Insert f into Unique<ItemChildFeature> to ensure that f is unique (i.e. it has not already been constructed).	
	Unique<ItemChildFeature>::insert(f);
}

/// Ensure that this feature is semantically valid.
void ItemChildFeature::sanity_check() const {
	sanity_check_which_span_ty(_which);
	sanity_check_span_where_ty(_where);
	sanity_check_which_span_where_ty(_which, _where);
	sanity_check_where_offset(_where, _offset);
	sanity_check_span_where_ty(_child_where);
	sanity_check_child_where_offset(_child_where, _child_offset);
	sanity_check_item_predicate_ty(_predicate);
	sanity_check_predicate_and_value(_predicate, _value);
	if (!parameter::construct_redundant_features())
		assert(_predicate != HEADWORD_PREDICATE);
}

/// \todo Assert that headword doesn't contain doublequotes.
const string ItemChildFeature::to_string() const {
	this->sanity_check();

	string s;
	s += which_span_string(_which);
	s += "[" + where_offset_string(_where, _offset) + "]";
	s += ".children";
	s += "[" + where_offset_string(_child_where, _child_offset) + "]";
	s += ".";
	s += predicate_value_string(_predicate, _value);
	return s;
}

bool ItemChildFeature::apply(const IntermediateExample& e) const {
#ifdef SANITY_CHECKS
	this->sanity_check();
#endif /* SANITY_CHECKS */

	const ItemToken* item = get_which_where_offset_item(_which, _where, _offset, e);
	if (!item) return false;
	const ItemToken* child_item = get_child_where_offset_item(_child_where, _child_offset, *item);
	if (!child_item) return false;
	else return predicate_holds(_predicate, _value, *child_item);
}

FeatureIDs ItemChildFeatures::get_slow(const IntermediateExample& e) const {
	FeatureIDs l;

	const FeaturePtrs& allf = ItemChildFeatures::instance()->all();
	FeaturePtrs::const_iterator i;
	for (i = allf.begin(); i != allf.end(); i++) {
		const Feature& f = **i;
		if (f(e))
			l.push_back(f.id());
	}

	return l;
}

FeatureIDs ItemChildFeatures::get(const IntermediateExample& e) const {
	FeatureIDs l;

	ItemChildFeatures::get(e, l);

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
void ItemChildFeatures::get(const IntermediateExample& e, FeatureIDs& l) const {
//	LIST<which_span_ty>::const_iterator i;
//	for (i = all_which_span().begin(); i != all_which_span().end(); i++)
//		ItemChildFeatures::get(e, l, *i);
	ItemChildFeatures::get(e, l, SPAN);
}

void ItemChildFeatures::get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which) const {
	LIST<span_where_ty>::const_iterator i;
	for (i = all_span_where(which).begin(); i != all_span_where(which).end(); i++)
		ItemChildFeatures::get(e, l, which, *i);
}

void ItemChildFeatures::get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which, span_where_ty where) const {
	LIST<int>::const_iterator i;
	for (i = all_where_offsets(where).begin(); i != all_where_offsets(where).end(); i++) {
		const int& offset = *i;
//		if (abs(offset) > 1)
//			continue;
		if (abs(offset) > 0)
			continue;
		const ItemToken* item = get_which_where_offset_item(which, where, offset, e);
		if (!item) continue;
		else ItemChildFeatures::get(e, l, *item, which, where, offset);
	}
}

void ItemChildFeatures::get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset) const {
	LIST<span_where_ty>::const_iterator i;
	for (i = all_span_where().begin(); i != all_span_where().end(); i++)
		ItemChildFeatures::get(e, l, item, which, where, offset, *i);
}

void ItemChildFeatures::get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset, span_where_ty child_where) const {
	LIST<int>::const_iterator i;
	for (i = all_child_where_offsets(child_where).begin(); i != all_child_where_offsets(child_where).end(); i++) {
		const int& child_offset = *i;
		const ItemToken* child_item = get_child_where_offset_item(child_where, child_offset, item);
		if (!child_item) continue;
		else ItemChildFeatures::get(e, l, *child_item, which, where, offset, child_where, child_offset);
	}
}

void ItemChildFeatures::get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& child_item, which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset) const {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++) {
		if (*i == HEADWORD_PREDICATE) continue;
		ItemChildFeatures::get(e, l, child_item, which, where, offset, child_where, child_offset, *i);
	}
}

void ItemChildFeatures::get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& child_item, which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate) const {
	unsigned v = get_predicate_value(predicate, child_item);
	if (is_binary_predicate(predicate) && v == (unsigned)false && !parameter::binary_predicates_can_have_false_value()) return;
	l.push_back(cache_get(which, where, offset, child_where, child_offset, predicate, v)->id());
}
