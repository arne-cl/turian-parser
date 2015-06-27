/*  $Id: Ensemble-tune.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Ensemble-tune.C
 *  \brief Ensemble methods for tuning confidence computations for speed.
 *
 *  Automagically tune the classifier to the fastest method available.
 *  We also have methods that---given the results of tuning---will make
 *  the appropriate conversions (thus implementing the tuning).
 *  
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/decision-trees/Ensemble.H"

#include "learn/examples/Example.H"
#include "learn/examples/FullExample.H"
#include "learn/examples/IntermediateExample.H"
#include "learn/examples/Examples.H"

#include "universal/Random.H"
#include "universal/Debug.H"
#include "universal/stats.H"

/// Compute the confidence of an Example from scratch.
/// If the Ensemble has been tuned (using tune()),
/// we will convert the Example to the appropriate type before
/// computing its confidence.
template<typename EXAMPLE>
double Ensemble::confidence_compute(const EXAMPLE& e) const {
	if (_use_full_examples.empty()) {
		ostringstream o;
		o << "This ensemble (" << this << ") has not been tuned! Converting " << EXAMPLE::name() << " to IntermediateExample nonetheless...";
		Debug::warning(__FILE__, __LINE__, o.str());
		return this->confidence_compute_convert<EXAMPLE, IntermediateExample>(e);
	} else if (_use_full_examples()) {
		return this->confidence_compute_convert<EXAMPLE, DenseFullExample>(e);
	} else {
		assert(_use_full_examples == false);
		return this->confidence_compute_convert<EXAMPLE, IntermediateExample>(e);
	}
}

/// Set Example confidences, converting from ORIGEXAMPLE to WORKEXAMPLE
/// before computing confidences.
/// \param exmpls The Example%s whose confidences should be updated.
template<typename ORIGEXAMPLE, typename WORKEXAMPLE>
double Ensemble::confidence_compute_convert(const ORIGEXAMPLE& e) const {
	if (same<ORIGEXAMPLE, WORKEXAMPLE>::yes)
		// No need for conversion in this case.
		return this->confidence_compute_work(e);
	else
		return this->confidence_compute_work(WORKEXAMPLE(e));
}
/// This should not be instantiated.
/// \todo Should be a STATIC assertion.
/// \todo There are more (e.g. * -> Example, but I'm too lazy to do them).
template<> double Ensemble::confidence_compute_convert<Example, Example>(const Example& e) const {
	//BOOST_STATIC_ASSERT(false);
	assert(0);
}
template<> double Ensemble::confidence_compute_convert<DenseFullExample, IntermediateExample>(const DenseFullExample& e) const {
	//BOOST_STATIC_ASSERT(false);
	assert(0);
}
template<> double Ensemble::confidence_compute_convert<SparseFullExample, IntermediateExample>(const SparseFullExample& e) const {
	//BOOST_STATIC_ASSERT(false);
	assert(0);
}

/// Empirically determine whether it is faster to compute confidences over
/// DenseFullExample%s or IntermediateExample%s.
/// We use a sample of EXAMPLES_FOR_AUTOMAGIC_TUNING examples for
/// this automagic tuning.
/// \post Set ::_use_full_examples iff DenseFullExample%s are
/// faster than IntermediateExample%s.
/// \return The value of ::_use_full_examples.
/// \todo Compute variance and add that to the mean time, to reduce the
/// risk of bad worst-case times.
/// \todo Use Random instance to choose random numbers for std::random_shuffle.
template<typename EXAMPLE> const Optional<bool>& Ensemble::tune(const Examples<EXAMPLE>& exmpls) const {
	// Have we already tuned this Ensemble?
	if (!_use_full_examples.empty()) {
		ostringstream o;
		o << "This ensemble (" << this << ") is already tuned";
		Debug::warning(__FILE__, __LINE__, o.str());
		return _use_full_examples;
	}

	assert(_use_full_examples.empty());

	Debug::log(2) << "\n";
	Debug::log(2) << "About to tune Ensemble for Examples<" << EXAMPLE::name() << ">...\n";

	Examples<const EXAMPLE*> test_set;

	if (exmpls.size() <= MAX_EXAMPLES_FOR_AUTOMAGIC_TUNING) {
		Debug::log(2) << "Only " << exmpls.size() << " examples present for tuning (max = " << MAX_EXAMPLES_FOR_AUTOMAGIC_TUNING << "). Proceeding nonetheless...\n";
		test_set.reserve(exmpls.size());
		for (unsigned i = 0; i < exmpls.size(); i++) {
			test_set.push_back(&exmpls.at(i));
		}
		assert(test_set.size() == exmpls.size());
	} else {
		// Choose a sample of EXAMPLES_FOR_AUTOMAGIC_TUNING size, without replacement.

		vector<unsigned> rnd(exmpls.size());
		for (unsigned i = 0; i < exmpls.size(); i++)
			rnd.at(i) = 0;
		/// \todo Use Random instance to choose random numbers for std::random_shuffle.
		random_shuffle(rnd.begin(), rnd.end());

		test_set.reserve(MAX_EXAMPLES_FOR_AUTOMAGIC_TUNING);
		for (unsigned i = 0; i < MAX_EXAMPLES_FOR_AUTOMAGIC_TUNING; i++) {
			test_set.push_back(&exmpls.at(rnd.at(i)));
		}
		assert(test_set.size() == MAX_EXAMPLES_FOR_AUTOMAGIC_TUNING);
	}
	Debug::log(2) << "Done sampling Examples<" << EXAMPLE::name() << ">[" << test_set.size() << "] for tuning\n";

	vector<double> intermediate_conf, full_conf;

	_use_full_examples = false;	// Try this setting
	Debug::log(2) << "About to perform IntermediateExample test...\n";
	double intermediate_speed = this->determine_examples_per_second(test_set, intermediate_conf);

	_use_full_examples = true;	// Now try this setting
	Debug::log(2) << "About to perform DenseFullExample test...\n";
	double full_speed = this->determine_examples_per_second(test_set, full_conf);

	{
		// Sanity check
		unsigned chk = min(full_conf.size(), intermediate_conf.size());
		Debug::log(2) << "Sanity checking " << chk << " confidence values...\n";
		for (unsigned i = 0; i < chk; i++) {
			assert(full_conf.at(i) == intermediate_conf.at(i));
			/*
			if (full_conf.at(i) != intermediate_conf.at(i)) {
				ostringstream o;
				o << "fullconf != intconf: " << full_conf.at(i) << " != " << intermediate_conf.at(i);
				Debug::warning(__FILE__, __LINE__, o.str());
				assert(0);
			}
			*/
		}
	}

	if (full_speed > intermediate_speed) {
		_use_full_examples = true;	// DenseFullExample is faster.
		Debug::log(2) << "Preferring " << DenseFullExample::name() << " with estimated time " << exmpls.size() / full_speed << "s for all " << exmpls.size() << " examples\n";
	} else {
		_use_full_examples = false;	// IntermediateExample is faster.
		Debug::log(2) << "Preferring " << IntermediateExample::name() << " with estimated time " << exmpls.size() / intermediate_speed << "s for all " << exmpls.size() << " examples\n";
	}

	Debug::log(2) << "...done tuning Ensemble for Examples<" << EXAMPLE::name() << ">...\n";
	assert(!_use_full_examples.empty());
	return _use_full_examples;
}
template<>
const Optional<bool>& Ensemble::tune(const Examples<DenseFullExample>& exmpls) const {
	Debug::warning(__FILE__, __LINE__, "No need to tune DenseFullExamples.");
	if (!_use_full_examples.empty()) assert(_use_full_examples == true);
	_use_full_examples = true;
	return _use_full_examples;
}
template<>
const Optional<bool>& Ensemble::tune(const Examples<SparseFullExample>& exmpls) const {
	Debug::warning(__FILE__, __LINE__, "You shouldn't convert from SparseFullExample to DenseFullExample (I think)");
	assert(0);
	return _use_full_examples;
}

