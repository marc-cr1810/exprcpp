#include <iostream>

#include <exprcpp.hpp>

template<typename T>
struct my_function_t : public exprcpp::function_t<T>
{
    using exprcpp::function_t<T>::operator();

    my_function_t()
        : exprcpp::function_t<T>(2)
    { }

    auto operator()(const T& v1, const T& v2) -> T
    {
        return T(1) + (v1 * v2) / T(3);
    }
};

int main()
{
    my_function_t<double> my_func;

    double x = 1.0;
    double y = 1.5;

    exprcpp::symbol_table_t<double> symbol_table;
    exprcpp::expression_t<double> expression;

    symbol_table.add_variable("x", &x);
    symbol_table.add_variable("y", &y);
    symbol_table.add_constants();
    symbol_table.add_function("my_func", &my_func);

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