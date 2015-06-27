/*  $Id: Checkpoint.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Checkpoint.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "learn/Checkpoint.H"

#include "universal/parameter.H"
#include "universal/Debug.H"
#include "universal/Double.H"		// For BIGVAL

#include <cstdio>

Checkpoint* Checkpoint::_instance = NULL;

/// See if a Checkpoint exists for this label.
bool Checkpoint::exists() {
	assert(m_label != NO_LABEL);
	assert(!is_loaded);

	string filename = "checkpoint.label-" + label_string(m_label);
	ifstream i(filename.c_str());
	if (!i.is_open())
		return false;
	return true;
}

/// Load a checkpoint.
/// This allows us to restore our previous run and start where
/// we left off.
/// \param e An Ensemble into which we load a priorly generated
/// hypotheses. Empty if no checkpoint exists. Either way, e is initialized.
/// \param min_l1 The minimum l1 hypothesis file we can read.
/// \return The minimum l1 penalty factor of any split in the hypothesis loaded
/// \sideeffect Sets parameter::l1_penalty_factor() to return value.
double Checkpoint::load(Ensemble& e, double min_l1) {
	assert(m_label != NO_LABEL);

	is_loaded = true;

	e.clear();

	m_files.clear();
	string filename = "checkpoint.label-" + label_string(m_label);
	ifstream ifil(filename.c_str());
	if (!ifil.is_open()) {
		parameter::set_l1_penalty_factor(parameter::initial_l1_penalty_factor());
		if (parameter::vary_l1_penalty())
			parameter::set_l1_penalty_factor(parameter::initial_l1_penalty_factor());
		Debug::log(1) << "Could not open checkpoint file '" << filename << "' in Checkpoint::load()\n";
		Debug::log(1) << "\tResetting l1 penalty factor to initial value = " << parameter::l1_penalty_factor() << "\n";
		return parameter::l1_penalty_factor();
	}

	m_files.clear();
	double lowest_l1 = BIGVAL;
	string lowest_l1_file = "";
	while(!ifil.eof()) {
		string last_hypothesis;
		double last_l1;
		ifil >> last_hypothesis >> ws >> last_l1 >> ws;

		m_files[last_l1] = last_hypothesis;

		if (last_l1 < min_l1) continue;

		if (last_l1 < lowest_l1) {
			lowest_l1 = last_l1;
			lowest_l1_file = last_hypothesis;
		}
	}
	ifil.close();

	if (lowest_l1 == BIGVAL) {
		parameter::set_l1_penalty_factor(parameter::initial_l1_penalty_factor());

		Debug::log(1) << "WARNING: No hypotheses read from '" << filename << "' in Checkpoint::load()\n";
		Debug::log(1) << "\tResetting l1 penalty factor to initial value = " << parameter::l1_penalty_factor() << "\n";
		
		return parameter::l1_penalty_factor();
	}

	// Why the fuck can't I catch this elsewhere?
	try {
		parameter::set_l1_penalty_factor(lowest_l1);
		e.load(lowest_l1_file);
	} catch (string s) {
		cerr << "CAUGHT EXCEPTION: " << s << "\n";
		cerr << "EXITING...\n";
		exit(0);
	}

	last_ensemble = e;

	Debug::log(1) << "Checkpoint::load() loaded hypothesis from '" << lowest_l1_file << "'\n";
	Debug::log(1) << "\tSet l1_penalty_factor <- " << parameter::l1_penalty_factor() << "\n";
	return parameter::l1_penalty_factor();
}

/// Load all hypotheses in a checkpoint.
/// \param l A list of (Ensemble, min_l1) pairs into which we load all priorly generated
/// hypotheses. Empty if no checkpoint exists.
/// \warning l is initialized by this function.
/// \param min_l1 The minimum l1 hypothesis file we can read.
/// \return The minimum l1 penalty factor in the hypothesis loaded
/// \sideeffect Sets parameter::l1_penalty_factor() to return value.
double Checkpoint::load(LIST<pair<Ensemble, double> >& l, double min_l1) {
	assert(m_label != NO_LABEL);

	is_loaded = true;

	l.clear();

	m_files.clear();
	string filename = "checkpoint.label-" + label_string(m_label);
	ifstream ifil(filename.c_str());
	if (!ifil.is_open()) {
		parameter::set_l1_penalty_factor(parameter::initial_l1_penalty_factor());
		Debug::log(1) << "Could not open checkpoint file '" << filename << "' in Checkpoint::load()\n";
		Debug::log(1) << "\tResetting l1 penalty factor to initial value = " << parameter::l1_penalty_factor() << "\n";
		return parameter::l1_penalty_factor();
	}

	m_files.clear();
	double lowest_l1 = BIGVAL;
	string lowest_l1_file = "";
	while(!ifil.eof()) {
		string last_hypothesis;
		double last_l1;
		ifil >> last_hypothesis >> ws >> last_l1 >> ws;

		m_files[last_l1] = last_hypothesis;

		if (last_l1 < min_l1) continue;

		if (last_l1 < lowest_l1) {
			lowest_l1 = last_l1;
			lowest_l1_file = last_hypothesis;
		}

		Ensemble e;
		e.load(last_hypothesis);
		l.push_back(make_pair(e, last_l1));
	}
	ifil.close();

	if (lowest_l1 == BIGVAL) {
		parameter::set_l1_penalty_factor(parameter::initial_l1_penalty_factor());

		Debug::log(1) << "WARNING: No hypotheses read from '" << filename << "' in Checkpoint::load()\n";
		Debug::log(1) << "\tResetting l1 penalty factor to initial value = " << parameter::l1_penalty_factor() << "\n";
	
		return parameter::l1_penalty_factor();
	}

	// Why the fuck can't I catch this elsewhere?
	try {
		parameter::set_l1_penalty_factor(lowest_l1);
	} catch (string s) {
		cerr << "CAUGHT EXCEPTION: " << s << "\n";
		cerr << "EXITING...\n";
		exit(0);
	}

	Debug::log(1) << "Checkpoint::load() loaded " << l.size() << " hypotheses (lowest l1 = " << lowest_l1 << ")\n";
	Debug::log(1) << "\tSet l1_penalty_factor <- " << parameter::l1_penalty_factor() << "\n";
	return parameter::l1_penalty_factor();
}

/// Save a checkpoint.
/// \param e The current Ensemble we wish to checkpoint.
/// \todo Also, we should save the incomplete hypothesis from the current
/// iteration.
/// \invariant We call Checkpoint::Checkpoint() to initialize,
/// and Checkpoint::save() at the end of every iteration. That
/// way, every time Checkpoint::save() is called, we need only save
/// the last hypothesis added.
/// \todo Write this without fixed-length char string
void Checkpoint::save(const Ensemble& e) {
	last_ensemble = e;
	this->save();
}

void Checkpoint::save() {
	assert(m_label != NO_LABEL);

	assert(is_loaded);

	ostringstream oss;
	string filename;
	oss << "hypothesis.label-" << label_string(m_label) << ".l1_penalty-" << parameter::l1_penalty_factor();
	filename = oss.str();

	m_files[parameter::l1_penalty_factor()] = filename;

	Debug::log(1) << "Checkpointing l1_penalty_factor " << parameter::l1_penalty_factor() << " hypothesis to '" << filename << "'\n";

	ofstream ofil(filename.c_str());
	assert(ofil.is_open());
	ofil << last_ensemble.to_string();
	ofil << "END\n";
	ofil.close();

	filename = "checkpoint.label-" + label_string(m_label);
	Debug::log(2) << "Saving overall checkpoint to '" << filename << "'\n";
	ofstream ofil2(filename.c_str());
	assert(ofil2.is_open());
	for (map<double, string, greater<double> >::const_iterator p = m_files.begin(); p != m_files.end(); p++) {
		ofil2 << p->second << " " << p->first << "\n";
	}
	ofil2.close();
}
