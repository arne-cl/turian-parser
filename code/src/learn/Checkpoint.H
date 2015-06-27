/*  $Id: Checkpoint.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file Checkpoint.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class Checkpoint
 *  \brief Save and load training checkpoints.
 *
 *  This allows us to restart training where we left off, in the case of
 *  abnormal program termination.
 *  Each checkpoint keeps track of Inference learners for a particular Label.
 *  The checkpoint contains a list of intermediate hypotheses (Ensemble%s) for this Label,
 *  keyed by l1.
 *
 *  \warning The behavior is undefined if one creates and uses two
 *  different Checkpoint instances for the same label. In other words,
 *  only have one of these floating around at any given time.
 *
 *  \todo Remove commented out code
 *
 */

#ifndef __Checkpoint_H__
#define  __Checkpoint_H__

#include "learn/decision-trees/Ensemble.H"

#include "universal/vocabulary.H"
#include "universal/LIST.H"

#include <cassert>
#include <map>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

class Checkpoint {
public:
	static Checkpoint* instance() {
		if (!_instance) {
			_instance = new Checkpoint;
		}
		assert(_instance);
		return _instance;
	}

	~Checkpoint() {
		assert(_instance);
		delete(_instance);
		_instance = NULL;
	}

	/// Construct a checkpoint instance for a certain label.
	/// \param l The Label which classifier we are training, and
	/// would like to checkpoint.
	void construct(const Label l) {
		assert(is_constituent_label(l));
	
		m_label = l;
		is_loaded = false;
		m_files.clear();
	}

	/// See if a Checkpoint exists for this label.
	bool exists();

	/// Load a checkpoint.
	/// This allows us to restore our previous run and start where
	/// we left off.
	/// \param e An Ensemble into which we load a priorly generated
	/// hypotheses. Empty if no checkpoint exists. Either way, e is initialized.
	/// \param min_l1 The minimum l1 hypothesis file we can read.
	/// \return The minimum l1 penalty factor of any split in the hypothesis loaded
	/// \sideeffect Sets parameter::l1_penalty_factor() to return value.
	double load(Ensemble& e, double min_l1=0);
//	step_ty load(Ensemble& e) { assert(0); }

	/// Load all hypotheses in a checkpoint.
	/// \param l A list of (Ensemble, min_l1) pairs into which we load all priorly generated
	/// hypotheses. Empty if no checkpoint exists.
	/// \warning l is initialized by this function.
	/// \param min_l1 The minimum l1 hypothesis file we can read.
	/// \return The minimum l1 penalty factor of any split in the hypothesis loaded
	/// \sideeffect Sets parameter::l1_penalty_factor() to return value.
	double load(LIST<pair<Ensemble, double> >& l, double min_l1=0);
//	step_ty load(Ensemble& e) { assert(0); }

	/// Save a checkpoint.
	/// \param e The current Ensemble we wish to checkpoint.
	/// \todo Also, we should save the incomplete hypothesis from the current
	/// iteration.
	/// \invariant We call Checkpoint::Checkpoint() to initialize,
	/// and Checkpoint::save() at the end of every iteration. That
	/// way, every time Checkpoint::save() is called, we need only save
	/// the last hypothesis added.
	/// \todo Write this without fixed-length char string
//	/// \param s The step we just completed in the current iteration.
//	void save(step_ty s, Ensemble& e);
	void save(const Ensemble& e);
	void save();

protected:
	Checkpoint() {
		m_label = NO_LABEL;
		_instance = this;
	}

private:
	/// The Label which classifier we are training, and
	/// would like to checkpoint.
	Label m_label;

	Ensemble last_ensemble;

	/// Have we loaded this checkpoint?
	bool is_loaded;

	/// Files checkpointed for this label.
	map<double, string, greater<double> > m_files;

	static Checkpoint *_instance;
};

#endif /* __Checkpoint_H__ */
