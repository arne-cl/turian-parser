%{
/*  $Id: PennTokenizer.ll 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file PennTokenizer.L
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 */

#include "parse/PennTokenizer.H"

#include "universal/Debug.H"

#include <cassert>

%}

LPAREN		"("
RPAREN		")"
WHITESPACE	[\r\n \t]
TEXT		[^()\r\n \t]+

%option noyywrap
%option prefix="penn"

%%

{LPAREN}	{ return LPAREN_TOK; }
{RPAREN}	{ return RPAREN_TOK; }
<<EOF>>         { return END_TOK; }
{WHITESPACE}	;
{TEXT}		{ return TEXT_TOK; }
.		{ cerr << "Unexpected token: " << YYText() << "\n"; assert(0); }

%%

/// Consume a specific token.
/// We read a single token from the lexer, and throw an assertion if it
/// does not match the desired token or END_TOK.
/// \param tok The token that must be consumed.
/// \return False iff we consumed END_TOK instead of the desired token.
bool PennTokenizer::want(const token tok) const {
	assert(this->lexer);
	token mytok = (token)this->lexer->yylex();
	assert (mytok == tok || mytok == END_TOK);
	return (mytok == tok);
}

/// Lex one token.
/// \return The token consumed.
token PennTokenizer::get() const {
	assert(this->lexer);
	return (token) this->lexer->yylex();
}

/// Retrieve the text of the last token lexed.
/// \return The text of the last token lexed.
const char* PennTokenizer::current_yytext() const {
	return this->lexer->YYText();
}

void PennTokenizer::open(string fil) {
	Debug::log(2) << "PennTokenizer::open('" << fil << "')...\n";
	assert(!fin.is_open());
	fin.open(fil.c_str(), ifstream::in);
	assert(fin.is_open());
	fin.clear();
	fin.seekg(0);

	this->lexer = new pennFlexLexer((istream *)&fin, NULL);
	assert(this->lexer);
}

void PennTokenizer::open_cin() {
	this->lexer = new pennFlexLexer((istream *)&cin, NULL);
	assert(this->lexer);
}

void PennTokenizer::close() {
	assert(fin.is_open());
	fin.close();
	assert(!fin.is_open());
	delete this->lexer;
	this->lexer = NULL;
}

/*
void PennTokenizer::string(const char *str) {
	if (this->lexer)
		delete this->lexer;

	this->iss.str(str);
	this->lexer = new pennFlexLexer(&this->iss, NULL);
	assert(this->lexer);
}
*/