/// Do an empirical test of how many confidence computations per second
/// can be performed.
/// \param test_set The examples to be used for the test.
/// \param results The confidences computed will be put in this structure.
/// \param seconds Time to be spent performing the test.
template<typename EXAMPLE>
double Ensemble::determine_examples_per_second(const Examples<const EXAMPLE*>& test_set,
				vector<double>& results,
				float seconds) const {
	assert(results.empty());
	unsigned cnt = 0;
	typename Examples<const EXAMPLE*>::const_iterator e = test_set.begin();
	double orig_t = stats::usersys_time();
	double cur_t = stats::usersys_time();
	while (cur_t - orig_t < seconds && e != test_set.end()) {
		for (unsigned i = 0; i < EXAMPLES_TO_CHECK_PER_TIME_CHECK; i++) {
			if (e == test_set.end()) break;
			results.push_back(this->confidence_compute(**e));
			e++;
			cnt++;
		}
		cur_t = stats::usersys_time();
	}
	// Make sure our user+sys time is accurate.
	cur_t = stats::usersys_time();

	assert(cnt <= test_set.size());
	if (e == test_set.end()) assert(cnt = test_set.size());

	double speed = cnt / (cur_t - orig_t);

	Debug::log(2) << "\tspeed = " << speed << " exmpl confs/sec (computed over " << (cur_t - orig_t) << "s user+sys using " << cnt << " examples)\n";

	return speed;
}

// TEMPLATE INSTANTIATIONS
template const Optional<bool>& Ensemble::tune<Example>(const Examples<Example>&) const;
template double Ensemble::confidence_compute(const Example& e) const;
template double Ensemble::confidence_compute(const SparseFullExample& e) const;	// REMOVEME
