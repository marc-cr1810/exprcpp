#pragma once

#include "exprcpp/expression.hpp"
#include "exprcpp/function.hpp"
#include "exprcpp/symbol_table.hpp"

namespace exprcpp
{

	template<typename T>
	auto compile(const std::string& expression_string, expression_t<T>& expression) -> int;

}

#include "exprcpp.inl"