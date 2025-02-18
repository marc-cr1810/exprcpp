#pragma once

#include "exprcpp/symbol_table.hpp"

namespace exprcpp
{

	template<typename T>
	class expression_t
	{
	public:
		expression_t() = default;
		~expression_t() = default;

		auto register_symbol_table(const symbol_table_t<T> symbol_table) -> void;
	private:
		symbol_table_t<T> m_symbol_table;
	};

}

#include "expression.inl"