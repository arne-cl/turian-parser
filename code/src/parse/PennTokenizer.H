/*  $Id: PennTokenizer.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file PennTokenizer.H
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */
/*!
 *  \class PennTokenizer
 *  \brief A Penn treebank lexer.
 *
 *  This tokenizer is built using flex.
 *
 *  \todo Integrate with libboost/BoostTokenizer.[HL], making a unified
 *  Tokenizer class. This is difficult, however, since C++ FlexLexer's are
 *  *insensitive to other people's feelings*.
 *
 */

#ifndef __PennTokenizer_H__
#define  __PennTokenizer_H__

#include "universal/token.H"

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include <fstream>
#include <sstream>

#ifndef DOXYGEN
using namespace std;
#endif /* DOXYGEN */

class PennTokenizer {
public:

	/// Consume a specific token.
	/// We read a single token from the lexer, and throw an assertion if it
	/// does not match the desired token or END_TOK.
	/// \param tok The token that \b<must> be consumed.
	/// \return False iff we consumed END_TOK instead of the desired token.
	bool want(const token tok) const;

	/// Lex one token.
	/// \return The token consumed.
	token get() const;

	/// Retrieve the text of the last token lexed.
	/// \return The text of the last token lexed.
	const char* current_yytext() const;

	void open(string fil);
	void open_cin();
	void close();
	//void string(const char *str);

	/// Initialize a null lexer.
	/// \todo We could overload the constructor and get rid of
	/// the above "open" functions.
	PennTokenizer() { lexer = NULL; }

	/// Delete the current lexer.
	/// \todo Fold close() into this destructor
	~PennTokenizer() { if (lexer) { delete lexer; lexer = NULL; } }

private:
	ifstream fin;
	istringstream iss;
	FlexLexer *lexer;
};

#endif /* __PennTokenizer_H__ */
