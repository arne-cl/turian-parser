/*  $Id: ExistsTerminalFeatures.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file ExistsTerminalFeatures.H
 *  \brief Exists features over the *terminals* dominated by some group of items.
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

#ifndef __ExistsTerminalFeatures_H__
#define  __ExistsTerminalFeatures_H__

#include "learn/features/Features.H"
#include "learn/features/features-help.H"

#include "universal/AutoVector.H"

class ExistsTerminalFeature : public Feature {
public:
	/// \todo If we're done constructing all ExistsTerminalFeatures, this constructor *should* construct
	/// the canonical object (i.e. with correct ID<Feature>).
	ExistsTerminalFeature(item_group_ty group, terminal_predicate_ty predicate, unsigned value) {
		_group = group;
		_predicate = predicate;
		_value = value;
		this->sanity_check();
	}

	virtual const string to_string() const;

	bool operator==(const ExistsTerminalFeature& f) const {
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
	terminal_predicate_ty _predicate;
	unsigned _value;
};

class ExistsTerminalFeatures {
	/// \todo REMOVEME?
	friend class Features;
public:
	static const ExistsTerminalFeatures* instance();

	const FeaturePtrs& all() const;

protected:
	FeatureIDs get(const IntermediateExample& e) const;

	/// Protected for Singleton pattern.
	ExistsTerminalFeatures();
private:
	static void construct();
	static void construct(item_group_ty group);
	static void construct(item_group_ty group, terminal_predicate_ty predicate);
	
	static void new_feature(item_group_ty group, terminal_predicate_ty predicate, unsigned value);

	void get(const IntermediateExample& e, FeatureIDs& l) const;
	void get(const IntermediateExample& e, FeatureIDs& l, terminal_predicate_ty predicate) const;
	void get(const IntermediateExample& e, FeatureIDs& l, terminal_predicate_ty predicate, item_group_ty group) const;

	FeatureIDs get_slow(const IntermediateExample& e) const;
	
	static void cache_put(item_group_ty group, terminal_predicate_ty predicate, unsigned value, const ExistsTerminalFeature* ptr);
	const ExistsTerminalFeature* cache_get(item_group_ty group, terminal_predicate_ty predicate, unsigned value) const;

	static ExistsTerminalFeatures* _instance;
	
	/// Storage of all ExistsTerminalFeature objects.
	static list<ExistsTerminalFeature> _all;
	
	/// Pointers to all ExistsTerminalFeature objects.
	static FeaturePtrs _all_ptrs;
	
	/// Fast construction/lookup of the ptr to a ExistsTerminalFeature.
	static AutoVector<AutoVector<AutoVector<const ExistsTerminalFeature*> > > _cache;
};


#ifndef DOXYGEN
namespace __gnu_cxx {
	template<> class hash<ExistsTerminalFeature> {
		public: size_t operator() (ExistsTerminalFeature f) const{ return f.hash(); }
	};
}
#endif /* DOXYGEN */


#endif /* __ExistsTerminalFeatures_H__ */
