/*  $Id: LengthFeatures.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file LengthFeatures.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/LengthFeatures.H"
#include "learn/features/GroupIterator.H"

#include "learn/examples/IntermediateExample.H"

#include "universal/parameter.H"
#include "universal/Debug.H"
#include "universal/Unique.H"

#include <cassert>
#include <ostream>
#include <sstream>

LengthFeatures* LengthFeatures::_instance = NULL;

list<LengthFeature> LengthFeatures::_all = list<LengthFeature>();
FeaturePtrs LengthFeatures::_all_ptrs = FeaturePtrs();
AutoVector<AutoVector<AutoVector<AutoVector<const LengthFeature*> > > > LengthFeatures::_cache = AutoVector<AutoVector<AutoVector<AutoVector<const LengthFeature*> > > >();

const LengthFeatures* LengthFeatures::instance() {
	if (!_instance) {
		_instance = new LengthFeatures;
		assert(_instance);
	}
	return _instance;
}

const FeaturePtrs& LengthFeatures::all() const {
	return _all_ptrs;
}
	
void LengthFeatures::cache_put(item_group_ty group, length_count_ty count, length_cmp_ty cmp, unsigned len, const LengthFeature* ptr) {
	_cache[group][count][cmp][len] = ptr;
}

const LengthFeature* LengthFeatures::cache_get(item_group_ty group, length_count_ty count, length_cmp_ty cmp, unsigned len) const {
	assert (_cache.size());
	return _cache.at(group).at(count).at(cmp).at(len);
}

LengthFeatures::LengthFeatures() {
	assert(_all.empty());
	assert(_all_ptrs.empty());
	// Assert _cache is empty?
	
	LengthFeatures::construct();
	_cache.lock();
	
	sanity_check_uniq(_all);
	assert(_all.size() == _all_ptrs.size());

	Debug::log(2) << "LengthFeatures::LengthFeatures() built " << _all.size() << " features\n";
}

void LengthFeatures::construct() {
	LIST<item_group_ty>::const_iterator i;
	for (i = all_item_groups().begin(); i != all_item_groups().end(); i++)
		LengthFeatures::construct(*i);
}

void LengthFeatures::construct(item_group_ty group) {
	if (parameter::length_of_context_items_features() ||
			(group != ALL_ITEMS && group != ALL_LCONTEXT_ITEMS && group != ALL_RCONTEXT_ITEMS)) {
		if (!parameter::construct_redundant_features()) {
			// Skip redundant ITEM_COUNT groups.
			switch (group) {
				case NONLEFTMOST_SPAN_ITEMS:
				case NONRIGHTMOST_SPAN_ITEMS:
				case NONHEAD_SPAN_ITEMS:
				case HEAD_OR_LEFT_OF_HEAD_SPAN_ITEMS:
				case HEAD_OR_RIGHT_OF_HEAD_SPAN_ITEMS:
					break;

				default:
					LengthFeatures::construct(group, ITEM_COUNT);
			}
		} else {
			LengthFeatures::construct(group, ITEM_COUNT);
		}
	}

	if (parameter::use_terminal_features())
		LengthFeatures::construct(group, TERMINAL_COUNT);
}

void LengthFeatures::construct(item_group_ty group, length_count_ty count) {
	LengthFeatures::construct(group, count, GREATER_THAN);
	if (parameter::length_equal_to_feature())
		LengthFeatures::construct(group, count, EQUAL_TO);
}

/// \todo FIXME: Make sure that for span length queries, we don't query lengths greater than parameter::max_span_length()
void LengthFeatures::construct(item_group_ty group, length_count_ty count, length_cmp_ty cmp) {
	for (unsigned i = 0; i < LENGTH_VALUES_CNT; i++) {
		assert(LENGTH_VALUES[i] < LENGTH_VALUES[i+1]);
		LengthFeatures::new_feature(group, count, cmp, LENGTH_VALUES[i]);
	}
	assert(LENGTH_VALUES[LENGTH_VALUES_CNT] == NO_VALUE);
}

void LengthFeatures::new_feature(item_group_ty group, length_count_ty count, length_cmp_ty cmp, unsigned len) {
	LengthFeature f(group, count, cmp, len);
	f.newID();
	_all.push_back(f);
	assert(&_all.back());
	_all_ptrs.push_back(&_all.back());
	cache_put(group, count, cmp, len, &_all.back());

	// Insert f into Unique<LengthFeature> to ensure that f is unique (i.e. it has not already been constructed).	
	Unique<LengthFeature>::insert(f);
}

/// Ensure that this feature is semantically valid.
/// \todo FIXME: Make sure that for span length queries, we don't query lengths greater than parameter::max_span_length()
void LengthFeature::sanity_check() const {
	sanity_check_item_group_ty(_group);
	sanity_check_length_count_ty(_count);
	sanity_check_length_cmp_ty(_cmp);

	if (!parameter::construct_redundant_features() && _count == ITEM_COUNT) {
		assert (_group != NONLEFTMOST_SPAN_ITEMS);
		assert (_group != NONRIGHTMOST_SPAN_ITEMS);
		assert (_group != NONHEAD_SPAN_ITEMS);
		assert (_group != HEAD_OR_LEFT_OF_HEAD_SPAN_ITEMS);
		assert (_group != HEAD_OR_RIGHT_OF_HEAD_SPAN_ITEMS);
	}

	if (!parameter::length_of_context_items_features() && _count == ITEM_COUNT) {
		assert (_group != ALL_ITEMS);
		assert (_group != ALL_LCONTEXT_ITEMS);
		assert (_group != ALL_RCONTEXT_ITEMS);
	}

	bool found = false;
	for (unsigned i = 0; i < LENGTH_VALUES_CNT; i++) {
		assert(LENGTH_VALUES[i] != NO_VALUE);
		if (_len == LENGTH_VALUES[i]) {
			found = true;
			break;
		}
	}
	assert(found);
}

const string LengthFeature::to_string() const {
	ostringstream o;
	o << "length(" << item_group_string(_group);
	o << ".";
	switch (_count) {
		case ITEM_COUNT:
			o << "items"; break;
		case TERMINAL_COUNT:
			assert(parameter::use_terminal_features());
			o << "terminals";
			break;
		default: assert(0);
	}
	o << ")";
	switch (_cmp) {
		case GREATER_THAN: o << ">"; break;
		case EQUAL_TO: o << "=="; break;
		default: assert(0);
	}
	o << _len;
	return o.str();
}

bool LengthFeature::apply(const IntermediateExample& e) const {
#ifdef SANITY_CHECKS
	this->sanity_check();
#endif /* SANITY_CHECKS */

	unsigned len = 0;
	switch (_count) {
		case ITEM_COUNT: len = GroupIterator(_group, e).size(); break;
		case TERMINAL_COUNT:
				 assert(parameter::use_terminal_features());
				 for (GroupIterator i(_group, e); !i.end(); i++)
					 len += (*i).terminal_cnt();
				 break;
		default: assert(0);
	}

	switch (_cmp) {
		case GREATER_THAN: return len > _len;
		case EQUAL_TO: return len == _len;
		default: assert(0);
	}
	assert(0); return false;
}

