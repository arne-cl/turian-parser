/*  $Id: Sampler.C 1657 2006-06-04 03:03:05Z turian $	
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Sampler.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/examples/Sampler.H"

#include "learn/examples/Example.H"
#include "learn/examples/FullExample.H"

#include "learn/loss.H"

#include "universal/NBest.H"
#include "universal/stats.H"

template<typename EXAMPLE> class SamplePriority {
public:
	SamplePriority() : _empty(true) { }

	/// ...
	/// \todo Use a Random type for random numbers.
	SamplePriority(const EXAMPLE& e) : _empty(false), _example(e) {
		_priority = _example.weight() / drand48();
	}

	bool operator<(const SamplePriority& p) const { return this->priority() < p.priority(); }
	bool operator>(const SamplePriority& p) const { return this->priority() > p.priority(); }
	bool operator==(const SamplePriority& p) const { return this->priority() == p.priority(); }
	bool operator!=(const SamplePriority& p) const { return this->priority() != p.priority(); }

	const Double& priority() const { assert(!this->empty()); return _priority; }
//	const Double& true_weight() const { assert(!this->empty()); return _true_weight; }
	const EXAMPLE& example() const { assert(!this->empty()); return _example; }

	bool empty() const { return _empty; }

	/// We assume that all objects are distinct i.e.
	/// that we cannot detect if the two Example%s are equal.
	bool object_equal_to(const SamplePriority& p) const {
		//assert(this->example() != p.example());
		return false;
	}

private:
	bool _empty;
	Double _priority;
//	Double _true_weight;
	EXAMPLE _example;
};

/// Sample a set of examples.
/// The sampling method is based upon "Sampling to estimate
/// arbitrary subset sums" (Duffield et al.):
/// "we consider a [stream] of items i=0,...,n-1 with weights
/// wi. For each item i, we generate a random number ri in
/// (0,1) and create a priority qi=wi/ri. The sample S consists
/// of the k highest priority items. Let t be the (k+1)th
/// highest priority. Each sampled item i in S gets a weight
/// estimate Wi=max{wi,t}, while non-sampled items get weight
/// estimate Wi=0."
/// \arg yvalue Required y-value for each example sampled.
/// \arg maxn The maximum number of examples to sample.
/// \post *this will contain at most maxn yvalue'd examples.
template<typename EXAMPLE> void Sampler<EXAMPLE>::sample(const Examples<EXAMPLE>& exmpls, bool yvalue, unsigned maxn) {
	string ystr = "+";
	if (!yvalue) ystr = "-";
	Debug::log(2) << "Sampling at most " << maxn << " " << ystr << "examples at " << stats::current_time() << "...\n";

	// debg mesaage 
	NBest<SamplePriority<EXAMPLE> > sample(maxn+1);

	unsigned tot = 0;
	Double totweight(0);
	for (typename Examples<EXAMPLE>::const_iterator ex = exmpls.begin(); ex != exmpls.end(); ex++) {
		if (ex->is_correct() != yvalue) continue;

		tot++;
		totweight += ex->weight();

		if (tot % 100000 == 0 && !sample.empty())
			Debug::log(3) << "\tsourced " << tot << " examples (min_weight = " << sample.bottom().priority() << ") at " << stats::current_time() << "...\n";
		sample.push(*ex);
	}

	// If the sample isn't full
	if (!sample.full()) {
		// then keep all examples
		Debug::log(2) << "Using only " << sample.size() << " " << ystr << "examples (less than maxn=" << maxn << ") at " << stats::current_time() << "\n";
		this->reserve(this->size() + sample.size());
		while(!sample.empty()) {
			this->push_back(sample.top().example());
			sample.pop();
		}
		// and break
		return;
	}

	assert((int)sample.size()-1 > 0);
	unsigned n = maxn;

	unsigned origsize = this->size();
	this->reserve(origsize + n);
	for(unsigned i = 0; i < n; i++) {
		assert(!sample.empty());
		this->push_back(sample.top().example());
		sample.pop();
	}
	assert(!sample.empty());
	double min_weight = sample.top().priority()();
	sample.pop();
	assert(sample.empty());

	assert(this->size() == origsize + n);
	unsigned replaced = 0;
	Double trueweight;
	Double keepweight;
	Double replacedweight;
	for(unsigned i = origsize; i < this->size(); i++) {
		trueweight += this->at(i).weight();
		if(this->at(i).weight() < min_weight) {
			this->at(i).set_weight(min_weight);
			replaced++;
			replacedweight += this->at(i).weight();
		}
		keepweight += this->at(i).weight();
	}

	assert(this->size() == this->capacity());

	Debug::log(2) << "...done sampling " << n << " of " << tot << " " << ystr << "examples at " << stats::current_time() << "\n";
	Debug::log(2) << "\tSampling lost " << (totweight-trueweight)/totweight*100 << "% of " << ystr << "example weight\n";
	Debug::log(2) << "\tReplaced " << replaced << " " << ystr << "examples (" << 100. * replaced/n << "%) with min_weight " << min_weight << "\n";
	Debug::log(3) << "\tTotal " << ystr << "example weight, prior to sampling = " << totweight << "\n";
	Debug::log(3) << "\tReplace " << ystr << "example weight = " << replacedweight << "\n";
	Debug::log(3) << "\tSample " << ystr << "example weight = " << keepweight << "\n";
	Debug::log(3) << "\tOriginal weight of " << ystr << "examples in sample = " << trueweight << "\n";
	Debug::log(2) << stats::resource_usage() << "\n";
}


template<typename EXAMPLE> void Sampler<EXAMPLE>::sample(const Examples<EXAMPLE>& exmpls, unsigned maxn) {
	Debug::log(2) << "Sampling at most " << maxn << " examples at " << stats::current_time() << "...\n";

	// debg mesaage 
	NBest<SamplePriority<EXAMPLE> > sample(maxn+1);

	unsigned tot = 0;
	Double totweight(0);
	for (typename Examples<EXAMPLE>::const_iterator ex = exmpls.begin(); ex != exmpls.end(); ex++) {
		tot++;
		totweight += ex->weight();

		if (tot % 100000 == 0 && !sample.empty())
			Debug::log(3) << "\tsourced " << tot << " examples (min_weight = " << sample.bottom().priority() << ") at " << stats::current_time() << "...\n";
		sample.push(*ex);
	}

	// If the sample isn't full
	if (!sample.full()) {
		// then keep all examples
		Debug::log(2) << "Using only " << sample.size() << " examples (less than maxn=" << maxn << ") at " << stats::current_time() << "\n";
		this->reserve(this->size() + sample.size());
		while(!sample.empty()) {
			this->push_back(sample.top().example());
			sample.pop();
		}
		// and break
		return;
	}

	assert((int)sample.size()-1 > 0);
	unsigned n = maxn;

	unsigned origsize = this->size();
	this->reserve(origsize + n);
	for(unsigned i = 0; i < n; i++) {
		assert(!sample.empty());
		this->push_back(sample.top().example());
		sample.pop();
	}
	assert(!sample.empty());
	double min_weight = sample.top().priority()();
	sample.pop();
	assert(sample.empty());

	assert(this->size() == origsize + n);
	unsigned replaced = 0;
	Double trueweight;
	Double keepweight;
	Double replacedweight;
	for(unsigned i = origsize; i < this->size(); i++) {
		trueweight += this->at(i).weight();
		if(this->at(i).weight() < min_weight) {
			this->at(i).set_weight(min_weight);
			replaced++;
			replacedweight += this->at(i).weight();
		}
		keepweight += this->at(i).weight();
	}

	assert(this->size() == this->capacity());

	Debug::log(2) << "...done sampling " << n << " of " << tot << " examples at " << stats::current_time() << "\n";
	Debug::log(2) << "\tSampling lost " << (totweight-trueweight)/totweight*100 << "% of example weight\n";
	Debug::log(2) << "\tReplaced " << replaced << " examples (" << 100. * replaced/n << "%) with min_weight " << min_weight << "\n";
	Debug::log(3) << "\tTotal example weight, prior to sampling = " << totweight << "\n";
	Debug::log(3) << "\tReplace example weight = " << replacedweight << "\n";
	Debug::log(3) << "\tSample example weight = " << keepweight << "\n";
	Debug::log(3) << "\tOriginal weight of examples in sample = " << trueweight << "\n";
	Debug::log(2) << stats::resource_usage() << "\n";
}

/// Sample a set of at most n examples.
/// \todo Use Random instance to choose random numbers for std::random_shuffle.
template<typename EXAMPLE> Sampler<EXAMPLE>::Sampler(const Examples<EXAMPLE>& exmpls, unsigned n) {
	Debug::log(2) << "Sampling " << n << " " << EXAMPLE::name() << "s at " << stats::current_time() << "...\n";

	/*
	assert(n%2 == 0);
	this->sample(exmpls, true, n/2);
	this->sample(exmpls, false, n/2);
	*/

	this->sample(exmpls, n);

	// Shuffle the examples.
	random_shuffle(this->begin(), this->end());
}

// TEMPLATE INSTANTIATIONS
template Sampler<SparseFullExample>::Sampler(Examples<SparseFullExample> const&, unsigned int);
template Sampler<Example>::Sampler(Examples<Example> const&, unsigned int);
