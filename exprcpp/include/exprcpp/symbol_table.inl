#include "symbol_table.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

namespace exprcpp
{
    namespace constants
    {

        const std::vector<std::string> keywords =
        {
            "in",
            "not",
            "if",
            "else"
        };

    }

    template<typename T>
    symbol_table_t<T>::symbol_table_t()
    {
        add_functions(*this);
    }

    template<typename T>
    auto symbol_table_t<T>::add_constants() -> void
    {
        add_constant("true", T(true));
        add_constant("false", T(false));

        add_constant("e", M_E);
        add_constant("pi", M_PI);
        add_constant("pi_2", M_PI_2);
        add_constant("pi_4", M_PI_4);
        add_constant("sqrt2", M_SQRT2);
        add_constant("log2", M_LOG2E);
    }

    template<typename T>
    auto symbol_table_t<T>::add_constant(const std::string& name, const T& value) -> bool
    {
        if (has(name))
        {
            return false;
        }
        m_constants[name] = value;
        return false;
    }

    template<typename T>
    auto symbol_table_t<T>::add_variable(const std::string& name, const T& value) -> bool
    {
        if (has(name))
        {
            return false;
        }
        m_dynamic[name] = value;
        return false;
    }

    template<typename T>
    auto symbol_table_t<T>::add_variable(const std::string& name, T* variable) -> bool
    {
        if (has(name) || variable == nullptr)
        {
            return false;
        }
        m_variables[name] = variable;
        return true;
    }

    template<typename T>
    auto symbol_table_t<T>::add_function(const std::string& name, function_ptr_t func) -> bool
    {
        if (m_functions.find(name) != m_functions.end())
        {
            return false;
        }
        m_functions[name] = func;
        return true;
    }

    template<typename T>
    auto symbol_table_t<T>::has(const std::string& name) const -> bool
    {
        return std::count(constants::keywords.begin(), constants::keywords.end(), name) > 0 ||
               has_constant(name) || has_variable(name);
    }

    template<typename T>
    auto symbol_table_t<T>::has_constant(const std::string& name) const -> bool
    {
        return m_constants.find(name) != m_constants.end();
    }

    template<typename T>
    auto symbol_table_t<T>::has_variable(const std::string& name) const -> bool
    {
        return m_variables.find(name) != m_variables.end() || m_dynamic.find(name) != m_dynamic.end();
    }

    template<typename T>
    inline auto symbol_table_t<T>::has_function(const std::string& name) const -> bool
    {
        return m_functions.find(name) != m_functions.end();
    }

    template<typename T>
    inline auto symbol_table_t<T>::get_constant(const std::string& name) -> T&
    {
        return m_constants[name];
    }

    template<typename T>
    inline auto symbol_table_t<T>::get_variable(const std::string& name) -> T&
    {
        if (m_variables.find(name) != m_variables.end())
        {
            return *m_variables[name];
        }
        return m_dynamic[name];
    }

    template<typename T>
    inline auto symbol_table_t<T>::get_function(const std::string& name) -> function_t<T>*
    {

        return m_functions[name];
    }

    template<typename T>
    inline auto symbol_table_t<T>::operator[](const std::string& name) const -> const T&
    {
        if (has_variable(name))
        {
            return get_variable(name);
        }
        return get_constant(name);
    }

    template<typename T>
    inline auto symbol_table_t<T>::operator[](const std::string& name) -> T&
    {
        if (has_variable(name))
        {
            return get_variable(name);
        }
        return get_constant(name);
    }

    template<typename T>
    auto symbol_table_t<T>::add_functions(symbol_table_t& symbol_table) -> void
    {
        static abs_ipml_t<T> abs_impl;
        static ceil_ipml_t<T> ceil_impl;
        static clamp_ipml_t<T> clamp_impl;
        static floor_ipml_t<T> floor_impl;
        static frac_ipml_t<T> frac_impl;
        static inrange_ipml_t<T> inrange_impl;
        static log_ipml_t<T> log_impl;
        static log10_ipml_t<T> log10_impl;
        static log1p_ipml_t<T> log1p_impl;
        static log2_ipml_t<T> log2_impl;
        static round_ipml_t<T> round_impl;
        static trunc_ipml_t<T> trunc_impl;

        symbol_table.add_function("abs", &abs_impl);
        symbol_table.add_function("ceil", &ceil_impl);
        symbol_table.add_function("clamp", &clamp_impl);
        symbol_table.add_function("floor", &floor_impl);
        symbol_table.add_function("frac", &frac_impl);
        symbol_table.add_function("inrange", &inrange_impl);
        symbol_table.add_function("log", &log_impl);
        symbol_table.add_function("log10", &log10_impl);
        symbol_table.add_function("log1p", &log1p_impl);
        symbol_table.add_function("log2", &log2_impl);
        symbol_table.add_function("round", &round_impl);
        symbol_table.add_function("trunc", &trunc_impl);
    }

}