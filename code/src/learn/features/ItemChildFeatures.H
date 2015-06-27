/*  $Id: ItemChildFeatures.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ItemChildFeatures.H
 *  \brief WRITEME
 *
 *  WRITEME
 *
 *  \todo Make some of these methods const? or return const?
 *  \todo Use Singleton pattern (inherit from Singleton<T>).
 *  \todo Assert that no to_string() method returns a string containing whitespace
 *  \todo Optimize by explicitly writing get() method for subtypes of Features?
 *  \todo Use virtual destructors.
 *
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#ifndef __ItemChildFeatures_H__
#define  __ItemChildFeatures_H__

#include "learn/features/Features.H"
#include "learn/features/features-help.H"

#include "universal/AutoVector.H"

class ItemChildFeature : public Feature {
public:
	/// \todo If we're done constructing all ItemChildFeatures, this constructor *should* construct
	/// the canonical object (i.e. with correct ID<Feature>).
	ItemChildFeature(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate, unsigned value) {
		_which = which;
		_where = where;
		_offset = offset;
		_child_where = child_where;
		_child_offset = child_offset;
		_predicate = predicate;
		_value = value;
		this->sanity_check();
	}

	virtual const string to_string() const;

	bool operator==(const ItemChildFeature& f) const {
		return (_which == f._which && _where == f._where && _offset == f._offset && _child_where == f._child_where && _child_offset == f._child_offset && _predicate == f._predicate && _value == f._value);
	}
	size_t hash() const {
		return feature_values_hash(_which, _where, _offset, _child_where, _child_offset, _predicate, _value);
	}

protected:
	virtual bool apply(const IntermediateExample& e) const;
	virtual bool operator()(const IntermediateExample& e) const { return this->apply(e); }

private:
	/// Ensure that this feature is semantically valid.
	void sanity_check() const;

	which_span_ty _which;
	span_where_ty _where;
	int _offset;
	span_where_ty _child_where;
	int _child_offset;
	item_predicate_ty _predicate;
	unsigned _value;
};

class ItemChildFeatures {
	/// \todo REMOVEME?
	friend class Features;
public:
	static const ItemChildFeatures* instance();

	const FeaturePtrs& all() const;

protected:
	FeatureIDs get(const IntermediateExample& e) const;

	/// Protected for Singleton pattern.
	ItemChildFeatures();
private:
	static void construct();
	static void construct(which_span_ty which);
	static void construct(which_span_ty which, span_where_ty where);
	static void construct(which_span_ty which, span_where_ty where, int offset);
	static void construct(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where);
	static void construct(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset);
	static void construct(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate);
	
	static void new_feature(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate, unsigned value);

	/// \todo Should these be static?
	void get(const IntermediateExample& e, FeatureIDs& l) const;
	void get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which) const;
	void get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which, span_where_ty where) const;
	void get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset) const;
	void get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset, span_where_ty child_where) const;
	void get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& child_item, which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset) const;
	void get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& child_item, which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate) const;

	FeatureIDs get_slow(const IntermediateExample& e) const;
	
	static void cache_put(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate, unsigned value, const ItemChildFeature* ptr);
	const ItemChildFeature* cache_get(which_span_ty which, span_where_ty where, int offset, span_where_ty child_where, int child_offset, item_predicate_ty predicate, unsigned value) const;

	static ItemChildFeatures* _instance;
	
	/// Storage of all ItemChildFeature objects.
	static list<ItemChildFeature> _all;
	
	/// Pointers to all ItemChildFeature objects.
	static FeaturePtrs _all_ptrs;
	
	/// Fast construction/lookup of the ptr to a ItemChildFeature.
	static AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<const ItemChildFeature*> > > > > > > _cache;
};


#ifndef DOXYGEN
namespace __gnu_cxx {
	template<> class hash<ItemChildFeature> {
		public: size_t operator() (ItemChildFeature f) const{ return f.hash(); }
	};
}
#endif /* DOXYGEN */


#endif /* __ItemChildFeatures_H__ */
