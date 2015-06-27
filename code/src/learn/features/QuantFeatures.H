/*  $Id: QuantFeatures.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file QuantFeatures.H
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

#ifndef __QuantFeatures_H__
#define  __QuantFeatures_H__

#include "learn/features/Features.H"
#include "learn/features/features-help.H"

#include "universal/AutoVector.H"

class QuantFeature : public Feature {
public:
	/// \todo If we're done constructing all QuantFeatures, this constructor *should* construct
	/// the canonical object (i.e. with correct ID<Feature>).
	QuantFeature(quant_ty quant, item_group_ty group, item_predicate_ty predicate, unsigned value) {
		_quant = quant;
		_group = group;
		_predicate = predicate;
		_value = value;
		this->sanity_check();
	}

	virtual const string to_string() const;

	bool operator==(const QuantFeature& f) const {
		return (_quant == f._quant && _group == f._group && _predicate == f._predicate && _value == f._value);
	}
	size_t hash() const {
		return feature_values_hash(_quant, _group, _predicate, _value);
	}

protected:
	virtual bool apply(const IntermediateExample& e) const;
	virtual bool operator()(const IntermediateExample& e) const { return this->apply(e); }

private:
	/// Ensure that this feature is semantically valid.
	void sanity_check() const;

	quant_ty _quant;
	item_group_ty _group;
	item_predicate_ty _predicate;
	unsigned _value;
};

/// \todo Use Singleton pattern (inherit from Singleton<T>).
class QuantFeatures {
	/// \todo REMOVEME?
	friend class Features;
public:
	static const QuantFeatures* instance();

	const FeaturePtrs& all() const;

protected:
	FeatureIDs get(const IntermediateExample& e) const;

	/// Protected for Singleton pattern.
	QuantFeatures();
private:
	static void construct();
	static void construct(quant_ty quant);
	static void construct(quant_ty quant, item_group_ty group);
	static void construct(quant_ty quant, item_group_ty group, item_predicate_ty predicate);
	
	static void new_feature(quant_ty quant, item_group_ty group, item_predicate_ty predicate, unsigned value);

	void get(const IntermediateExample& e, FeatureIDs& l) const;
	void get(const IntermediateExample& e, FeatureIDs& l, item_predicate_ty predicate) const;
	void get(const IntermediateExample& e, FeatureIDs& l, item_predicate_ty predicate, item_group_ty group) const;
	void get(const ItemToken& item, item_predicate_ty predicate, vector<bool>& exists_features, vector<bool>& not_all_features) const;

	FeatureIDs get_slow(const IntermediateExample& e) const;
	
	static void cache_put(quant_ty quant, item_group_ty group, item_predicate_ty predicate, unsigned value, const QuantFeature* ptr);
	const QuantFeature* cache_get(quant_ty quant, item_group_ty group, item_predicate_ty predicate, unsigned value) const;

	static QuantFeatures* _instance;
	
	/// Storage of all QuantFeature objects.
	static list<QuantFeature> _all;
	
	/// Pointers to all QuantFeature objects.
	static FeaturePtrs _all_ptrs;
	
	/// Fast construction/lookup of the ptr to a QuantFeature.
	static AutoVector<AutoVector<AutoVector<AutoVector<const QuantFeature*> > > > _cache;
};


#ifndef DOXYGEN
namespace __gnu_cxx {
	template<> class hash<QuantFeature> {
		public: size_t operator() (QuantFeature f) const{ return f.hash(); }
	};
}
#endif /* DOXYGEN */


#endif /* __QuantFeatures_H__ */
