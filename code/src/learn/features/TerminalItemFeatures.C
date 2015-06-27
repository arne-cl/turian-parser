/*  $Id: TerminalItemFeatures.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file TerminalItemFeatures.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/features/TerminalItemFeatures.H"

#include "learn/examples/IntermediateExample.H"

#include "parse/ItemToken.H"

#include "universal/parameter.H"
#include "universal/posclass.H"
#include "universal/vocabulary.H"
#include "universal/Debug.H"
#include "universal/Unique.H"

TerminalItemFeatures* TerminalItemFeatures::_instance = NULL;

list<TerminalItemFeature> TerminalItemFeatures::_all = list<TerminalItemFeature>();
FeaturePtrs TerminalItemFeatures::_all_ptrs = FeaturePtrs();
AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<const TerminalItemFeature*> > > > > TerminalItemFeatures::_cache = AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<const TerminalItemFeature*> > > > >();

const TerminalItemFeatures* TerminalItemFeatures::instance() {
	if (!_instance) {
		_instance = new TerminalItemFeatures;
		assert(_instance);
	}
	return _instance;
}

const FeaturePtrs& TerminalItemFeatures::all() const {
	return _all_ptrs;
}
	
/// \todo Use parameter::max_terminal_context_offset() instead of parameter::max_terminal_offset() iff
/// which == LCONTEXT or which == RCONTEXT
void TerminalItemFeatures::cache_put(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value, const TerminalItemFeature* ptr) {
	// Add parameter::max_terminal_offset() to offset s.t. we will never have a negative index.
	_cache[which][where][offset+parameter::max_terminal_offset()][predicate][value] = ptr;
}

/// \todo Use parameter::max_terminal_context_offset() instead of parameter::max_terminal_offset() iff
/// which == LCONTEXT or which == RCONTEXT
const TerminalItemFeature* TerminalItemFeatures::cache_get(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value) const {
	// Add parameter::max_terminal_offset() to offset s.t. we will never have a negative index.
	return _cache.at(which).at(where).at(offset+parameter::max_terminal_offset()).at(predicate).at(value);
}

TerminalItemFeatures::TerminalItemFeatures() {
	assert(_all.empty());
	assert(_all_ptrs.empty());
	// Assert _cache is empty?
	
	TerminalItemFeatures::construct();
	_cache.lock();

	sanity_check_uniq(_all);
	assert(_all.size() == _all_ptrs.size());

	Debug::log(2) << "TerminalItemFeatures::TerminalItemFeatures() built " << _all.size() << " features\n";
}

void TerminalItemFeatures::construct() {
	LIST<which_span_ty>::const_iterator i;
	for (i = all_which_span().begin(); i != all_which_span().end(); i++)
		TerminalItemFeatures::construct(*i);
}

void TerminalItemFeatures::construct(which_span_ty which) {
	LIST<span_where_ty>::const_iterator i;
	for (i = all_span_where(which).begin(); i != all_span_where(which).end(); i++)
		TerminalItemFeatures::construct(which, *i);
}

void TerminalItemFeatures::construct(which_span_ty which, span_where_ty where) {
	LIST<int>::const_iterator i;
	for (i = all_where_offsets(where).begin(); i != all_where_offsets(where).end(); i++) {
		// Skip context terminals that are beyond max_terminal_context_offset
		if ((which == LCONTEXT || which == RCONTEXT) &&
				abs(*i) > parameter::max_terminal_context_offset())
			continue;
		// Skip span terminals that are beyond max_terminal_offset
		if (which == SPAN && abs(*i) > parameter::max_terminal_offset())
			continue;

		TerminalItemFeatures::construct(which, where, *i);
	}
}

void TerminalItemFeatures::construct(which_span_ty which, span_where_ty where, int offset) {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		TerminalItemFeatures::construct(which, where, offset, *i);
}

void TerminalItemFeatures::construct(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate) {
	const LIST<unsigned>& l = all_predicate_values(predicate);
	for (LIST<unsigned>::const_iterator i = l.begin(); i != l.end(); i++)
		if (!is_binary_predicate(predicate) || *i != (unsigned)false || parameter::binary_predicates_can_have_false_value())
			TerminalItemFeatures::new_feature(which, where, offset, predicate, *i);
}

void TerminalItemFeatures::new_feature(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value) {
	TerminalItemFeature f(which, where, offset, predicate, value);
	f.newID();
	_all.push_back(f);
	_all_ptrs.push_back(&_all.back());
	cache_put(which, where, offset, predicate, value, &_all.back());

	// Insert f into Unique<TerminalItemFeature> to ensure that f is unique (i.e. it has not already been constructed).	
	Unique<TerminalItemFeature>::insert(f);
}

/// Ensure that this feature is semantically valid.
void TerminalItemFeature::sanity_check() const {
	sanity_check_which_span_ty(_which);
	sanity_check_span_where_ty(_where);
	sanity_check_which_span_where_ty(_which, _where);
	sanity_check_item_predicate_ty(_predicate);
	sanity_check_terminal_where_offset(_where, _offset);
	sanity_check_predicate_and_value(_predicate, _value);

	// Skip context items that are beyond max_terminal_context_offset
	if (_which == LCONTEXT || _which == RCONTEXT)
		assert(abs(_offset) <= parameter::max_terminal_context_offset());
}

/// \todo Assert that headword doesn't contain doublequotes.
const string TerminalItemFeature::to_string() const {
	this->sanity_check();

	string s;
	s += which_span_string(_which);
	s += ".terminals";
	s += "[" + where_offset_string(_where, _offset) + "]";
	s += ".";
	s += predicate_value_string(_predicate, _value);
	return s;
}

bool TerminalItemFeature::apply(const IntermediateExample& e) const {
#ifdef SANITY_CHECKS
	this->sanity_check();
#endif /* SANITY_CHECKS */

	const ItemToken* terminal = get_which_where_offset_terminal(_which, _where, _offset, e);
	if (!terminal) return false;
	else return predicate_holds(_predicate, _value, *terminal);
}

