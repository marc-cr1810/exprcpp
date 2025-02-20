#include "expression.hpp"

namespace exprcpp
{
	template<class Integer, typename Enable = void> 
	struct convert_to_number_t 
	{ 
		auto operator()(std::string const& str) const -> Integer 
		{ 
			return std::stoi(str); 
		} 
	}; 
	
	template<class Integer>
	struct convert_to_number_t<Integer, std::enable_if_t<std::is_same<long, Integer>::value>> 
	{
		auto operator()(std::string const& str) const -> long 
		{
			return std::stol(str); 
		} 
	}; 
	
	template<class Float> 
	struct convert_to_number_t<Float, std::enable_if_t<std::is_same<float, Float>::value>> 
	{
		auto operator()(std::string const& str) const -> float 
		{ 
			return std::stof(str); 
		} 
	}; 
	
	template<class Float> 
	struct convert_to_number_t<Float, std::enable_if_t<std::is_same<double, Float>::value>> 
	{ 
		auto operator()(std::string const& str) const -> double 
		{ 
			return std::stod(str); 
		} 
	}; 
	
	template<class T, class StringLike> 
	auto to_number(StringLike&& str) -> T 
	{ 
		using type = std::decay_t<T>;
		return convert_to_number_t<type>()(str); 
	};

	template<typename T>
	inline auto expression_t<T>::value() -> T
	{
		if (m_ast == nullptr)
		{
			return T();
		}

		while (!m_stack.empty())
		{
			m_stack.pop();
		}

		for (const auto& statement : m_ast->elements)
		{
			if (!execute_statement(statement))
			{
				return T();
			}
		}

		auto result = T();
		if (!m_stack.empty())
		{
			result = m_stack.top();
			m_stack.pop();
		}
		return result;
	}

	template<typename T>
	auto expression_t<T>::register_symbol_table(const symbol_table_t<T> symbol_table) -> void
	{
		m_symbol_table = symbol_table;
	}

	template<typename T>
	auto expression_t<T>::set_ast(const internal::ast::stmt_seq_ptr_t& ast) -> void
	{
		m_ast = ast;
	}

	template<typename T>
	auto expression_t<T>::execute_statement(const internal::ast::stmt_ptr_t& statement) -> bool
	{
		if (statement == nullptr)
		{
			return false;
		}

		switch (statement->kind)
		{
		case internal::ast::statement_kind_e::expr:
		{
			auto expr = std::get<internal::ast::statement_t::stmt_expr_t>(statement->value);
			return execute_expression(expr.value);
		}
		}

		return false;
	}

	template<typename T>
	auto expression_t<T>::execute_expression(const internal::ast::expr_ptr_t& expression) -> bool
	{
		if (expression == nullptr)
		{
			return false;
		}

		switch (expression->kind)
		{
		case internal::ast::expression_kind_e::bool_op:
		{
			auto bool_op = std::get<internal::ast::expression_t::expr_bool_op_t>(expression->value);
			return execute_bool_op(bool_op.op, bool_op.values);
		}
		case internal::ast::expression_kind_e::bin_op:
		{
			auto bin_op = std::get<internal::ast::expression_t::expr_bin_op_t>(expression->value);
			return execute_bin_op(bin_op.left, bin_op.op, bin_op.right);
		}
		case internal::ast::expression_kind_e::unary_op:
		{
			auto unary_op = std::get<internal::ast::expression_t::expr_unary_op_t>(expression->value);
			return execute_unary_op(unary_op.op, unary_op.right);
		}
		case internal::ast::expression_kind_e::cmp_op:
		{
			auto cmp_op = std::get<internal::ast::expression_t::expr_cmp_op_t>(expression->value);
			return execute_cmp_op(cmp_op.left, cmp_op.op, cmp_op.right);
		}
		case internal::ast::expression_kind_e::assign:
		{
			auto assign = std::get<internal::ast::expression_t::expr_assign_t>(expression->value);
			return execute_assign(assign.id, assign.value);
		}
		case internal::ast::expression_kind_e::constant:
		{
			auto constant = std::get<internal::ast::expression_t::expr_constant_t>(expression->value);
			return execute_constant(constant.value);
		}
		case internal::ast::expression_kind_e::name:
		{
			auto name = std::get<internal::ast::expression_t::expr_name_t>(expression->value);
			return execute_name(name.id, name.context);
		}
		}

		return false;
	}

