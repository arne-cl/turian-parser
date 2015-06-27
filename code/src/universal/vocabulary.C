/*  $Id: vocabulary.C 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file vocabulary.C
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#include "universal/globals.H"
#include "universal/vocabulary.H"
#include "universal/FastMap.H"
//#include "universal/LIST.H"

#include <iostream>
#include <fstream>
#include <vector>
#include <ext/hash_map>
#ifndef DOXYGEN
using namespace std;
using namespace __gnu_cxx;
#endif /* DOXYGEN */

static bool open_word_list = false;
static bool open_label_list = false;

static vector<string> word_list;
static vector<string> label_list;

static LIST<Label> _all_labels;
static LIST<Word> _all_words;
static LIST<Label> _all_terminal_labels;
static LIST<Label> _all_constituent_labels;
static unsigned _max_label;

static hash_map<string, Word, hash<string>, equal_to<string> > word_map;
static hash_map<string, Label, hash<string>, equal_to<string> > label_map;
static FastMap<bool> terminal_set;
static FastMap<bool> constituent_set;

//static Word _Word_CONTENT = NO_WORD;
static Label _Label_ADJP = NO_LABEL;
static Label _Label_ADVP = NO_LABEL;
static Label _Label_AUX = NO_LABEL;
static Label _Label_AUXG = NO_LABEL;
static Label _Label_CC = NO_LABEL;
static Label _Label_CD = NO_LABEL;
static Label _Label_COLON = NO_LABEL;
static Label _Label_COMMA = NO_LABEL;
static Label _Label_CONJP = NO_LABEL;
static Label _Label_DOLLAR = NO_LABEL;
static Label _Label_DT = NO_LABEL;
static Label _Label_EX = NO_LABEL;
static Label _Label_FRAG = NO_LABEL;
static Label _Label_FW = NO_LABEL;
static Label _Label_HASH = NO_LABEL;
static Label _Label_IN = NO_LABEL;
static Label _Label_INTJ = NO_LABEL;
static Label _Label_JJ = NO_LABEL;
static Label _Label_JJR = NO_LABEL;
static Label _Label_JJS = NO_LABEL;
static Label _Label_LS = NO_LABEL;
static Label _Label_LST = NO_LABEL;
static Label _Label_MD = NO_LABEL;
static Label _Label_NAC = NO_LABEL;
static Label _Label_NN = NO_LABEL;
static Label _Label_NNP = NO_LABEL;
static Label _Label_NNPS = NO_LABEL;
static Label _Label_NNS = NO_LABEL;
static Label _Label_NP = NO_LABEL;
static Label _Label_NPB = NO_LABEL;
static Label _Label_NX = NO_LABEL;
static Label _Label_POS = NO_LABEL;
static Label _Label_PP = NO_LABEL;
static Label _Label_PRN = NO_LABEL;
static Label _Label_PRP = NO_LABEL;
static Label _Label_PRPP = NO_LABEL;
static Label _Label_PRT = NO_LABEL;
static Label _Label_QP = NO_LABEL;
static Label _Label_RB = NO_LABEL;
static Label _Label_RBR = NO_LABEL;
static Label _Label_RBS = NO_LABEL;
static Label _Label_RP = NO_LABEL;
static Label _Label_RRC = NO_LABEL;
static Label _Label_S = NO_LABEL;
static Label _Label_SBAR = NO_LABEL;
static Label _Label_SBARQ = NO_LABEL;
static Label _Label_SINV = NO_LABEL;
static Label _Label_SQ = NO_LABEL;
static Label _Label_SYM = NO_LABEL;
static Label _Label_TO = NO_LABEL;
static Label _Label_TOP = NO_LABEL;
static Label _Label_UCP = NO_LABEL;
static Label _Label_UH = NO_LABEL;
static Label _Label_VB = NO_LABEL;
static Label _Label_VBD = NO_LABEL;
static Label _Label_VBG = NO_LABEL;
static Label _Label_VBN = NO_LABEL;
static Label _Label_VBP = NO_LABEL;
static Label _Label_VBZ = NO_LABEL;
static Label _Label_VP = NO_LABEL;
static Label _Label_WDT = NO_LABEL;
static Label _Label_WHADJP = NO_LABEL;
static Label _Label_WHADVP = NO_LABEL;
static Label _Label_WHNP = NO_LABEL;
static Label _Label_WHPP = NO_LABEL;
static Label _Label_WP = NO_LABEL;
static Label _Label_X = NO_LABEL;

