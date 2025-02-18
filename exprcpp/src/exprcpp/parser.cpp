#include "exprcpp/parser.hpp"

#include <iostream>

namespace exprcpp::internal
{

	namespace memo_type
	{
		const auto sum		= int(1);
		const auto term		= int(2);
		const auto factor	= int(3);
		const auto primary	= int(4);
	}

	parser_t::parser_t(const std::string& expression_string)
		: m_tokenizer(expression_string)
	{ }

	auto parser_t::compile() -> bool
	{
		return rule_statement();
	}

	auto parser_t::fill_token() -> token_type_e
	{
		auto token = m_tokenizer.get();
		m_tokens.push_back(token);
		return token->type;
	}

	auto parser_t::expect_token(token_type_e type) -> bool
	{
		if (m_mark == m_tokens.size())
		{
			if (fill_token() < 0)
			{
				return false;
			}
		}

		auto token = m_tokens[m_mark];
		if (token->type != type)
		{
			return false;
		}
		m_mark++;
		return true;
	}

	auto parser_t::expect_token(token_type_e type, token_ptr_t& token) -> bool
	{
		if (m_mark == m_tokens.size())
		{
			if (fill_token() < 0)
			{
				return false;
			}
		}

		token = m_tokens[m_mark];
		if (token->type != type)
		{
			return false;
		}
		m_mark++;
		return true;
	}

	auto parser_t::is_memoized(int type, int& pres) -> bool
	{
		if (m_mark == m_tokens.size())
		{
			if (fill_token() < 0)
			{
				return false;
			}
		}

		auto token = m_tokens[m_mark];

		for (auto memo = token->memo; memo != nullptr; memo = memo->next)
		{
			if (memo->type == type)
			{
				m_mark = memo->mark;
				pres = memo->node;
				return true;
			}
		}
		return false;
	}

	auto parser_t::insert_memo(size_t mark, int type, int& node) -> bool
	{
		auto memo = std::make_shared<memo_t>();
		if (memo == nullptr)
		{
			return false;
		}
		memo->type = type;
		memo->node = node;
		memo->mark = mark;
		memo->next = m_tokens[mark]->memo;
		m_tokens[mark]->memo = memo;
		return true;
	}

	auto parser_t::update_memo(size_t mark, int type, int& node) -> bool
	{
		for (auto memo = m_tokens[mark]->memo; memo != nullptr; memo = memo->next)
		{
			if (memo->type == type)
			{
				memo->node = node;
				memo->mark = m_mark;
				return true;
			}
		}
		return insert_memo(mark, type, node);
	}

