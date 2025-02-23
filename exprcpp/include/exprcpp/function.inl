#include "function.hpp"

namespace exprcpp
{

	template<typename T>
	function_t<T>::function_t(const size_t& num_args)
		: m_num_args(num_args)
	{ }

	template<typename T>
	inline auto function_t<T>::num_args() const -> const size_t
	{
		return m_num_args;
	}

}