void read_words(const string filename) {
	assert(!open_word_list);

	ifstream fin(filename.c_str());
	assert(fin.is_open());

	_all_words.clear();

	word_list.clear();
	word_list.push_back("");
	assert(word_list.at(NO_WORD) == "");
	word_map.clear();
	word_map[""] = NO_WORD;		// FIXME: Don't use [] operator

	unsigned i;
	string word;
	while(!fin.eof()) {
		fin >> i >> ws >> word >> ws;

		word_list.push_back(word);
		assert(word_list.at(i) == word);
		word_map[word] = i;	// FIXME: Don't use [] operator
		_all_words.push_back(i);

//		if (word == "*content*") _Word_CONTENT = i;
	}
	assert(word_list.size() == word_map.size());

	cerr << "Read " << word_list.size()-1 << " words from '" << filename << "'\n";

	fin.close();
	open_word_list = true;
}


/// \todo Make some assertion about # of constit. labels, and/or that
/// they are the lowest numbered ones?
void read_labels(const string filename) {
	assert(!open_label_list);

	ifstream fin(filename.c_str());
	assert(fin.is_open());

	_all_labels.clear();
	_all_constituent_labels.clear();
	_all_terminal_labels.clear();
	_max_label = 0;

	label_list.clear();
	label_map.clear();
	terminal_set.clear();
	constituent_set.clear();

	unsigned i, is_terminal, cnt;
	string label;
	while(!fin.eof()) {
		fin >> i >> ws >> is_terminal >> ws >> cnt >> label >> ws;

		if (i == NO_LABEL)
			label = "";

		label_list.push_back(label);
		assert(label_list.at(i) == label);
		label_map[label] = i;	// FIXME: Don't use [] operator

		if (i != NO_LABEL) {
			_all_labels.push_back(i);
			if (is_terminal) {
				terminal_set.insert(i, true);
				_all_terminal_labels.push_back(i);
			} else {
				constituent_set.insert(i, true);
				_all_constituent_labels.push_back(i);
			}
			if (i > _max_label) _max_label = i+1;
		}
	}
	assert(label_list.at(NO_LABEL) == "");
	assert(label_map[""] == NO_LABEL);	// FIXME: Don't use [] operator
	assert(label_list.size() == label_map.size());
//	assert(label_map.size() == terminal_set.size() + constituent_set.size() + 1);
	terminal_set.lock();
	constituent_set.lock();

/*
	Debug::log(1) << "Read " << constituent_set.size() << " constituents, " << \
			terminal_set.size() << " terminals from '" << filename << "'\n";
*/

	fin.close();
	open_label_list = true;

	if (is_label_string("ADJP")) _Label_ADJP = string_to_label("ADJP");
	if (is_label_string("ADVP")) _Label_ADVP = string_to_label("ADVP");
	if (is_label_string("AUX")) _Label_AUX = string_to_label("AUX");
	if (is_label_string("AUXG")) _Label_AUXG = string_to_label("AUXG");
	if (is_label_string("CC")) _Label_CC = string_to_label("CC");
	if (is_label_string("CD")) _Label_CD = string_to_label("CD");
	if (is_label_string("COLON")) _Label_COLON = string_to_label(":");
	if (is_label_string("COMMA")) _Label_COMMA = string_to_label(",");
	if (is_label_string("CONJP")) _Label_CONJP = string_to_label("CONJP");
	if (is_label_string("DOLLAR")) _Label_DOLLAR = string_to_label("$");
	if (is_label_string("DT")) _Label_DT = string_to_label("DT");
	if (is_label_string("EX")) _Label_EX = string_to_label("EX");
	if (is_label_string("FRAG")) _Label_FRAG = string_to_label("FRAG");
	if (is_label_string("FW")) _Label_FW = string_to_label("FW");
	if (is_label_string("HASH")) _Label_HASH = string_to_label("#");
	if (is_label_string("IN")) _Label_IN = string_to_label("IN");
	if (is_label_string("INTJ")) _Label_INTJ = string_to_label("INTJ");
	if (is_label_string("JJ")) _Label_JJ = string_to_label("JJ");
	if (is_label_string("JJR")) _Label_JJR = string_to_label("JJR");
	if (is_label_string("JJS")) _Label_JJS = string_to_label("JJS");
	if (is_label_string("LS")) _Label_LS = string_to_label("LS");
	if (is_label_string("LST")) _Label_LST = string_to_label("LST");
	if (is_label_string("MD")) _Label_MD = string_to_label("MD");
	if (is_label_string("NAC")) _Label_NAC = string_to_label("NAC");
	if (is_label_string("NN")) _Label_NN = string_to_label("NN");
	if (is_label_string("NNP")) _Label_NNP = string_to_label("NNP");
	if (is_label_string("NNPS")) _Label_NNPS = string_to_label("NNPS");
	if (is_label_string("NNS")) _Label_NNS = string_to_label("NNS");
	if (is_label_string("NP")) _Label_NP = string_to_label("NP");
	if (is_label_string("NPB")) _Label_NPB = string_to_label("NPB");
	if (is_label_string("NX")) _Label_NX = string_to_label("NX");
	if (is_label_string("POS")) _Label_POS = string_to_label("POS");
	if (is_label_string("PP")) _Label_PP = string_to_label("PP");
	if (is_label_string("PRN")) _Label_PRN = string_to_label("PRN");
	if (is_label_string("PRP")) _Label_PRP = string_to_label("PRP");
	if (is_label_string("PRPP")) _Label_PRPP = string_to_label("PRP$");
	if (is_label_string("PRT")) _Label_PRT = string_to_label("PRT");
	if (is_label_string("QP")) _Label_QP = string_to_label("QP");
	if (is_label_string("RB")) _Label_RB = string_to_label("RB");
	if (is_label_string("RBR")) _Label_RBR = string_to_label("RBR");
	if (is_label_string("RBS")) _Label_RBS = string_to_label("RBS");
	if (is_label_string("RP")) _Label_RP = string_to_label("RP");
	if (is_label_string("RRC")) _Label_RRC = string_to_label("RRC");
	if (is_label_string("S")) _Label_S = string_to_label("S");
	if (is_label_string("SBAR")) _Label_SBAR = string_to_label("SBAR");
	if (is_label_string("SBARQ")) _Label_SBARQ = string_to_label("SBARQ");
	if (is_label_string("SINV")) _Label_SINV = string_to_label("SINV");
	if (is_label_string("SQ")) _Label_SQ = string_to_label("SQ");
	if (is_label_string("SYM")) _Label_SYM = string_to_label("SYM");
	if (is_label_string("TO")) _Label_TO = string_to_label("TO");
	if (is_label_string("TOP")) _Label_TOP = string_to_label("TOP");
	if (is_label_string("UCP")) _Label_UCP = string_to_label("UCP");
	if (is_label_string("UH")) _Label_UH = string_to_label("UH");
	if (is_label_string("VB")) _Label_VB = string_to_label("VB");
	if (is_label_string("VBD")) _Label_VBD = string_to_label("VBD");
	if (is_label_string("VBG")) _Label_VBG = string_to_label("VBG");
	if (is_label_string("VBN")) _Label_VBN = string_to_label("VBN");
	if (is_label_string("VBP")) _Label_VBP = string_to_label("VBP");
	if (is_label_string("VBZ")) _Label_VBZ = string_to_label("VBZ");
	if (is_label_string("VP")) _Label_VP = string_to_label("VP");
	if (is_label_string("WDT")) _Label_WDT = string_to_label("WDT");
	if (is_label_string("WHADJP")) _Label_WHADJP = string_to_label("WHADJP");
	if (is_label_string("WHADVP")) _Label_WHADVP = string_to_label("WHADVP");
	if (is_label_string("WHNP")) _Label_WHNP = string_to_label("WHNP");
	if (is_label_string("WHPP")) _Label_WHPP = string_to_label("WHPP");
	if (is_label_string("WP")) _Label_WP = string_to_label("WP");
	if (is_label_string("X")) _Label_X = string_to_label("X");
}

