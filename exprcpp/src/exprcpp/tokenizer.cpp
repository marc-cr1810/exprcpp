#include "exprcpp/tokenizer.hpp"

#include <unordered_map>

namespace exprcpp::internal
{

	namespace constants
	{

		const std::unordered_map<std::string, token_type_e> keyword_token_pairs =
		{
			{ "in", TOK_IN },
			{ "not", TOK_NOT },
			{ "if", TOK_IF },
			{ "else", TOK_ELSE },
			{ "and", TOK_AND },
			{ "or", TOK_OR }
		};

	}

	inline auto is_whitespace(const char c) -> bool
	{
		return (' ' == c) || ('\n' == c) ||
			('\r' == c) || ('\t' == c) ||
			('\b' == c) || ('\v' == c) ||
			('\f' == c);
	}

	inline auto is_operator_char(const char c) -> bool
	{
		return ('+' == c) || ('-' == c) ||
			('*' == c) || ('/' == c) ||
			('^' == c) || ('<' == c) ||
			('>' == c) || ('=' == c) ||
			(',' == c) || ('!' == c) ||
			('(' == c) || (')' == c) ||
			('[' == c) || (']' == c) ||
			('{' == c) || ('}' == c) ||
			('%' == c) || (':' == c) ||
			('?' == c) || ('&' == c) ||
			('|' == c) || (';' == c);
	}

	inline auto is_letter(const char c) -> bool
	{
		return (('a' <= c) && (c <= 'z')) ||
			(('A' <= c) && (c <= 'Z'));
	}

	inline auto is_digit(const char c) -> bool
	{
		return ('0' <= c) && (c <= '9');
	}

	inline auto is_letter_or_digit(const char c) -> bool
	{
		return is_letter(c) || is_digit(c);
	}

	inline auto is_left_bracket(const char c) -> bool
	{
		return ('(' == c) || ('[' == c) || ('{' == c);
	}

	inline auto is_right_bracket(const char c) -> bool
	{
		return (')' == c) || (']' == c) || ('}' == c);
	}

	inline auto is_bracket(const char c) -> bool
	{
		return is_left_bracket(c) || is_right_bracket(c);
	}

	inline auto is_sign(const char c) -> bool
	{
		return ('+' == c) || ('-' == c);
	}

	inline auto is_potential_identifier_start(const char& c) -> bool
	{
		return (c >= 'a' && c <= 'z')
			|| (c >= 'A' && c <= 'Z')
			|| (c == '_')
			|| (c >= 128);
	}

	inline auto is_potential_identifier_char(const char& c) -> bool
	{
		return (c >= 'a' && c <= 'z')
			|| (c >= 'A' && c <= 'Z')
			|| (c >= '0' && c <= '9')
			|| (c == '_')
			|| (c >= 128);
	}

	static auto syntax_error(token_ptr_t& token, const std::string& message) -> void
	{
		if (token == nullptr)
		{
			token = std::make_shared<token_t>();
		}
		token->value = message;
		token->type = TOK_ERRORTOKEN;
	}

	tokenizer_t::tokenizer_t(const std::string& expression_string)
		: m_expression_string(expression_string)
	{
		m_current = m_expression_string.begin();
	}

	auto tokenizer_t::get() -> token_ptr_t 
	{
		token_ptr_t token = std::make_shared<token_t>();
		char c; // skip white spaces    
		do 
		{
			c = next();
		} while (is_whitespace(c));

		m_start = m_current;

		if (c == EOF)
		{
			token->value = "";
			token->type = TOK_ENDMARKER;
			m_done = true;
			return token;
		}

		if (is_potential_identifier_start(c))
		{
			while (is_potential_identifier_char(c))
			{
				c = next();
			}
			back();
			m_end = m_current;
			const auto value = std::string(m_start, m_end + 1);
			token->value = value;
			token->type = TOK_NAME;
			if (constants::keyword_token_pairs.find(value) != constants::keyword_token_pairs.end())
			{
				token->type = constants::keyword_token_pairs.at(value);
			}
			return token;
		}

		if (c == '\n')
		{
			m_end = m_current - 1;
			token->value = "";
			token->type = TOK_NEWLINE;
			return token;
		}

		if (is_digit(c))
		{
			c = decimal_tail(token);
			if (c == 0) 
			{
				return token;
			} 
			{
				if (c == '.') 
				{
					c = next();
				fraction: // fraction                
					if (is_digit(c)) {
						c = decimal_tail(token);
						if (c == 0) 
						{
							return token;
						}
					}
				}
				if (c == 'e' || c == 'E') 
				{
					char e;
				exponent: e = c; // exponent part                
					c = next();
					if (c == '+' || c == '-') 
					{
						c = next();
						if (!is_digit(c)) 
						{
							back();
							token->type = TOK_ERRORTOKEN;
							return token;
						}
					}
					else if (!is_digit(c)) 
					{
						back();
						back();
						m_end = m_current;
						token->value = std::string(m_start, m_end + 1);
						token->type = TOK_NUMBER;
						return token;
					}
					c = decimal_tail(token);
					if (c == 0) 
					{
						return token;
					}
				}
			}
			back();
			m_end = m_current;
			token->value = std::string(m_start, m_end + 1);
			token->type = TOK_NUMBER;
			return token;
		} 
		{
			auto c2 = next();
			token->type = operator_two_chars(c, c2);
			if (token->type != TOK_OP)
			{
				auto c3 = next();
				auto type = operator_three_chars(c, c2, c3);
				if (type != TOK_OP)
				{
					token->type = type;
				}
				else
				{
					back();
				}
				m_end = m_current;
				token->value = std::string(m_start, m_end + 1);
				return token;
			}
			back();
		}
		m_end = m_current;
		token->value = std::string(m_start, m_end + 1);
		token->type = operator_one_char(c);
		return token;
	}

