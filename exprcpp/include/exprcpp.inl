#include "exprcpp.hpp"

#include <iostream>

#include "exprcpp/parser.hpp"

template<typename T>
auto exprcpp::compile(const std::string& expression_string, expression_t<T>& expression) -> int
{
	internal::parser_t parser(expression_string);
	auto ast = parser.compile();
	if (!ast)
	{
		std::cerr << "Failed to parse '" << expression_string << "'" << std::endl;
		return EXIT_FAILURE;
	}

	expression.set_ast(ast);
	return EXIT_SUCCESS;
}