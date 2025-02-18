#include <iostream>

#include <exprcpp.hpp>

int main()
{
    double x = 1.0;
    double y = 1.5;
    const std::string expression_string = "x + y";

    exprcpp::symbol_table_t<double> symbol_table;
    exprcpp::expression_t<double> expression;

    symbol_table.add_variable("x", &x);
    symbol_table.add_variable("y", &y);
    symbol_table.add_constants();

    expression.register_symbol_table(symbol_table);
    exprcpp::compile(expression_string, expression);
}