const LIST<Word>& all_words() {
	assert(open_word_list);
	return _all_words;
}

const LIST<Label>& all_labels() {
	assert(open_label_list);
	return _all_labels;
}

const LIST<Label>& all_constituent_labels() {
	assert(open_label_list);
	return _all_constituent_labels;
}

const LIST<Label>& all_terminal_labels() {
	assert(open_label_list);
	return _all_terminal_labels;
}

const unsigned& max_label() {
	assert(open_label_list);
	return _max_label;
}

Word string_to_word(const string word) {
	assert(open_word_list);
	assert(word_map.find(word) != word_map.end());
	return word_map.find(word)->second;
}

Word string_to_word(const char *word) {
	assert(open_word_list);
	return string_to_word(string(word));
}

Label string_to_label(const string label) {
//	Debug::log(9) << "string_to_label of " << label << "\n";
	assert(open_label_list);
	assert(label_map.find(label) != label_map.end());
	return label_map.find(label)->second;
}

bool is_label_string(const string label) {
	return (label_map.find(label) != label_map.end());
}

Label string_to_label(const char *label) {
	assert(open_label_list);
	return string_to_label(string(label));
}

string word_string(const Word word) {
	assert(open_word_list);
	return word_list.at(word);
}

string label_string(const Label label) {
	assert(open_label_list);
	return label_list.at(label);
}

