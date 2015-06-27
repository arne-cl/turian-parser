/*  $Id: vocabulary.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file vocabulary.H
 *  \brief Functions to handle vocabulary Word%s and consituent Label%s.
 *
 *  Internally, Word%s and Label%s are stored numerically.
 *  These functions allow one to convert back-and-forth between numerical and
 *  string representations.
 *
 *  \note Before the vocabulary/labels can be used, a vocabulary/label file must be read.
 *  This file should contain the string -> numeric value mappings.
 *
 *  \todo It shouldn't be necessary to read in a vocabulary/labels file.
 *  \todo Make this a class, built with the Singleton strategy
 *  \todo Make a Label class and a Word class, with string() methods,
 *  and constructors from string and char*.
 *
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#ifndef __vocabulary_H__
#define  __vocabulary_H__

#include <string>
#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

#include "universal/LIST.H"	// REMOVEME
//class LIST;

typedef unsigned Label;
typedef unsigned Word;

const Label NO_LABEL = 0;
const Label NO_WORD = 0;

/// Read in a vocabulary word list, and their indices
/// The vocabulary file consists of lines in a two column format:
/// - The first column is a (numerical) index.
/// Indices must be in increasing numerical order, starting at 1.
/// - The second column is the (lowercase) word string.
/// (Index 0 is reserved for "".)
/// \param filename Word list filename.
void read_words(const string filename);

/// Read in a label list, and their indices
/// The label file consists of lines in a four column format:
/// - The first column is a (numerical) index.
///	Indices must be in increasing numerical order.
/// - The second column is 0 iff the label is a constituent,
/// 	1 iff the label is a terminal.
/// - The third column is an (unused) number.
/// - The fourth column is the label string.
/// The first line must be "no_label" with index 0, which is neither a
/// constituent nor a terminal.
/// \param filename Label list filename.
void read_labels(const string filename);

const LIST<Word>& all_words();
const LIST<Label>& all_labels();
const LIST<Label>& all_constituent_labels();
const LIST<Label>& all_terminal_labels();

const unsigned& max_label();

/// Convert a word string to its index.
Word string_to_word(const string word);

/// Convert a word string to its index.
Word string_to_word(const char *word);

/// Convert a label string to its index.
Label string_to_label(const string label);

/// Convert a label string to its index.
Label string_to_label(const char *label);

/// Convert a word index to its string.
string word_string(const Word word);

/// Convert a label index to its string.
string label_string(const Label label);

/// Is this a word?
/// Return false iff word is NO_WORD or not a word that was read in the vocabulary.
bool is_word(const Word word);

/// Is this a label?
/// Return false iff label is NO_LABEL or not a label that was read.
bool is_label(const Label label);

bool is_label_string(const string label);

/// Is this a constituent label?
/// \todo Option to whether to all label to be -EMPTY-.
/// Disable by default.
bool is_constituent_label(const Label label);

/// Is this a terminal label?
bool is_terminal_label(const Label label);

/// Is this a punctuation terminal?
/// \invariant label must be a terminal.
bool is_punctuation_terminal(const Label label);

/// Is this an -EMPTY- constituent?
/// NB "-EMPTY-" need not be in the label list.
/// \invariant label must be a constituent.
bool is_empty_constituent(const Label label);

//const Word& Word_CONTENT();
const Label& Label_ADJP();
const Label& Label_ADVP();
const Label& Label_AUX();
const Label& Label_AUXG();
const Label& Label_CC();
const Label& Label_CD();
const Label& Label_COLON();
const Label& Label_COMMA();
const Label& Label_CONJP();
const Label& Label_DOLLAR();
const Label& Label_DT();
const Label& Label_EX();
const Label& Label_FRAG();
const Label& Label_FW();
const Label& Label_HASH();
const Label& Label_IN();
const Label& Label_INTJ();
const Label& Label_JJ();
const Label& Label_JJR();
const Label& Label_JJS();
const Label& Label_LS();
const Label& Label_LST();
const Label& Label_MD();
const Label& Label_NAC();
const Label& Label_NN();
const Label& Label_NNP();
const Label& Label_NNPS();
const Label& Label_NNS();
const Label& Label_NP();
const Label& Label_NPB();
const Label& Label_NX();
const Label& Label_POS();
const Label& Label_PP();
const Label& Label_PRN();
const Label& Label_PRP();
const Label& Label_PRPP();
const Label& Label_PRT();
const Label& Label_QP();
const Label& Label_RB();
const Label& Label_RBR();
const Label& Label_RBS();
const Label& Label_RP();
const Label& Label_RRC();
const Label& Label_S();
const Label& Label_SBAR();
const Label& Label_SBARQ();
const Label& Label_SINV();
const Label& Label_SQ();
const Label& Label_SYM();
const Label& Label_TO();
const Label& Label_TOP();
const Label& Label_UCP();
const Label& Label_UH();
const Label& Label_VB();
const Label& Label_VBD();
const Label& Label_VBG();
const Label& Label_VBN();
const Label& Label_VBP();
const Label& Label_VBZ();
const Label& Label_VP();
const Label& Label_WDT();
const Label& Label_WHADJP();
const Label& Label_WHADVP();
const Label& Label_WHNP();
const Label& Label_WHPP();
const Label& Label_WP();
const Label& Label_X();

/// Relative frequency of different labels in the treebank
/// \todo Don't hardcode these
double label_frequency(Label l);

/// Maximum relative frequency of some label in the treebank
/// \todo Don't hardcode this 
double max_label_frequency();

#endif /* __vocabulary_H__ */
