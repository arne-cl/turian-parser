/*  $Id: EnsembleTokenizer.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file EnsembleTokenizer.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class EnsembleTokenizer
 *  \brief A tokenizer for reading an Ensemble.
 *
 *  This tokenizer is built using flex.
 *  \todo Integrate with libparse/PennTokenizer.[HL], making a unified
 *  Tokenizer class. This is difficult, however, since C++ FlexLexer's are
 *  *insensitive to other people's feelings*.
 *
 */

#ifndef __EnsembleTokenizer_H__
#define  __EnsembleTokenizer_H__

#include "universal/token.H"

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include <fstream>
#include <sstream>

#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class EnsembleTokenizer {
public:
	/// Consume a specific token.
	/// We read a single token from the lexer, and throw an assertion if it
	/// does not match the desired token or END_TOK.
	/// \param tok The token that \b<must> be consumed.
	/// \return False iff we consumed END_TOK instead of the desired token.
	bool want(const token tok) const;

	double get_float() const;
	int get_int() const;

	/// Lex one token.
	/// \return The token consumed.
	token get() const;

	/// Retrieve the text of the last token lexed.
	/// \return The text of the last token lexed.
	const char* current_yytext() const;

	void open(const char *fil);
	void open_cin();
	void close();
	void string(const char *str);

	/// Initialize a null lexer.
	/// \todo We could overload the constructor and get rid of
	/// the above "open" functions.
	EnsembleTokenizer() { lexer = NULL; }

	/// Delete the current lexer.
	/// \todo Fold close() into this destructor
	~EnsembleTokenizer() { if (lexer) { delete lexer; lexer = NULL; } }

private:
	ifstream fin;
	istringstream iss;
	FlexLexer *lexer;
};

#endif /* __EnsembleTokenizer_H__ */
