#pragma once

#include <algorithm>
#include <cmath>

namespace exprcpp
{

	template<typename T>
	class function_t
	{
	public:
		explicit function_t(const size_t& num_args);
		virtual ~function_t() = default;

		auto num_args() const -> const size_t;

#define empty_method_body(N)						 \
    {                                              \
        return std::numeric_limits<T>::quiet_NaN(); \
    }

		inline virtual T operator() () empty_method_body(0);
		inline virtual T operator() (const T&) empty_method_body(1);
		inline virtual T operator() (const T&, const T&) empty_method_body(2);
		inline virtual T operator() (const T&, const T&, const T&) empty_method_body(3);
		inline virtual T operator() (const T&, const T&, const T&, const T&) empty_method_body(4);

#undef empty_method_body
	private:
		const size_t m_num_args;
	};

	template<typename T>
	struct abs_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		abs_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			return T(std::abs(v));
		}
	};

	template<typename T>
	struct ceil_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		ceil_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			return T(std::ceil(v));
		}
	};

	template<typename T>
	struct clamp_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		clamp_ipml_t()
			: exprcpp::function_t<T>(3)
		{
		}

		auto operator()(const T& lo, const T&v, const T& hi) -> T
		{
			return T(std::clamp(v, lo, hi));
		}
	};
	
	template<typename T>
	struct floor_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		floor_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			return T(std::floor(v));
		}
	};

	template<typename T>
	struct frac_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		frac_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			T int_val;
			return T(std::modf(v, &int_val));
		}
	};

	template<typename T>
	struct inrange_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		inrange_ipml_t()
			: exprcpp::function_t<T>(3)
		{
		}

		auto operator()(const T& lo, const T& v, const T& hi) -> T
		{
			return T(v >= lo && v <= hi);
		}
	};

	template<typename T>
	struct log_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		log_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			return T(std::log(v));
		}
	};

	template<typename T>
	struct log10_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		log10_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			return T(std::log10(v));
		}
	};

	template<typename T>
	struct log1p_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		log1p_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			return T(std::log1p(v));
		}
	};

	template<typename T>
	struct log2_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		log2_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			return T(std::log2(v));
		}
	};

	template<typename T>
	struct round_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		round_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			T int_val;
			T frac = std::modf(v, &int_val);

			if (frac >= 0.5)
			{
				int_val++;
			}

			return T(int_val);
		}
	};

	template<typename T>
	struct trunc_ipml_t : public exprcpp::function_t<T>
	{
		using exprcpp::function_t<T>::operator();

		trunc_ipml_t()
			: exprcpp::function_t<T>(1)
		{
		}

		auto operator()(const T& v) -> T
		{
			T int_val;
			T frac = std::modf(v, &int_val);
			return T(int_val);
		}
	};

}

#include "function.inl"