/*  $Id: token.H 1657 2006-06-04 03:03:05Z turian $ 
 *  Copyright (c) 2004-2006, New York University. All rights reserved. */
/*!
 *  \file token.H
 *  \brief Tokens used by various system tokenizers.
 *
 *  \todo Split this into several copies, one for each tokenizer?
 *
 *  $LastChangedDate: 2006-06-03 23:03:05 -0400 (Sat, 03 Jun 2006) $
 *  $Revision: 1657 $
 *
 */

#ifndef __token_H__
#define  __token_H__

/// Possible tokens for lexing.
/// - END_TOK is the end of file token.
/// - LPAREN is a left parenthesis.
/// - RPAREN is a right parenthesis.
/// - TEXT is any string of non-whitespace, non-parenthesis characters.
/// - WRITEME
/// \bug When a TEXT token contains unprintable/weird characters, flex
/// truncates the string by ignoring this and all subsequent characters.
/// \todo Rename to token_ty
typedef enum {
	INT_TOK, FLOAT_TOK, LPAREN_TOK, RPAREN_TOK, TEXT_TOK, TREE_TOK, NODE_TOK, LEAF_TOK, HASH_TOK, COLON_TOK, POSCHILD_TOK, NEGCHILD_TOK, EQ_TOK, CONF_TOK, INITIAL_WEIGHTS_TOK, DEPTH_TOK, SPLITON_TOK, QUOTED_TOK, LSQUARE_BRACKET_TOK, RSQUARE_BRACKET_TOK, COMMA_TOK, USE_FULL_EXAMPLES_TOK, TRUE_TOK, FALSE_TOK, EMPTY_TOK, END_TOK, EOF_TOK
} token;

#endif /* __token_H__ */
