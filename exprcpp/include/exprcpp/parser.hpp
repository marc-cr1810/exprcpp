#pragma once

#include <vector>

#include "exprcpp/tokenizer.hpp"

namespace exprcpp::internal
{

	class parser_t
	{
	public:
		parser_t(const std::string& expression_string);
		~parser_t() = default;

		auto compile() -> ast::stmt_seq_ptr_t;
	private:
		auto fill_token() -> token_type_e;
		auto expect_token(token_type_e type) -> bool;
		auto expect_token(token_type_e type, token_ptr_t& token) -> bool;

		auto is_memoized(int type, ast::node_ptr_t& pres) -> bool;
		auto insert_memo(size_t mark, int type, ast::node_ptr_t node) -> bool;
		auto update_memo(size_t mark, int type, ast::node_ptr_t node) -> bool;

		auto rule_statements() -> ast::stmt_seq_ptr_t;
		auto rule_statement() -> ast::stmt_ptr_t;
		auto rule_expression() -> ast::expr_ptr_t;
		auto rule_assignment() -> ast::expr_ptr_t;
		auto rule_comparison() -> ast::expr_ptr_t;
		auto raw_sum() -> ast::expr_ptr_t;
		auto rule_sum() -> ast::expr_ptr_t;
		auto raw_term() -> ast::expr_ptr_t;
		auto rule_term() -> ast::expr_ptr_t;
		auto rule_factor() -> ast::expr_ptr_t;
		auto rule_power() -> ast::expr_ptr_t;
		auto raw_primary() -> ast::expr_ptr_t;
		auto rule_primary() -> ast::expr_ptr_t;
		auto rule_atom() -> ast::expr_ptr_t;
	private:
		tokenizer_t m_tokenizer;
		std::vector<token_ptr_t> m_tokens;

		size_t m_mark = 0;
	};

}