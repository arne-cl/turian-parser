/*  $Id: Features.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Features.H
 *  \brief WRITEME
 *
 *  WRITEME
 *
 *  \todo Fix feature to_string() methods s.t. they allow whitespace. Then,
 *  fix BuildNode.C and Tree.C and BoostTokenizer.ll.
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

#ifndef __Features_H__
#define  __Features_H__

#include "universal/AutoVector.H"
#include "universal/ID.H"
#include "universal/LIST.H"

#include "universal/globals.H"

#include <deque>
#include <list>		// REMOVEME
#include <ext/hash_map>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

class IntermediateExample;

/// WRITEME
/// \todo Make some of these methods static?
class Feature {
	/// \todo REMOVEME?
	friend class ExistsFeatures;
	friend class ExistsTerminalFeatures;
	friend class ItemChildFeatures;
	friend class ItemFeatures;
	friend class LengthFeatures;
	friend class QuantFeatures;
	friend class RangeFeatures;
	friend class TerminalItemFeatures;

	friend class IntermediateExample;

public:
	Feature() { assert(_id.empty()); }

	virtual const string to_string() const = 0;

	void newID() {
		assert(_id.empty());
		_id.create();
		assert(!_id.empty());
	}
	
	const ID<Feature>& id() const {
		assert(!_id.empty());
		return _id;
	}

protected:
	virtual bool apply(const IntermediateExample& e) const = 0;
	virtual bool operator()(const IntermediateExample& e) const = 0;

private:
	ID<Feature> _id;
};

/// In practice, list is much slower than deque and vector, since list wastes a lot of time allocating memory.
/// \todo Compare vector and deque again.
class FeatureIDs : public deque<ID<Feature> > {
};
//typedef deque<ID<Feature> > FeatureIDs;
//typedef vector<ID<Feature> > FeatureIDs;
//typedef LIST<ID<Feature> > FeatureIDs;

/// A distinguished "empty" ID used by google::dense_hash_set.
/// NB this is also distinct from ID<T>.empty()
extern ID<Feature> EMPTY_FEATURE_ID;

typedef const Feature* FeaturePtr;
//typedef list<FeaturePtr> FeaturePtrs;
typedef LIST<FeaturePtr> FeaturePtrs;

/// WRITEME
/// \todo Maybe add init(), to specify which features are valid (not just syntactically)
/// \todo Make some of these methods const? or return const?
/// \todo Add () operator?
/// \todo Use Singleton pattern (inherit from Singleton<T>).
/// \todo Somewhere, debug output #features constructed
class Features {
	/// \todo REMOVEME?
	friend class IntermediateExample;
public:
	static const Features* instance();

	// /// \todo Use slist?
	//const FeatureIDs& all() const;

	/// Return the number of features.
	unsigned count() const;

	/// \todo Maybe return a ptr?
	const Feature& from_id(const ID<Feature>& id) const;

	ID<Feature> of_string(const string s) const;

	/// Free the string -> ID<Feature> map.
	void free_string_map() const;

protected:
	/// Protected for Singleton pattern.
	Features();

	/// Get all FeatureIDs over which IntermediateExample e holds.
	FeatureIDs get(const IntermediateExample& e) const;

private:
	/// Create the string -> ID<Feature> map.
	void create_string_map() const;

	static Features* _instance;
	static FeaturePtrs _all;
	static AutoVector<FeaturePtr> _id_to_ptr;

	/// \todo This structure consumes a lot of memory.
	/// Destroy this hash when we're done with it.
	static hash_map<string, ID<Feature>, hash<string>, equal_to<string> > _string_map;
};

#endif /* __Features_H__ */
