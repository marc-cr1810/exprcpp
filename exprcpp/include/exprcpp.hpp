#pragma once

#include "exprcpp/symbol_table.hpp"
#include "exprcpp/expression.hpp"

namespace exprcpp
{

	template<typename T>
	auto compile(const std::string& expression_string, expression_t<T>& expression) -> int;

}

#include "exprcpp.inl"