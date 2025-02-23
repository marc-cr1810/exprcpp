#pragma once

#include "exprcpp/function.hpp"
#include <string>
#include <unordered_map>

namespace exprcpp
{
	template<typename T>
	class symbol_table_t
	{
	public:
		typedef function_t<T>* function_ptr_t;

		symbol_table_t();
		~symbol_table_t() = default;

		auto add_constants() -> void;
		auto add_constant(const std::string& name, const T& value) -> bool;
		auto add_variable(const std::string& name, const T& value) -> bool;
		auto add_variable(const std::string& name, T* variable) -> bool;
		auto add_function(const std::string& name, function_ptr_t func) -> bool;

		inline auto has(const std::string& name) const -> bool;
		inline auto has_constant(const std::string& name) const -> bool;
		inline auto has_variable(const std::string& name) const -> bool;
		inline auto has_function(const std::string& name) const -> bool;

		inline auto get_constant(const std::string& name) -> T&;
		inline auto get_variable(const std::string& name) -> T&;
		inline auto get_function(const std::string& name) -> function_t<T>*;

		inline auto operator[](const std::string& name) const -> const T&;
		inline auto operator[](const std::string& name) -> T&;
	private:
		static auto add_functions(symbol_table_t& symbol_table) -> void;
	private:
		std::unordered_map<std::string, T> m_constants;
		std::unordered_map<std::string, T> m_dynamic;
		std::unordered_map<std::string, T*> m_variables;
		std::unordered_map<std::string, function_ptr_t> m_functions;
	};

}

#include "symbol_table.inl"