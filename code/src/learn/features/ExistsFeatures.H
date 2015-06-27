/*  $Id: ExistsFeatures.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ExistsFeatures.H
 *  \brief Degenerate quantifier features, allowing only THERE_EXISTS (not FOR_ALL or NOT_ALL).
 *
 *  WRITEME
 *
 *  \todo Make some of these methods const? or return const?
 *  \todo Assert that no to_string() method returns a string containing whitespace
 *  \todo Optimize by explicitly writing get() method for subtypes of Features?
 *  \todo Use virtual destructors.
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#ifndef __ExistsFeatures_H__
#define  __ExistsFeatures_H__

#include "learn/features/Features.H"
#include "learn/features/features-help.H"

#include "universal/AutoVector.H"

class ExistsFeature : public Feature {
public:
	/// \todo If we're done constructing all ExistsFeatures, this constructor *should* construct
	/// the canonical object (i.e. with correct ID<Feature>).
	ExistsFeature(item_group_ty group, item_predicate_ty predicate, unsigned value) {
		_group = group;
		_predicate = predicate;
		_value = value;
		this->sanity_check();
	}

	virtual const string to_string() const;

	bool operator==(const ExistsFeature& f) const {
		return (_group == f._group && _predicate == f._predicate && _value == f._value);
	}
	size_t hash() const {
		return feature_values_hash(_group, _predicate, _value);
	}

protected:
	virtual bool apply(const IntermediateExample& e) const;
	virtual bool operator()(const IntermediateExample& e) const { return this->apply(e); }

private:
	/// Ensure that this feature is semantically valid.
	void sanity_check() const;

	item_group_ty _group;
	item_predicate_ty _predicate;
	unsigned _value;
};

class ExistsFeatures {
	/// \todo REMOVEME?
	friend class Features;
public:
	static const ExistsFeatures* instance();

	const FeaturePtrs& all() const;

protected:
	FeatureIDs get(const IntermediateExample& e) const;

	/// Protected for Singleton pattern.
	ExistsFeatures();
private:
	static void construct();
	static void construct(item_group_ty group);
	static void construct(item_group_ty group, item_predicate_ty predicate);
	
	static void new_feature(item_group_ty group, item_predicate_ty predicate, unsigned value);

	void get(const IntermediateExample& e, FeatureIDs& l) const;
	void get(const IntermediateExample& e, FeatureIDs& l, item_predicate_ty predicate) const;
	void get(const IntermediateExample& e, FeatureIDs& l, item_predicate_ty predicate, item_group_ty group) const;

	FeatureIDs get_slow(const IntermediateExample& e) const;
	
	static void cache_put(item_group_ty group, item_predicate_ty predicate, unsigned value, const ExistsFeature* ptr);
	const ExistsFeature* cache_get(item_group_ty group, item_predicate_ty predicate, unsigned value) const;

	static ExistsFeatures* _instance;
	
	/// Storage of all ExistsFeature objects.
	static list<ExistsFeature> _all;
	
	/// Pointers to all ExistsFeature objects.
	static FeaturePtrs _all_ptrs;
	
	/// Fast construction/lookup of the ptr to a ExistsFeature.
	static AutoVector<AutoVector<AutoVector<const ExistsFeature*> > > _cache;
};


#ifndef DOXYGEN
namespace __gnu_cxx {
	template<> class hash<ExistsFeature> {
		public: size_t operator() (ExistsFeature f) const{ return f.hash(); }
	};
}
#endif /* DOXYGEN */


#endif /* __ExistsFeatures_H__ */
