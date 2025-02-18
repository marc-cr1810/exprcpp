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

		auto compile() -> bool;
	private:
		auto fill_token() -> token_type_e;
		auto expect_token(token_type_e type) -> bool;
		auto expect_token(token_type_e type, token_ptr_t& token) -> bool;

		auto is_memoized(int type) -> bool;
		auto insert_memo(size_t mark, int type) -> bool;
		auto update_memo(size_t mark, int type) -> bool;

		auto rule_statement() -> int;
		auto rule_expression() -> int;
		auto raw_sum() -> int;
		auto rule_sum() -> int;
		auto raw_term() -> int;
		auto rule_term() -> int;
		auto rule_factor() -> int;
		auto rule_power() -> int;
		auto raw_primary() -> int;
		auto rule_primary() -> int;
		auto rule_atom() -> int;
	private:
		tokenizer_t m_tokenizer;
		std::vector<token_ptr_t> m_tokens;

		size_t m_mark = 0;
	};

}