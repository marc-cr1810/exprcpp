#include "exprcpp/parser.hpp"

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

	auto parser_t::compile() -> ast::stmt_seq_ptr_t
	{
		return rule_statements();
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

	auto parser_t::is_memoized(int type, ast::node_ptr_t& pres) -> bool
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

	auto parser_t::insert_memo(size_t mark, int type, ast::node_ptr_t node) -> bool
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

	auto parser_t::update_memo(size_t mark, int type, ast::node_ptr_t node) -> bool
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

	auto parser_t::rule_statements() -> ast::stmt_seq_ptr_t
	{
		auto mark = m_mark;
		auto start_mark = m_mark;
		std::vector<ast::stmt_ptr_t> elements;

		{ // statement+
			ast::stmt_ptr_t statement;
			while (
				(statement = rule_statement())
				)
			{
				elements.push_back(statement);
				mark = m_mark;
			}
			m_mark = mark;
		}

		if (elements.empty())
		{
			return nullptr;
		}

		auto statements = std::make_shared<ast::stmt_seq_t>();
		statements->elements = elements;
		return statements;
	}

	auto parser_t::rule_statement() -> ast::stmt_ptr_t
	{
		auto mark = m_mark;
		{ // expression NEWLINE
			ast::expr_ptr_t expr;
			if (
				(expr = rule_expression()) &&
				expect_token(TOK_NEWLINE)
				)
			{
				return ast::expression(expr);
			}
		}
		m_mark = mark;
		{ // expression ENDMARKER
			ast::expr_ptr_t expr;
			if (
				(expr = rule_expression()) &&
				expect_token(TOK_ENDMARKER)
				)
			{
				return ast::expression(expr);
			}
		}

		m_mark = mark;
		return nullptr;
	}

	auto parser_t::rule_expression() -> ast::expr_ptr_t
	{
		auto mark = m_mark;
		{ // assignment
			ast::expr_ptr_t assign;
			if (
				(assign = rule_assignment())
				)
			{
				return assign;
			}
		}
		m_mark = mark;
		{ // sum
			ast::expr_ptr_t sum;
			if (
				(sum = rule_sum())
				)
			{
				return sum;
			}
		}

		m_mark = mark;
		return nullptr;
	}

	auto parser_t::rule_assignment() -> ast::expr_ptr_t
	{
		auto mark = m_mark;
		{ // NAME ':=' expression
			token_ptr_t name;
			ast::expr_ptr_t value;
			if (
				expect_token(TOK_NAME, name) &&
				expect_token(TOK_COLONEQUAL) &&
				(value = rule_expression())
				)
			{
				return ast::assign(name->value, value);
			}
		}
		m_mark = mark;
		return nullptr;
	}

	auto parser_t::rule_comparison() -> ast::expr_ptr_t
	{
		return ast::expr_ptr_t();
	}

	auto parser_t::raw_sum() -> ast::expr_ptr_t
	{
		auto mark = m_mark;
		{ // sum '+' term
			ast::expr_ptr_t left;
			ast::expr_ptr_t right;
			if (
				(left = rule_sum()) &&
				expect_token(TOK_ADD) &&
				(right = rule_term())
				)
			{
				return ast::bin_op(left, ast::operator_type_t::add, right);
			}
		}
		m_mark = mark;
		{ // sum '-' term
			ast::expr_ptr_t left;
			ast::expr_ptr_t right;
			if (
				(left = rule_sum()) &&
				expect_token(TOK_MINUS) &&
				(right = rule_term())
				)
			{
				return ast::bin_op(left, ast::operator_type_t::sub, right);
			}
		}
		m_mark = mark;
		{ // term
			ast::expr_ptr_t term;
			if (
				(term = rule_term())
				)
			{
				return term;
			}
		}
		m_mark = mark;
		return nullptr;
	}

	auto parser_t::rule_sum() -> ast::expr_ptr_t
	{
		ast::node_ptr_t result = 0;
		if (is_memoized(memo_type::sum, result))
		{
			return std::static_pointer_cast<ast::expression_t>(result);result;
		}
		auto mark = m_mark;
		auto resmark = m_mark;
		while (true)
		{
			if (!update_memo(mark, memo_type::sum, result))
			{
				return std::static_pointer_cast<ast::expression_t>(result);
			}
			m_mark = mark;
			auto raw = raw_sum();
			if (raw == nullptr || m_mark <= resmark)
			{
				break;
			}
			resmark = m_mark;
			result = raw;
		}
		m_mark = resmark;
		return std::static_pointer_cast<ast::expression_t>(result);
	}

	auto parser_t::raw_term() -> ast::expr_ptr_t
	{
		auto mark = m_mark;
		{ // term '*' factor
			ast::expr_ptr_t left;
			ast::expr_ptr_t right;
			if (
				(left = rule_term()) &&
				expect_token(TOK_STAR) &&
				(right = rule_factor())
				)
			{
				return ast::bin_op(left, ast::operator_type_t::mult, right);
			}
		}
		m_mark = mark;
		{ // term '/' factor
			ast::expr_ptr_t left;
			ast::expr_ptr_t right;
			if (
				(left = rule_term()) &&
				expect_token(TOK_FSLASH) &&
				(right = rule_factor())
				)
			{
				return ast::bin_op(left, ast::operator_type_t::div, right);
			}
		}
		m_mark = mark;
		{ // factor
			ast::expr_ptr_t factor;
			if (
				(factor = rule_factor())
				)
			{
				return factor;
			}
		}
		m_mark = mark;
		return nullptr;
	}

	auto parser_t::rule_term() -> ast::expr_ptr_t
	{
		ast::node_ptr_t result = 0;
		if (is_memoized(memo_type::term, result))
		{
			return std::static_pointer_cast<ast::expression_t>(result);
		}
		auto mark = m_mark;
		auto resmark = m_mark;
		while (true)
		{
			if (!update_memo(mark, memo_type::term, result))
			{
				return std::static_pointer_cast<ast::expression_t>(result);
			}
			m_mark = mark;
			auto raw = raw_term();
			if (raw == nullptr || m_mark <= resmark)
			{
				break;
			}
			resmark = m_mark;
			result = raw;
		}
		m_mark = resmark;
		return std::static_pointer_cast<ast::expression_t>(result);
	}

	auto parser_t::rule_factor() -> ast::expr_ptr_t
	{
		ast::node_ptr_t result = 0;
		if (is_memoized(memo_type::factor, result))
		{
			return std::static_pointer_cast<ast::expression_t>(result);
		}

		auto mark = m_mark;
		{ // '+' factor
			ast::expr_ptr_t right;
			if (
				expect_token(TOK_ADD) &&
				(right = rule_factor())
				)
			{
				return ast::unary_op(ast::unary_op_type_t::add, right);
			}
		}
		m_mark = mark;
		{ // '-' factor
			ast::expr_ptr_t right;
			if (
				expect_token(TOK_MINUS) &&
				(right = rule_factor())
				)
			{
				return ast::unary_op(ast::unary_op_type_t::sub, right);
			}
		}
		m_mark = mark;
		{ // '~' factor
			ast::expr_ptr_t right;
			if (
				expect_token(TOK_TILDE) &&
				(right = rule_factor())
				)
			{
				return ast::unary_op(ast::unary_op_type_t::invert, right);
			}
		}
		m_mark = mark;
		{ // power
			ast::expr_ptr_t power;
			if (
				(power = rule_power())
				)
			{
				return power;
			}
		}
		m_mark = mark;
		return nullptr;
	}

	auto parser_t::rule_power() -> ast::expr_ptr_t
	{
		auto mark = m_mark;
		{ // primary '**' factor
			ast::expr_ptr_t left;
			ast::expr_ptr_t right;
			if (
				(left = rule_primary()) &&
				expect_token(TOK_DOUBLESTAR) &&
				(right = rule_factor())
				)
			{
				return ast::bin_op(left, ast::operator_type_t::pow, right);
			}
		}
		m_mark = mark;
		{ // primary
			ast::expr_ptr_t primary;
			if (
				(primary = rule_primary())
				)
			{
				return primary;
			}
		}
		m_mark = mark;
		return nullptr;
	}

	auto parser_t::raw_primary() -> ast::expr_ptr_t
	{
		auto mark = m_mark;
		{ // atom
			ast::expr_ptr_t atom;
			if (
				(atom = rule_atom())
				)
			{
				return atom;
			}
		}
		m_mark = mark;
		return nullptr;
	}

	auto parser_t::rule_primary() -> ast::expr_ptr_t
	{
		ast::node_ptr_t result = 0;
		if (is_memoized(memo_type::primary, result))
		{
			return std::static_pointer_cast<ast::expression_t>(result);
		}
		auto mark = m_mark;
		auto resmark = m_mark;
		while (true)
		{
			if (!update_memo(mark, memo_type::primary, result))
			{
				return std::static_pointer_cast<ast::expression_t>(result);
			}
			m_mark = mark;
			auto raw = raw_primary();
			if (raw == nullptr || m_mark <= resmark)
			{
				break;
			}
			resmark = m_mark;
			result = raw;
		}
		m_mark = resmark;
		return std::static_pointer_cast<ast::expression_t>(result);
	}

	auto parser_t::rule_atom() -> ast::expr_ptr_t
	{
		auto mark = m_mark;
		{ // NAME
			token_ptr_t token;
			if (
				expect_token(TOK_NAME, token)
				)
			{
				return ast::name(token->value, ast::expr_context_type_e::load);
			}
		}
		m_mark = mark;
		{ // NUMBER
			token_ptr_t token;
			if (
				expect_token(TOK_NUMBER, token)
				)
			{
				return ast::constant(token->value);
			}
		}
		m_mark = mark;
		{ // '(' expression ')'
			ast::expr_ptr_t expr;
			if (
				expect_token(TOK_LPAREN) &&
				(expr = rule_expression()) &&
				expect_token(TOK_RPAREN)
				)
			{
				return expr;
			}
		}
		m_mark = mark;
		return nullptr;
	}

}