FeatureIDs TerminalItemFeatures::get_slow(const IntermediateExample& e) const {
	FeatureIDs l;

	const FeaturePtrs& allf = TerminalItemFeatures::instance()->all();
	FeaturePtrs::const_iterator i;
	for (i = allf.begin(); i != allf.end(); i++) {
		const Feature& f = **i;
		if (f(e))
			l.push_back(f.id());
	}

	return l;
}

FeatureIDs TerminalItemFeatures::get(const IntermediateExample& e) const {
	FeatureIDs l;

	TerminalItemFeatures::get(e, l);

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
void TerminalItemFeatures::get(const IntermediateExample& e, FeatureIDs& l) const {
	LIST<which_span_ty>::const_iterator i;
	for (i = all_which_span().begin(); i != all_which_span().end(); i++)
		TerminalItemFeatures::get(e, l, *i);
}

void TerminalItemFeatures::get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which) const {
	LIST<span_where_ty>::const_iterator i;
	for (i = all_span_where(which).begin(); i != all_span_where(which).end(); i++)
		TerminalItemFeatures::get(e, l, which, *i);
}

void TerminalItemFeatures::get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which, span_where_ty where) const {
	LIST<int>::const_iterator i;
	for (i = all_where_offsets(where).begin(); i != all_where_offsets(where).end(); i++) {
		// Skip context terminals that are beyond max_terminal_context_offset
		if ((which == LCONTEXT || which == RCONTEXT) &&
				abs(*i) > parameter::max_terminal_context_offset())
			continue;
		// Skip span terminals that are beyond max_terminal_offset
		if (which == SPAN && abs(*i) > parameter::max_terminal_offset())
			continue;

		const int& offset = *i;
		const ItemToken* terminal = get_which_where_offset_terminal(which, where, offset, e);
		if (!terminal) continue;
		else TerminalItemFeatures::get(e, l, *terminal, which, where, offset);
	}
}

void TerminalItemFeatures::get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset) const {
	LIST<item_predicate_ty>::const_iterator i;
	for (i = all_item_predicates().begin(); i != all_item_predicates().end(); i++)
		TerminalItemFeatures::get(e, l, item, which, where, offset, *i);
}

void TerminalItemFeatures::get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate) const {
	unsigned v = get_predicate_value(predicate, item);
	if (is_binary_predicate(predicate) && v == (unsigned)false && !parameter::binary_predicates_can_have_false_value()) return;
	l.push_back(cache_get(which, where, offset, predicate, v)->id());
}