	auto parser_t::rule_statement() -> int
	{
		std::cout << "rule statement:" << std::endl;

		auto mark = m_mark;
		{ // expression NEWLINE
			if (
				rule_expression() &&
				expect_token(TOK_NEWLINE)
				)
			{
				std::cout << "  expression NEWLINE" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // expression ENDMARKER
			if (
				rule_expression() &&
				expect_token(TOK_ENDMARKER)
				)
			{
				std::cout << "  expression ENDMARKER" << std::endl;
				return 1;
			}
		}

		return 0;
	}

	auto parser_t::rule_expression() -> int
	{
		std::cout << "rule expression:" << std::endl;

		auto mark = m_mark;
		{ // sum
			if (
				rule_sum()
				)
			{
				std::cout << "  sum" << std::endl;
				return 1;
			}
		}

		return 0;
	}

	auto parser_t::raw_sum() -> int
	{
		std::cout << "raw sum:" << std::endl;

		auto mark = m_mark;
		{ // sum '+' term
			if (
				rule_sum() &&
				expect_token(TOK_ADD) &&
				rule_term()
				)
			{
				std::cout << "  sum + term" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // sum '-' term
			if (
				rule_sum() &&
				expect_token(TOK_MINUS) &&
				rule_term()
				)
			{
				std::cout << "  sum - term" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // term
			if (
				rule_term()
				)
			{
				std::cout << "  term" << std::endl;
				return 1;
			}
		}

		return 0;
	}

	auto parser_t::rule_sum() -> int
	{
		int result = 0;
		if (is_memoized(memo_type::sum, result))
		{
			return result;
		}
		auto mark = m_mark;
		auto resmark = m_mark;
		while (true)
		{
			if (!update_memo(mark, memo_type::sum, result))
			{
				return result;
			}
			m_mark = mark;
			auto raw = raw_sum();
			if (raw == 0 || m_mark <= resmark)
			{
				break;
			}
			resmark = m_mark;
			result = raw;
		}
		m_mark = resmark;
		return result;
	}

	auto parser_t::raw_term() -> int
	{
		std::cout << "raw term:" << std::endl;

		auto mark = m_mark;
		{ // term '*' factor
			if (
				rule_term() &&
				expect_token(TOK_STAR) &&
				rule_factor()
				)
			{
				std::cout << "  term * factor" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // term '/' factor
			if (
				rule_term() &&
				expect_token(TOK_FSLASH) &&
				rule_factor()
				)
			{
				std::cout << "  term / factor" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // factor
			if (
				rule_factor()
				)
			{
				std::cout << "  factor" << std::endl;
				return 1;
			}
		}
		return 0;
	}

	auto parser_t::rule_term() -> int
	{
		std::cout << "rule term:" << std::endl;

		int result = 0;
		if (is_memoized(memo_type::term, result))
		{
			return result;
		}
		auto mark = m_mark;
		auto resmark = m_mark;
		while (true)
		{
			if (!update_memo(mark, memo_type::term, result))
			{
				return result;
			}
			m_mark = mark;
			auto raw = raw_term();
			if (raw == 0 || m_mark <= resmark)
			{
				break;
			}
			resmark = m_mark;
			result = raw;
		}
		m_mark = resmark;
		return result;
	}

	auto parser_t::rule_factor() -> int
	{
		std::cout << "rule factor:" << std::endl;

		int result = 0;
		if (is_memoized(memo_type::factor, result))
		{
			return result;
		}

		auto mark = m_mark;
		{ // '+' factor
			if (
				expect_token(TOK_ADD) &&
				rule_factor()
				)
			{
				std::cout << "  + factor" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // '-' factor
			if (
				expect_token(TOK_MINUS) &&
				rule_factor()
				)
			{
				std::cout << "  - factor" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // '~' factor
			if (
				expect_token(TOK_TILDE) &&
				rule_factor()
				)
			{
				std::cout << "  ~ factor" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // power
			if (
				rule_power()
				)
			{
				std::cout << "  power" << std::endl;
				return 1;
			}
		}

		return 0;
	}

	auto parser_t::rule_power() -> int
	{
		std::cout << "rule power:" << std::endl;

		auto mark = m_mark;
		{ // primary '**' factor
			if (
				rule_primary() &&
				expect_token(TOK_DOUBLESTAR) &&
				rule_factor()
				)
			{
				std::cout << "  primary ** factor" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // primary
			if (
				rule_primary()
				)
			{
				std::cout << "  primary" << std::endl;
				return 1;
			}
		}
		return 0;
	}

	auto parser_t::raw_primary() -> int
	{
		std::cout << "raw primary:" << std::endl;

		auto mark = m_mark;
		{ // atom
			if (
				rule_atom()
				)
			{
				std::cout << "  atom" << std::endl;
				return 1;
			}
		}

		return 0;
	}

	auto parser_t::rule_primary() -> int
	{
		std::cout << "rule primary:" << std::endl;

		int result = 0;
		if (is_memoized(memo_type::primary, result))
		{
			return result;
		}
		auto mark = m_mark;
		auto resmark = m_mark;
		while (true)
		{
			if (!update_memo(mark, memo_type::primary, result))
			{
				return result;
			}
			m_mark = mark;
			auto raw = raw_primary();
			if (raw == 0 || m_mark <= resmark)
			{
				break;
			}
			resmark = m_mark;
			result = raw;
		}
		m_mark = resmark;
		return result;
	}

	auto parser_t::rule_atom() -> int
	{
		std::cout << "rule atom:" << std::endl;

		auto mark = m_mark;
		{ // NAME
			token_ptr_t token;
			if (
				expect_token(TOK_NAME, token)
				)
			{
				std::cout << "  '" << token->value << "'" << std::endl;
				return 1;
			}
		}
		m_mark = mark;
		{ // NUMBER
			token_ptr_t token;
			if (
				expect_token(TOK_NUMBER, token)
				)
			{
				std::cout << "  " << token->value << std::endl;
				return 1;
			}
		}
		return 0;
	}

}