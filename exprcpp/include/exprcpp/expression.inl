#include "expression.hpp"

namespace exprcpp
{

	template<typename T>
	inline auto expression_t<T>::register_symbol_table(const symbol_table_t<T> symbol_table) -> void
	{
		m_symbol_table = symbol_table;
	}

}