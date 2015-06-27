/*  $Id: ItemFeatures.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ItemFeatures.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/ItemFeatures.H"

#include "learn/examples/IntermediateExample.H"

#include "parse/ItemToken.H"

#include "universal/parameter.H"
#include "universal/posclass.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/Unique.H"

ItemFeatures* ItemFeatures::_instance = NULL;

list<ItemFeature> ItemFeatures::_all = list<ItemFeature>();
FeaturePtrs ItemFeatures::_all_ptrs = FeaturePtrs();
AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<const ItemFeature*> > > > > ItemFeatures::_cache = AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<const ItemFeature*> > > > >();

const ItemFeatures* ItemFeatures::instance() {
	if (!_instance) {
		_instance = new ItemFeatures;
		assert(_instance);
	}
	return _instance;
}

const FeaturePtrs& ItemFeatures::all() const {
	return _all_ptrs;
}
	
/// \todo Use parameter::max_context_offset() instead of parameter::max_offset() iff
/// which == LCONTEXT or which == RCONTEXT
void ItemFeatures::cache_put(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value, const ItemFeature* ptr) {
	// Add parameter::max_offset() to offset s.t. we will never have a negative index.
	_cache[which][where][offset+parameter::max_offset()][predicate][value] = ptr;
}

/// \todo Use parameter::max_context_offset() instead of parameter::max_offset() iff
/// which == LCONTEXT or which == RCONTEXT
const ItemFeature* ItemFeatures::cache_get(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value) const {
	// Add parameter::max_offset() to offset s.t. we will never have a negative index.
	return _cache.at(which).at(where).at(offset+parameter::max_offset()).at(predicate).at(value);
}

ItemFeatures::ItemFeatures() {
	assert(_all.empty());
	assert(_all_ptrs.empty());
	// Assert _cache is empty?
	
	ItemFeatures::construct();
	_cache.lock();

	sanity_check_uniq(_all);
	assert(_all.size() == _all_ptrs.size());

	Debug::log(2) << "ItemFeatures::ItemFeatures() built " << _all.size() << " features\n";
}

void ItemFeatures::construct() {
	LIST<which_span_ty>::const_iterator i;
	for (i = all_which_span().begin(); i != all_which_span().end(); i++)
		ItemFeatures::construct(*i);
}

void ItemFeatures::construct(which_span_ty which) {
	LIST<span_where_ty>::const_iterator i;
	for (i = all_span_where(which).begin(); i != all_span_where(which).end(); i++)
		ItemFeatures::construct(which, *i);
}

void ItemFeatures::construct(which_span_ty which, span_where_ty where) {
	LIST<int>::const_iterator i;
	for (i = all_where_offsets(where).begin(); i != all_where_offsets(where).end(); i++) {
		// Skip context items that are beyond max_context_offset
		if ((which == LCONTEXT || which == RCONTEXT) &&
				abs(*i) > parameter::max_context_offset())
			continue;
		ItemFeatures::construct(which, where, *i);
	}
}

void ItemFeatures::construct(which_span_ty which, span_where_ty where, int offset) {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		ItemFeatures::construct(which, where, offset, *i);
}

void ItemFeatures::construct(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate) {
	const LIST<unsigned>& l = all_predicate_values(predicate);
	for (LIST<unsigned>::const_iterator i = l.begin(); i != l.end(); i++)
		if (!is_binary_predicate(predicate) || *i != (unsigned)false || parameter::binary_predicates_can_have_false_value())
			ItemFeatures::new_feature(which, where, offset, predicate, *i);
}

void ItemFeatures::new_feature(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value) {
	ItemFeature f(which, where, offset, predicate, value);
	f.newID();
	_all.push_back(f);
	_all_ptrs.push_back(&_all.back());
	cache_put(which, where, offset, predicate, value, &_all.back());

	// Insert f into Unique<ItemFeature> to ensure that f is unique (i.e. it has not already been constructed).	
	Unique<ItemFeature>::insert(f);
}

/// Ensure that this feature is semantically valid.
void ItemFeature::sanity_check() const {
	sanity_check_which_span_ty(_which);
	sanity_check_span_where_ty(_where);
	sanity_check_which_span_where_ty(_which, _where);
	sanity_check_item_predicate_ty(_predicate);
	sanity_check_where_offset(_where, _offset);
	sanity_check_predicate_and_value(_predicate, _value);

	// Skip context items that are beyond max_context_offset
	if (_which == LCONTEXT || _which == RCONTEXT)
		assert(abs(_offset) <= parameter::max_context_offset());
}

/// \todo Assert that headword doesn't contain doublequotes.
const string ItemFeature::to_string() const {
	this->sanity_check();

	string s;
	s += which_span_string(_which);
	s += "[" + where_offset_string(_where, _offset) + "]";
	s += ".";
	s += predicate_value_string(_predicate, _value);
	return s;
}

bool ItemFeature::apply(const IntermediateExample& e) const {
#ifdef SANITY_CHECKS
	this->sanity_check();
#endif /* SANITY_CHECKS */

	const ItemToken* item = get_which_where_offset_item(_which, _where, _offset, e);
	if (!item) return false;
	else return predicate_holds(_predicate, _value, *item);
}

FeatureIDs ItemFeatures::get_slow(const IntermediateExample& e) const {
	FeatureIDs l;

	const FeaturePtrs& allf = ItemFeatures::instance()->all();
	FeaturePtrs::const_iterator i;
	for (i = allf.begin(); i != allf.end(); i++) {
		const Feature& f = **i;
		if (f(e))
			l.push_back(f.id());
	}

	return l;
}

FeatureIDs ItemFeatures::get(const IntermediateExample& e) const {
	FeatureIDs l;

	ItemFeatures::get(e, l);

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
void ItemFeatures::get(const IntermediateExample& e, FeatureIDs& l) const {
	LIST<which_span_ty>::const_iterator i;
	for (i = all_which_span().begin(); i != all_which_span().end(); i++)
		ItemFeatures::get(e, l, *i);
}

void ItemFeatures::get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which) const {
	LIST<span_where_ty>::const_iterator i;
	for (i = all_span_where(which).begin(); i != all_span_where(which).end(); i++)
		ItemFeatures::get(e, l, which, *i);
}

void ItemFeatures::get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which, span_where_ty where) const {
	LIST<int>::const_iterator i;
	for (i = all_where_offsets(where).begin(); i != all_where_offsets(where).end(); i++) {
		// Skip context items that are beyond max_context_offset
		if ((which == LCONTEXT || which == RCONTEXT) &&
				abs(*i) > parameter::max_context_offset())
			continue;

		const int& offset = *i;
		const ItemToken* item = get_which_where_offset_item(which, where, offset, e);
		if (!item) continue;
		else ItemFeatures::get(e, l, *item, which, where, offset);
	}
}

void ItemFeatures::get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset) const {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		ItemFeatures::get(e, l, item, which, where, offset, *i);
}

void ItemFeatures::get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate) const {
	unsigned v = get_predicate_value(predicate, item);
	if (is_binary_predicate(predicate) && v == (unsigned)false && !parameter::binary_predicates_can_have_false_value()) return;
	l.push_back(cache_get(which, where, offset, predicate, v)->id());
}
