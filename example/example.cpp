#include <iostream>

#include <exprcpp.hpp>

int main()
{
    double x = 1.0;
    double y = 1.5;

    exprcpp::symbol_table_t<double> symbol_table;
    exprcpp::expression_t<double> expression;

    symbol_table.add_variable("x", &x);
    symbol_table.add_variable("y", &y);
    symbol_table.add_constants();

    expression.register_symbol_table(symbol_table);

    std::string string_expression;
    while (true)
    {
        std::cout << ">>> ";
        std::getline(std::cin, string_expression);

        if (exprcpp::compile(string_expression, expression) == EXIT_SUCCESS)
        {
            std::cout << expression.value() << std::endl;
        }
    }
}