	template<typename T>
	auto expression_t<T>::execute_bool_op(internal::ast::bool_op_type_t op, const internal::ast::expr_seq_ptr_t& values) -> bool
	{
		return false;
	}

	template<typename T>
	auto expression_t<T>::execute_bin_op(const internal::ast::expr_ptr_t& left, internal::ast::operator_type_t op, const internal::ast::expr_ptr_t& right) -> bool
	{
		if ((left == nullptr || right == nullptr) || (!execute_expression(left) || !execute_expression(right)))
		{
			return false;
		}

		T right_value = m_stack.top();
		m_stack.pop();
		T left_value = m_stack.top();
		m_stack.pop();

		switch (op)
		{
		case internal::ast::operator_type_t::add: m_stack.push(left_value + right_value); return true;
		case internal::ast::operator_type_t::sub: m_stack.push(left_value - right_value); return true;
		case internal::ast::operator_type_t::mult: m_stack.push(left_value * right_value); return true;
		case internal::ast::operator_type_t::div: m_stack.push(left_value / right_value); return true;
		case internal::ast::operator_type_t::mod: m_stack.push(std::fmod(left_value, right_value)); return true;
		case internal::ast::operator_type_t::pow: m_stack.push(std::pow(left_value, right_value)); return true;
		}

		return false;
	}

	template<typename T>
	auto expression_t<T>::execute_unary_op(internal::ast::unary_op_type_t op, const internal::ast::expr_ptr_t& right) -> bool
	{
		if (right == nullptr || !execute_expression(right))
		{
			return false;
		}

		T right_value = m_stack.top();
		m_stack.pop();

		switch (op)
		{
		case internal::ast::unary_op_type_t::invert: m_stack.push(static_cast<T>(~static_cast<uint64_t>(right_value))); return true;
		case internal::ast::unary_op_type_t::Not: m_stack.push(!right_value); return true;
		case internal::ast::unary_op_type_t::add: m_stack.push(+right_value); return true;
		case internal::ast::unary_op_type_t::sub: m_stack.push(-right_value); return true;
		}

		return false;
	}

	template<typename T>
	inline auto expression_t<T>::execute_cmp_op(const internal::ast::expr_ptr_t& left, internal::ast::cmp_op_type_t op, const internal::ast::expr_ptr_t& right) -> bool
	{
		if ((left == nullptr || right == nullptr) || (!execute_expression(left) || !execute_expression(right)))
		{
			return false;
		}

		T right_value = m_stack.top();
		m_stack.pop();
		T left_value = m_stack.top();
		m_stack.pop();

		bool value = false;
		switch (op)
		{
		case internal::ast::cmp_op_type_t::eq: value = left_value == right_value; break;
		case internal::ast::cmp_op_type_t::Not_eq: value = left_value != right_value; break;
		case internal::ast::cmp_op_type_t::lt: value = left_value < right_value; break;
		case internal::ast::cmp_op_type_t::lt_eq: value = left_value <= right_value; break;
		case internal::ast::cmp_op_type_t::gt: value = left_value > right_value; break;
		case internal::ast::cmp_op_type_t::gt_eq: value = left_value >= right_value; break;
		}

		m_stack.push(value ? T(1) : T(0));
		return true;
	}

	template<typename T>
	inline auto expression_t<T>::execute_assign(const std::string& id, const internal::ast::expr_ptr_t& value) -> bool
	{
		if (value == nullptr || !execute_expression(value))
		{
			return false;
		}

		auto result = execute_name(id, internal::ast::expr_context_type_e::store);
	}

	template<typename T>
	auto expression_t<T>::execute_constant(const std::string& value) -> bool
	{
		m_stack.push(to_number<T>(value));
		return true;
	}

	template<typename T>
	auto expression_t<T>::execute_name(const std::string& id, internal::ast::expr_context_type_e context) -> bool
	{
		switch (context)
		{
		case internal::ast::expr_context_type_e::load:
		{
			if (!m_symbol_table.has(id))
			{
				return false;
			}

			m_stack.push(m_symbol_table[id]);
			return true;
		}
		case internal::ast::expr_context_type_e::store:
		{
			T value = m_stack.top();

			if (m_symbol_table.has(id))
			{
				m_symbol_table[id] = value;
			}
			m_symbol_table.add_variable(id, value);
			return true;
		}
		case internal::ast::expr_context_type_e::del:
		{
			return false;
		}

		return false;
		}
	}

}