/*  $Id: posclass.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file posclass.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "universal/posclass.H"
#include "universal/Debug.H"
#include "universal/FastMap.H"
//#include "universal/LIST.H"

#include <ext/hash_map>
#ifndef DOXYGEN
using namespace __gnu_cxx;
#endif

static FastMap<Posclass> posclass_map;

static bool posclass_is_init = false;

static Posclass _Posclass_N = NO_POSCLASS;
static Posclass _Posclass_NP = NO_POSCLASS;

/// \todo Add a dummy last element to tag_to_class, s.t. we can check
/// that tag_to_class_cnt is correct.
static const unsigned tag_to_class_cnt = 47;
static const string tag_to_class[tag_to_class_cnt][2] = {
	{"CD", "::CD"},
	{"LS", "::CD"},

	{"CC", "::CJ"},
	{",", "::CJ"},
	{":", "::CJ"},

//	{"--", "::EOP"},
//	{",", "::EOP"},
//	{":", "::EOP"},

	{"DT", "::D"},
	{"PDT", "::D"},
	{"PRP$", "::D"},
	{"WDT", "::D"},

	{".", "::EOS"},

	{"IN", "::IN"},
	{"RP", "::IN"},
	{"TO", "::IN"},

	{"JJ", "::J"},
	{"JJR", "::J"},
	{"JJS", "::J"},

	{"$", "::N"},
	{"NN", "::N"},
	{"NNS", "::N"},

	{"NNP", "::NP"},
	{"NNPS", "::NP"},

	{"EX", "::P"},
	{"POS", "::P"},
	{"PRP", "::P"},
	{"WP", "::P"},
	{"WP$", "::P"},

	{"RB", "::R"},
	{"RBR", "::R"},
	{"RBS", "::R"},
	{"WRB", "::R"},

	{"``", "::SCM"},
	{"''", "::SCM"},
//	{"\"", "::SCM"},
	{"-LRB-", "::SCM"},
	{"-RRB-", "::SCM"},

	{"#", "::SYM"},
	{"SYM", "::SYM"},

	{"UH", "::UH"},

	{"FW", "::UK"},

	{"VBG", "::VBG"},

	{"VBN", "::VBN"},

	{"MD", "::V"},
	{"VB", "::V"},
	{"VBD", "::V"},
	{"VBP", "::V"},
	{"VBZ", "::V"},

	{"AUX", "::AUX"},
	{"AUXG", "::AUX"},

//	{"-NONE-", "::NONE"},
};

/// \todo Add a dummy last element to posclass_list, s.t. we can check
/// that posclass_cnt is correct.
static const unsigned posclass_cnt = 21;
static const string posclass_list[posclass_cnt] = {
	"",		// Blank place holder
	"::CD",		// 1,
	"::CJ",		// 2,
	"::D",		// 3,
	"",		// Blank place holder (formerly ::EOP)
	"::EOS",	// 5,
	"::IN",		// 6,
	"::J",		// 7,
	"::N",		// 8,
	"::NONE",	// 9,
	"::NP",		// 10,
	"::P",		// 11,
	"::R",		// 12,
	"::SCM",	// 13,
	"::SYM",	// 14,
	"::UH",		// 15,
	"::UK",		// 16,
	"::V",		// 17,
	"::VBG",	// 18,
	"::VBN",	// 19,
	"::AUX",	// 20
};

static LIST<Posclass> _all_posclass;

static void posclass_init() {
	assert(!posclass_is_init);

	posclass_map.clear();
	_all_posclass.clear();

	unsigned skipcnt = 0;
	for (unsigned i = 0; i < tag_to_class_cnt; i++) {
		// Skip labels not in the vocabulary.
		if (!is_label_string(tag_to_class[i][0])) {
			Debug::log(1) << "Skipping unknown label " << tag_to_class[i][0] << " in posclass_init()\n";
			skipcnt++;
			continue;
		}
		
		Label tag = string_to_label(tag_to_class[i][0]);
		assert(is_terminal_label(tag));

		Posclass c = string_to_posclass(tag_to_class[i][1]);
		assert(posclass_list[c] == tag_to_class[i][1]);
		assert(posclass_list[c] != "");

		posclass_map.insert(tag, c);
	}
	posclass_map.lock();

	for (unsigned i = 0; i < posclass_cnt; i++) {
		if (posclass_list[i] != "")
			_all_posclass.push_back(i);
		if (posclass_list[i] == "::N")
			_Posclass_N = i;
		else if (posclass_list[i] == "::NP")
			_Posclass_NP = i;
	}

	posclass_is_init = true;
}

Posclass tag_to_posclass(const Label tag) {
	if (!posclass_is_init)
		posclass_init();
	assert(posclass_is_init);

	assert(is_terminal_label(tag));
	return posclass_map[tag];
}

Posclass string_to_posclass(const string s) {
	Posclass c;
	for (c = 0; c < posclass_cnt; c++) {
		if (posclass_list[c] == s)
			break;
	}
	assert(posclass_list[c] == s);
	return c;
}

string posclass_string(const Posclass c) {
	assert(c < posclass_cnt && c >= 0);
	return(posclass_list[c]);
}

/// Is this a posclass?
/// Return false iff c is NO_POSCLASS or not a valid posclass.
bool is_posclass(const Posclass c) {
	if (!posclass_is_init)
		posclass_init();
	assert(posclass_is_init);

	if (c == NO_POSCLASS)
		return false;
	if (posclass_list[c] == "")
		return false;
	if (c >= 0 && c < posclass_cnt)
		return true;
	return false;
}

const LIST<Posclass>& all_posclass() {
	if (!posclass_is_init)
		posclass_init();
	assert(posclass_is_init);
	assert(!_all_posclass.empty());

	return _all_posclass;
}

Posclass Posclass_N() {
	return _Posclass_N;
}

Posclass Posclass_NP() {
	return _Posclass_NP;
}
