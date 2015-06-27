/*  $Id: LengthFeatures.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file LengthFeatures.H
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

#ifndef __LengthFeatures_H__
#define  __LengthFeatures_H__

#include "learn/features/Features.H"
#include "learn/features/features-help.H"

#include "universal/AutoVector.H"

class LengthFeature : public Feature {
public:
	/// \todo If we're done constructing all LengthFeatures, this constructor *should* construct
	/// the canonical object (i.e. with correct ID<Feature>).
	LengthFeature(item_group_ty group, length_count_ty count, length_cmp_ty cmp, unsigned len) {
		_group = group;
		_count = count;
		_cmp = cmp;
		_len = len;
		this->sanity_check();
	}

	virtual const string to_string() const;

	bool operator==(const LengthFeature& f) const {
		return (_group == f._group && _count == f._count &&
				_cmp == f._cmp && _len == f._len);
	}
	size_t hash() const {
		return feature_values_hash(_group, _count, _cmp, _len);
	}

protected:
	virtual bool apply(const IntermediateExample& e) const;
	virtual bool operator()(const IntermediateExample& e) const { return this->apply(e); }

private:
	/// Ensure that this feature is semantically valid.
	void sanity_check() const;

	item_group_ty _group;
	length_count_ty _count;
	length_cmp_ty _cmp;
	unsigned _len;
};

class LengthFeatures {
	/// \todo REMOVEME?
	friend class Features;
public:
	static const LengthFeatures* instance();

	const FeaturePtrs& all() const;

protected:
	FeatureIDs get(const IntermediateExample& e) const;

	/// Protected for Singleton pattern.
	LengthFeatures();
private:
	static void construct();
	static void construct(item_group_ty group);
	static void construct(item_group_ty group, length_count_ty count);
	static void construct(item_group_ty group, length_count_ty count, length_cmp_ty cmp);
	
	static void new_feature(item_group_ty group, length_count_ty count, length_cmp_ty cmp, unsigned len);
	
	static void cache_put(item_group_ty group, length_count_ty count, length_cmp_ty cmp, unsigned len, const LengthFeature* ptr);
	const LengthFeature* cache_get(item_group_ty group, length_count_ty count, length_cmp_ty cmp, unsigned len) const;

	FeatureIDs get_slow(const IntermediateExample& e) const;

	void get(const IntermediateExample& e, FeatureIDs& l) const;
	void get(const IntermediateExample& e, FeatureIDs& l, item_group_ty group) const;
	void get(const IntermediateExample& e, FeatureIDs& l, unsigned len, item_group_ty group, length_count_ty count) const;

	static LengthFeatures* _instance;
	
	/// Storage of all LengthFeature objects.
	static list<LengthFeature> _all;
	
	/// Pointers to all LengthFeature objects.
	static FeaturePtrs _all_ptrs;
	
	/// Fast construction/lookup of the ptr to a LengthFeature.
	static AutoVector<AutoVector<AutoVector<AutoVector<const LengthFeature*> > > > _cache;
};

#ifndef DOXYGEN
namespace __gnu_cxx {
	template<> class hash<LengthFeature> {
		public: size_t operator() (LengthFeature f) const{ return f.hash(); }
	};
}
#endif /* DOXYGEN */


#endif /* __LengthFeatures_H__ */
