#pragma once

#include <string>
#include <unordered_map>

namespace exprcpp
{

	template<typename T>
	class symbol_table_t
	{
	public:
		symbol_table_t() = default;
		~symbol_table_t() = default;

		auto add_constants() -> void;
		auto add_constant(const std::string& name, T value) -> bool;
		auto add_variable(const std::string& name, T* variable) -> bool;

		inline auto has(const std::string& name) const -> bool;
		inline auto has_constant(const std::string& name) const -> bool;
		inline auto has_variable(const std::string& name) const -> bool;
	private:
		std::unordered_map<std::string, T> m_constants;
		std::unordered_map<std::string, T*> m_variables;
	};

}

#include "symbol_table.inl"