/// Is this a word?
/// Return false iff word is NO_WORD or not a word that was read in the vocabulary.
bool is_word(const Word word) {
	if (word == NO_WORD)
		return false;
	if (word >= 0 and word < word_list.size())
		return true;
	return false;
}

/// Is this a label?
/// Return false iff label is NO_LABEL or not a label that was read.
bool is_label(const Label label) {
	if (label == NO_LABEL)
		return false;
	if (label >= 0 and label < label_list.size())
		return true;
	return false;
}

/// \todo Option to whether to all label to be -EMPTY-.
/// Disable by default.
bool is_constituent_label(const Label label) {
	assert(open_label_list);
	assert(label != NO_LABEL);
	assert(label < label_list.size());
	assert(constituent_set.exists(label) == !terminal_set.exists(label));
	return constituent_set.exists(label);
}

bool is_terminal_label(const Label label) {
	assert(open_label_list);
	assert(label != NO_LABEL);
	assert(label < label_list.size());
	assert(constituent_set.exists(label) == !terminal_set.exists(label));
	return terminal_set.exists(label);
}

bool is_punctuation_terminal(const Label label) {
	assert(open_label_list);
	assert(label != NO_LABEL);

	string labelstr = label_string(label);
	for (unsigned i = 0; i < punctuation_cnt; i++)
		if (labelstr == punctuation_tags[i])
			return true;
	return false;
}

bool is_empty_constituent(const Label label) {
	assert(open_label_list);
	assert(label != NO_LABEL);
	return label_string(label) == "-EMPTY-";
}

//const Word& Word_CONTENT() {
//	assert(open_word_list);
//	return _Word_CONTENT;
//}

const Label& Label_NP() {
	assert(open_label_list);
	return _Label_NP;
}
const Label& Label_NPB() {
	assert(open_label_list);
	return _Label_NPB;
}
const Label& Label_POS() {
	assert(open_label_list);
	return _Label_POS;
}
const Label& Label_CC() {
	assert(open_label_list);
	return _Label_CC;
}
const Label& Label_S() {
	assert(open_label_list);
	return _Label_S;
}
const Label& Label_VP() {
	assert(open_label_list);
	return _Label_VP;
}
const Label& Label_MD() {
	assert(open_label_list);
	return _Label_MD;
}
const Label& Label_VB() {
	assert(open_label_list);
	return _Label_VB;
}
const Label& Label_VBD() {
	assert(open_label_list);
	return _Label_VBD;
}
const Label& Label_VBG() {
	assert(open_label_list);
	return _Label_VBG;
}
const Label& Label_VBN() {
	assert(open_label_list);
	return _Label_VBN;
}
const Label& Label_VBP() {
	assert(open_label_list);
	return _Label_VBP;
}
const Label& Label_VBZ() {
	assert(open_label_list);
	return _Label_VBZ;
}
const Label& Label_AUX() {
	assert(open_label_list);
	return _Label_AUX;
}
const Label& Label_AUXG() {
	assert(open_label_list);
	return _Label_AUXG;
}

const Label& Label_TOP() {
	assert(open_label_list);
	return _Label_TOP;
}

const Label& Label_ADJP() {
	assert(open_label_list);
	return _Label_ADJP;
}

const Label& Label_ADVP() {
	assert(open_label_list);
	return _Label_ADVP;
}

const Label& Label_COLON() {
	assert(open_label_list);
	return _Label_COLON;
}

const Label& Label_COMMA() {
	assert(open_label_list);
	return _Label_COMMA;
}

