/*  $Id: ItemFeatures.H 1657 2006-06-04 03:03:05Z turian $
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ItemFeatures.H
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

#ifndef __ItemFeatures_H__
#define  __ItemFeatures_H__

#include "learn/features/Features.H"
#include "learn/features/features-help.H"

#include "universal/AutoVector.H"

class ItemFeature : public Feature {
public:
	/// \todo If we're done constructing all ItemFeatures, this constructor *should* construct
	/// the canonical object (i.e. with correct ID<Feature>).
	ItemFeature(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value) {
		_which = which;
		_where = where;
		_offset = offset;
		_predicate = predicate;
		_value = value;
		this->sanity_check();
	}

	virtual const string to_string() const;

	bool operator==(const ItemFeature& f) const {
		return (_which == f._which && _where == f._where && _offset == f._offset && _predicate == f._predicate && _value == f._value);
	}
	size_t hash() const {
		return feature_values_hash(_which, _where, _offset, _predicate, _value);
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
	item_predicate_ty _predicate;
	unsigned _value;
};

class ItemFeatures {
	/// \todo REMOVEME?
	friend class Features;
public:
	static const ItemFeatures* instance();

	const FeaturePtrs& all() const;

protected:
	FeatureIDs get(const IntermediateExample& e) const;

	/// Protected for Singleton pattern.
	ItemFeatures();
private:
	static void construct();
	static void construct(which_span_ty which);
	static void construct(which_span_ty which, span_where_ty where);
	static void construct(which_span_ty which, span_where_ty where, int offset);
	static void construct(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate);
	
	static void new_feature(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value);

	/// \todo Should these be static?
	void get(const IntermediateExample& e, FeatureIDs& l) const;
	void get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which) const;
	void get(const IntermediateExample& e, FeatureIDs& l, which_span_ty which, span_where_ty where) const;
	void get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset) const;
	void get(const IntermediateExample& e, FeatureIDs& l, const ItemToken& item, which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate) const;

	FeatureIDs get_slow(const IntermediateExample& e) const;
	
	static void cache_put(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value, const ItemFeature* ptr);
	const ItemFeature* cache_get(which_span_ty which, span_where_ty where, int offset, item_predicate_ty predicate, unsigned value) const;

	static ItemFeatures* _instance;
	
	/// Storage of all ItemFeature objects.
	static list<ItemFeature> _all;
	
	/// Pointers to all ItemFeature objects.
	static FeaturePtrs _all_ptrs;
	
	/// Fast construction/lookup of the ptr to a ItemFeature.
	static AutoVector<AutoVector<AutoVector<AutoVector<AutoVector<const ItemFeature*> > > > > _cache;
};


#ifndef DOXYGEN
namespace __gnu_cxx {
	template<> class hash<ItemFeature> {
		public: size_t operator() (ItemFeature f) const{ return f.hash(); }
	};
}
#endif /* DOXYGEN */


#endif /* __ItemFeatures_H__ */
