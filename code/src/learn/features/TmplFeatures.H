/*  $Id: TmplFeatures.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file TmplFeatures.H
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

#ifndef __TmplFeatures_H__
#define  __TmplFeatures_H__

#include "learn/features/Features.H"
#include "learn/features/features-help.H"

#include "universal/AutoVector.H"

class TmplFeature : public Feature {
public:
	/// \todo WRITEME
	/// \todo If we're done constructing all TmplFeatures, this constructor *should* construct
	/// the canonical object (i.e. with correct ID<Feature>).
	TmplFeature(...) {
		assert(0);
		this->sanity_check();
	}

	virtual const string to_string() const;

	/// \todo WRITEME
	bool operator==(const TmplFeature& f) const {
		assert(0);
	}
	/// \todo WRITEME
	size_t hash() const {
		return feature_values_hash(...);
		assert(0);
	}

protected:
	virtual bool apply(const IntermediateExample& e) const;
	virtual bool operator()(const IntermediateExample& e) const { return this->apply(e); }

private:
	/// Ensure that this feature is semantically valid.
	void sanity_check() const;
};

class TmplFeatures {
	/// \todo REMOVEME?
	friend class Features;
public:
	static const TmplFeatures* instance();

	const FeaturePtrs& all() const;

protected:
	FeatureIDs get(const IntermediateExample& e) const;

	/// Protected for Singleton pattern.
	TmplFeatures();
private:
	static void construct();
	
	static void new_feature(...);
	
	static void cache_put(..., const TmplFeature* ptr);
	const TmplFeature* cache_get(...) const;

	static TmplFeatures* _instance;
	
	/// Storage of all TmplFeature objects.
	static list<TmplFeature> _all;
	
	/// Pointers to all TmplFeature objects.
	static FeaturePtrs _all_ptrs;
	
	/// Fast construction/lookup of the ptr to a TmplFeature.
	static AutoVector<AutoVector<AutoVector<AutoVector<const TmplFeature*> > > > _cache;
};


#ifndef DOXYGEN
namespace __gnu_cxx {
	template<> class hash<TmplFeature> {
		public: size_t operator() (TmplFeature f) const{ return f.hash(); }
	};
}
#endif /* DOXYGEN */


#endif /* __TmplFeatures_H__ */
