/*  $Id: vocabulary-freq.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file vocabulary-freq.C
 *  \brief Frequency of constituent labels
 *
 *  \todo Don't hardcode these!
 *  \bug This will be broken depending on whether we're using NPBs or not
 *
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "universal/vocabulary.H"
#include "universal/FastMap.H"

static FastMap<double> _label_freq_map;
static bool _label_freq_is_init = false;
void label_frequency_init();

/// Maximum relative frequency of some label in the treebank
/// \todo Don't hardcode this 
/// \bug This will be broken depending on whether we're using NPBs or not
double max_label_frequency() {
	assert(0);

//	return 0.357198; // NP
//	return 0.279904; // NPB
}

/// Relative frequency of different labels in the treebank
/// \todo Don't hardcode these
double label_frequency(Label l) {
	if (!_label_freq_is_init)
		label_frequency_init();
	return _label_freq_map[l];
}

void label_frequency_init() {
	_label_freq_map.insert(Label_ADJP(), 0.021609);
	_label_freq_map.insert(Label_ADVP(), 0.0364083);
	_label_freq_map.insert(Label_CONJP(), 0.000107241);
	_label_freq_map.insert(Label_FRAG(), 0.0020054);
	_label_freq_map.insert(Label_INTJ(), 0.000557653);
	_label_freq_map.insert(Label_LST(), 0.000160861);
	_label_freq_map.insert(Label_NAC(), 0.000139413);

	/// \bug This will be broken depending on whether we're using NPBs or not
//	_label_freq_map.insert(Label_NP(), 0.357198);	// w/o NPB
//	_label_freq_map.insert(Label_NP(), 0.0775364);	// w/ NPB
//	_label_freq_map.insert(Label_NPB(), 0.279904);

	_label_freq_map.insert(Label_NX(), 0.000589825);
	_label_freq_map.insert(Label_PP(), 0.0881842);
	_label_freq_map.insert(Label_PRN(), 0.00154427);
	_label_freq_map.insert(Label_QP(), 0.0127724);
	_label_freq_map.insert(Label_RRC(), 2.14482e-05);
	_label_freq_map.insert(Label_S(), 0.140453);
	_label_freq_map.insert(Label_SBAR(), 0.0185205);
	_label_freq_map.insert(Label_SBARQ(), 0.00102951);
	_label_freq_map.insert(Label_SINV(), 0.00170513);
	_label_freq_map.insert(Label_SQ(), 0.00164079);
	_label_freq_map.insert(Label_TOP(), 0.109107);
	_label_freq_map.insert(Label_UCP(), 0.000353895);
	_label_freq_map.insert(Label_VP(), 0.199125);
	_label_freq_map.insert(Label_WHADJP(), 9.65168e-05);
	_label_freq_map.insert(Label_WHADVP(), 0.00163006);
	_label_freq_map.insert(Label_WHNP(), 0.00412877);
	_label_freq_map.insert(Label_WHPP(), 8.57927e-05);
	_label_freq_map.insert(Label_X(), 0.000825755);
	_label_freq_map.lock();
	_label_freq_is_init = true;
}