const Label& Label_DOLLAR() {
	assert(open_label_list);
	return _Label_DOLLAR;
}

const Label& Label_DT() {
	assert(open_label_list);
	return _Label_DT;
}

const Label& Label_EX() {
	assert(open_label_list);
	return _Label_EX;
}

const Label& Label_FRAG() {
	assert(open_label_list);
	return _Label_FRAG;
}

const Label& Label_HASH() {
	assert(open_label_list);
	return _Label_HASH;
}

const Label& Label_IN() {
	assert(open_label_list);
	return _Label_IN;
}

const Label& Label_INTJ() {
	assert(open_label_list);
	return _Label_INTJ;
}

const Label& Label_JJ() {
	assert(open_label_list);
	return _Label_JJ;
}

const Label& Label_JJR() {
	assert(open_label_list);
	return _Label_JJR;
}

const Label& Label_JJS() {
	assert(open_label_list);
	return _Label_JJS;
}

const Label& Label_NN() {
	assert(open_label_list);
	return _Label_NN;
}

const Label& Label_NNS() {
	assert(open_label_list);
	return _Label_NNS;
}

const Label& Label_NX() {
	assert(open_label_list);
	return _Label_NX;
}

const Label& Label_PP() {
	assert(open_label_list);
	return _Label_PP;
}

const Label& Label_PRN() {
	assert(open_label_list);
	return _Label_PRN;
}

const Label& Label_PRP() {
	assert(open_label_list);
	return _Label_PRP;
}

const Label& Label_PRPP() {
	assert(open_label_list);
	return _Label_PRPP;
}

const Label& Label_PRT() {
	assert(open_label_list);
	return _Label_PRT;
}

const Label& Label_QP() {
	assert(open_label_list);
	return _Label_QP;
}

const Label& Label_RB() {
	assert(open_label_list);
	return _Label_RB;
}

const Label& Label_RBR() {
	assert(open_label_list);
	return _Label_RBR;
}

const Label& Label_RBS() {
	assert(open_label_list);
	return _Label_RBS;
}

const Label& Label_RP() {
	assert(open_label_list);
	return _Label_RP;
}

const Label& Label_SBAR() {
	assert(open_label_list);
	return _Label_SBAR;
}

const Label& Label_SBARQ() {
	assert(open_label_list);
	return _Label_SBARQ;
}

const Label& Label_SINV() {
	assert(open_label_list);
	return _Label_SINV;
}

const Label& Label_SQ() {
	assert(open_label_list);
	return _Label_SQ;
}

const Label& Label_TO() {
	assert(open_label_list);
	return _Label_TO;
}

const Label& Label_UCP() {
	assert(open_label_list);
	return _Label_UCP;
}

const Label& Label_WDT() {
	assert(open_label_list);
	return _Label_WDT;
}

const Label& Label_WHADJP() {
	assert(open_label_list);
	return _Label_WHADJP;
}

const Label& Label_WHADVP() {
	assert(open_label_list);
	return _Label_WHADVP;
}

const Label& Label_WHNP() {
	assert(open_label_list);
	return _Label_WHNP;
}

const Label& Label_WHPP() {
	assert(open_label_list);
	return _Label_WHPP;
}

const Label& Label_WP() {
	assert(open_label_list);
	return _Label_WP;
}

const Label& Label_X() {
	assert(open_label_list);
	return _Label_X;
}

const Label& Label_CD() {
	assert(open_label_list);
	return _Label_CD;
}

const Label& Label_FW() {
	assert(open_label_list);
	return _Label_FW;
}

const Label& Label_LS() {
	assert(open_label_list);
	return _Label_LS;
}

const Label& Label_NNP() {
	assert(open_label_list);
	return _Label_NNP;
}

const Label& Label_NNPS() {
	assert(open_label_list);
	return _Label_NNPS;
}

const Label& Label_SYM() {
	assert(open_label_list);
	return _Label_SYM;
}

const Label& Label_UH() {
	assert(open_label_list);
	return _Label_UH;
}

const Label& Label_CONJP() {
	assert(open_label_list);
	return _Label_CONJP;
}

const Label& Label_LST() {
	assert(open_label_list);
	return _Label_LST;
}

const Label& Label_NAC() {
	assert(open_label_list);
	return _Label_NAC;
}

const Label& Label_RRC() {
	assert(open_label_list);
	return _Label_RRC;
}
