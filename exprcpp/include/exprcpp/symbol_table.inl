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
    auto symbol_table_t<T>::add_constants() -> void
    {
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

}