	auto tokenizer_t::next() -> char
	{
		if (m_expression_string.empty() || m_done)
		{
			return EOF;
		}

		if (m_first)
		{
			m_first = false;
			return *m_current;
		}

		m_current++;
		if (m_current < m_expression_string.end())
		{
			return *m_current;
		}
		return EOF;
	}

	auto tokenizer_t::back() -> void
	{
		if (m_current == m_expression_string.begin())
		{
			return;
		}
		--m_current;
	}

	auto tokenizer_t::back(const std::string::const_iterator& location) -> void
	{
		m_current = location;
	}

	auto tokenizer_t::decimal_tail(token_ptr_t& token) -> char
	{
		char c;

		while (true)
		{
			do
			{
				c = next();
			} while (is_digit(c));
			if (c != '_')
			{
				break;
			}
			c = next();
			if (!is_digit(c))
			{
				back();
				syntax_error(token, "invalid decimal literal");
				return 0;
			}
		}

		return c;
	}

	auto tokenizer_t::operator_one_char(const char c1) -> token_type_e
	{
		switch (c1)
		{
		case '+':
			return TOK_ADD;
		case '-':
			return TOK_MINUS;
		case '*':
			return TOK_STAR;
		case '/':
			return TOK_FSLASH;
		case '\\':
			return TOK_BSLASH;
		case ',':
			return TOK_COMMA;
		case '.':
			return TOK_DOT;
		case '=':
			return TOK_EQUAL;
		case '>':
			return TOK_GREATER;
		case '<':
			return TOK_LESS;
		case '@':
			return TOK_AT;
		case '%':
			return TOK_PERCENT;
		case '&':
			return TOK_AMPER;
		case ':':
			return TOK_COLON;
		case ';':
			return TOK_SEMI;
		case '^':
			return TOK_CIRCUMFLEX;
		case '~':
			return TOK_TILDE;
		case '|':
			return TOK_VBAR;
		case '(':
			return TOK_LPAREN;
		case ')':
			return TOK_RPAREN;
		case '[':
			return TOK_LSQB;
		case ']':
			return TOK_RSQB;
		case '{':
			return TOK_LBRACE;
		case '}':
			return TOK_RBRACE;
		}
		return TOK_OP;
	}

	auto tokenizer_t::operator_two_chars(const char c1, const char c2) -> token_type_e
	{
		switch (c1)
		{
		case '+':
			switch (c2)
			{
			case '=':
				return TOK_ADDEQUAL;
			case '+':
				return TOK_DOUBLEADD;
			}
			break;
		case '-':
			switch (c2)
			{
			case '=':
				return TOK_MINUSEQUAL;
			case '-':
				return TOK_DOUBLEMINUS;
			}
			break;
		case '*':
			switch (c2)
			{
			case '*':
				return TOK_DOUBLESTAR;
			case '=':
				return TOK_STAREQUAL;
			}
			break;
		case '/':
			switch (c2)
			{
			case '/':
				return TOK_DOUBLEFSLASH;
			case '=':
				return TOK_FSLASHEQUAL;
			}
			break;
		case '=':
			switch (c2)
			{
			case '=':
				return TOK_EQUALEQUAL;
			}
			break;
		case '!':
			switch (c2)
			{
			case '=':
				return TOK_NOTEQUAL;
			}
			break;
		case '>':
			switch (c2)
			{
			case '=':
				return TOK_GREATEREQUAL;
			case '>':
				return TOK_RIGHTSHIFT;
			}
			break;
		case '<':
			switch (c2)
			{
			case '=':
				return TOK_LESSEQUAL;
			case '<':
				return TOK_LEFTSHIFT;
			case '>':
				return TOK_NOTEQUAL;
			}
			break;
		case '.':
			switch (c2)
			{
			case '.':
				return TOK_ELLIPSIS;
			}
			break;
		case ':':
			switch (c2)
			{
			case '=':
				return TOK_COLONEQUAL;
			}
			break;
		}
		return TOK_OP;
	}

	auto tokenizer_t::operator_three_chars(const char c1, const char c2, const char c3) -> token_type_e
	{
		switch (c1)
		{
		case '<':
			switch (c2)
			{
			case '=':
				switch (c3)
				{
				case '>':
					return TOK_LEQUALG;
				}
			}
		}
		return TOK_OP;
	}

}
