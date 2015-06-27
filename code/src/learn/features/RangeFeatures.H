/*  $Id: RangeFeatures.H 1657 2006-06-04 03:03:05Z turian $
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file RangeFeatures.H
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

#ifndef __RangeFeatures_H__
#define  __RangeFeatures_H__

#include "learn/features/Features.H"
#include "learn/features/features-help.H"

#include "universal/AutoVector.H"

class RangeFeature : public Feature {
public:
	/// \todo If we're done constructing all RangeFeatures, this constructor *should* construct
	/// the canonical object (i.e. with correct ID<Feature>).
	RangeFeature(range_group_ty group, unsigned range, item_predicate_ty predicate, unsigned value) {
		_group = group;
		_range = range;
		_predicate = predicate;
		_value = value;
		this->sanity_check();
	}

	virtual const string to_string() const;

	bool operator==(const RangeFeature& f) const {
		return (_group == f._group && _range == f._range && _predicate == f._predicate && _value == f._value);
	}
	size_t hash() const {
		return feature_values_hash(_group, _range, _predicate, _value);
	}

protected:
	virtual bool apply(const IntermediateExample& e) const;
	virtual bool operator()(const IntermediateExample& e) const { return this->apply(e); }

private:
	/// Ensure that this feature is semantically valid.
	void sanity_check() const;

	range_group_ty _group;
	unsigned _range;
	item_predicate_ty _predicate;
	unsigned _value;
};

class RangeFeatures {
	/// \todo REMOVEME?
	friend class Features;
public:
	static const RangeFeatures* instance();

	const FeaturePtrs& all() const;

protected:
	FeatureIDs get(const IntermediateExample& e) const;

	/// Protected for Singleton pattern.
	RangeFeatures();
private:
	static void construct();
	static void construct(range_group_ty group);
	static void construct(range_group_ty group, unsigned range);
	static void construct(range_group_ty group, unsigned range, item_predicate_ty predicate);
	
	static void new_feature(range_group_ty group, unsigned range, item_predicate_ty predicate, unsigned value);

	/// \todo Should these be static?
	void get(const IntermediateExample& e, FeatureIDs& l) const;
	void get(const IntermediateExample& e, FeatureIDs& l, range_group_ty group) const;
	void get(const IntermediateExample& e, FeatureIDs& l, range_group_ty group, unsigned range) const;
	void get(const IntermediateExample& e, FeatureIDs& l, range_group_ty group, unsigned range, item_predicate_ty predicate) const;

	FeatureIDs get_slow(const IntermediateExample& e) const;
	
	static void cache_put(range_group_ty group, unsigned range, item_predicate_ty predicate, unsigned value, const RangeFeature* ptr);
	const RangeFeature* cache_get(range_group_ty group, unsigned range, item_predicate_ty predicate, unsigned value) const;

	static RangeFeatures* _instance;
	
	/// Storage of all RangeFeature objects.
	static list<RangeFeature> _all;
	
	/// Pointers to all RangeFeature objects.
	static FeaturePtrs _all_ptrs;
	
	/// Fast construction/lookup of the ptr to a RangeFeature.
	static AutoVector<AutoVector<AutoVector<AutoVector<const RangeFeature*> > > > _cache;
};


#ifndef DOXYGEN
namespace __gnu_cxx {
	template<> class hash<RangeFeature> {
		public: size_t operator() (RangeFeature f) const{ return f.hash(); }
	};
}
#endif /* DOXYGEN */


#endif /* __RangeFeatures_H__ */
