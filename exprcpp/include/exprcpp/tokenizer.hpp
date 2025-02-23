#pragma once

#include "exprcpp/ast.hpp"
#include <memory>
#include <string>

namespace exprcpp::internal
{

	enum token_type_e
	{
		TOK_UNKNOWN = -1,	// Unknown token type; for errors
		TOK_ENDMARKER,		// Endmarker token
		TOK_NEWLINE,		// Newline token
		TOK_NAME,			// Variable names
		TOK_NUMBER,			// Number token type
		TOK_STRING,			// String token type

		TOK_IN,				// in
		TOK_NOT,			// not
		TOK_IF,				// if
		TOK_ELSE,			// else
		TOK_AND,			// and
		TOK_OR,				// or

		/* Operator tokens */
		TOK_OP,				// Blank operator value, often means unknown operator
		TOK_ADD,			// +
		TOK_MINUS,			// -
		TOK_STAR,			// *
		TOK_FSLASH,			// /
		TOK_BSLASH,			// "\"
		TOK_COMMA,			// ,
		TOK_DOT,			// .
		TOK_EQUAL,			// =
		TOK_GREATER,		// >
		TOK_LESS,			// <
		TOK_AT,				// @
		TOK_PERCENT,		// %
		TOK_AMPER,			// &
		TOK_COLON,			// :
		TOK_SEMI,			// ;
		TOK_CIRCUMFLEX,		// ^
		TOK_TILDE,			// ~
		TOK_VBAR,			// |
		TOK_LPAREN,			// (
		TOK_RPAREN,			// )
		TOK_LSQB,			// [
		TOK_RSQB,			// ]
		TOK_LBRACE,			// {
		TOK_RBRACE,			// }

		/* Double char operator tokens */
		TOK_EQUALEQUAL,		// ==
		TOK_NOTEQUAL,		// "!=", <>
		TOK_ADDEQUAL,		// +=
		TOK_MINUSEQUAL,		// -=
		TOK_STAREQUAL,		// *=
		TOK_FSLASHEQUAL,	// /=
		TOK_GREATEREQUAL,	// >=
		TOK_LESSEQUAL,		// <=
		TOK_DOUBLEADD,		// ++
		TOK_DOUBLEMINUS,	// --
		TOK_ELLIPSIS,		// ..
		TOK_DOUBLESTAR,		// **
		TOK_DOUBLEFSLASH,	// //
		TOK_LEFTSHIFT,		// <<
		TOK_RIGHTSHIFT,		// >>
		TOK_COLONEQUAL,		// :=

		/* Triple char operator tokens */
		TOK_LEQUALG,		// <=>

		/* Miscellaneous tokens */
		TOK_ERRORTOKEN,
		TOK_N_TOKENS,
		TOK_NT_OFFSET = 256
	};

	struct memo_t
	{
		int type;
		ast::node_ptr_t node;
		int mark;
		std::shared_ptr<memo_t> next;
	};

	struct token_t
	{
		std::string value;
		token_type_e type;
		std::shared_ptr<memo_t> memo;
	};

	typedef std::shared_ptr<token_t> token_ptr_t;

	class tokenizer_t
	{
	public:
		tokenizer_t(const std::string& expression_string);
		~tokenizer_t() = default;

		auto get() -> token_ptr_t;
	private:
		auto next() -> char;
		auto back() -> void;
		auto back(const std::string::const_iterator& location) -> void;

		auto decimal_tail(token_ptr_t& token) -> char;

		auto operator_one_char(const char c1) -> token_type_e;
		auto operator_two_chars(const char c1, const char c2) -> token_type_e;
		auto operator_three_chars(const char c1, const char c2, const char c3) -> token_type_e;
	private:
		const std::string m_expression_string;

		std::string::const_iterator m_start;
		std::string::const_iterator m_end;
		std::string::const_iterator m_current;

		bool m_done = false;
		bool m_first = true;
	};

}