FeatureIDs LengthFeatures::get_slow(const IntermediateExample& e) const {
	FeatureIDs l;

	const FeaturePtrs& allf = LengthFeatures::instance()->all();
	FeaturePtrs::const_iterator i;
	for (i = allf.begin(); i != allf.end(); i++) {
		const Feature& f = **i;
		if (f(e))
			l.push_back(f.id());
	}

	return l;
}

FeatureIDs LengthFeatures::get(const IntermediateExample& e) const {
	FeatureIDs l;

	LengthFeatures::instance()->get(e, l);

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

void LengthFeatures::get(const IntermediateExample& e, FeatureIDs& l) const {
	LIST<item_group_ty>::const_iterator i;
	for (i = all_item_groups().begin(); i != all_item_groups().end(); i++)
		LengthFeatures::get(e, l, *i);
}

void LengthFeatures::get(const IntermediateExample& e, FeatureIDs& l, item_group_ty group) const {
	unsigned item_cnt;
	unsigned terminal_cnt = 0;
	for (GroupIterator i(group, e); !i.end(); i++)
		terminal_cnt += (*i).terminal_cnt();

	if (parameter::length_of_context_items_features() ||
			(group != ALL_ITEMS && group != ALL_LCONTEXT_ITEMS && group != ALL_RCONTEXT_ITEMS)) {
		// Skip redundant ITEM_COUNT groups.
		switch (group) {
			case NONLEFTMOST_SPAN_ITEMS:
			case NONRIGHTMOST_SPAN_ITEMS:
			case NONHEAD_SPAN_ITEMS:
			case HEAD_OR_LEFT_OF_HEAD_SPAN_ITEMS:
			case HEAD_OR_RIGHT_OF_HEAD_SPAN_ITEMS:
				break;

			default:
				item_cnt = GroupIterator(group, e).size();
				LengthFeatures::get(e, l, item_cnt, group, ITEM_COUNT);
		}
	}

	if (parameter::use_terminal_features())
		LengthFeatures::get(e, l, terminal_cnt, group, TERMINAL_COUNT);
}

void LengthFeatures::get(const IntermediateExample& e, FeatureIDs& l, unsigned len, item_group_ty group, length_count_ty count) const {
	for (unsigned i = 0; i < LENGTH_VALUES_CNT; i++) {
		assert(LENGTH_VALUES[i] < LENGTH_VALUES[i+1]);
		if (len > LENGTH_VALUES[i])
			l.push_back(cache_get(group, count, GREATER_THAN, LENGTH_VALUES[i])->id());
		else if (len == LENGTH_VALUES[i] && parameter::length_equal_to_feature())
			l.push_back(cache_get(group, count, EQUAL_TO, LENGTH_VALUES[i])->id());
	}
	assert(LENGTH_VALUES[LENGTH_VALUES_CNT] == NO_VALUE);
}
