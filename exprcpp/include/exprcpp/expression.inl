#include "expression.hpp"

namespace exprcpp
{

	namespace internal
	{

		template<typename T>
		stack_object_t<T>::stack_object_t(T scalar)
		{
			type = stack_object_type_e::scalar;
			value = scalar;
		}

		template<typename T>
		stack_object_t<T>::stack_object_t(vector_t vector)
		{
			type = stack_object_type_e::vector;
			value = vector;
		}


#define stack_object_operator(op) \
		template<typename T> \
		constexpr auto operator op (const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T> \
		{ \
			if (lhs.type == stack_object_type_e::scalar) \
			{ \
				const auto lhs_scalar = std::get<T>(lhs.value); \
				if (rhs.type == stack_object_type_e::vector) \
				{ \
					const auto rhs_vector = std::get<std::vector<T>>(rhs.value); \
					std::vector<T> vector; \
					for (const auto& value : rhs_vector) \
					{ \
						vector.push_back(static_cast<T>(lhs_scalar op value)); \
					} \
					return stack_object_t(vector); \
				} \
				return stack_object_t(static_cast<T>(lhs_scalar op std::get<T>(rhs.value))); \
			} \
			const auto lhs_vector = std::get<std::vector<T>>(lhs.value); \
			std::vector<T> vector; \
			if (rhs.type == stack_object_type_e::vector) \
			{ \
				const auto rhs_vector = std::get<std::vector<T>>(rhs.value); \
				size_t size = std::max(lhs_vector.size(), rhs_vector.size()); \
				for (size_t i = 0; i < size; i++) \
				{ \
					if (i < lhs_vector.size() && i < rhs_vector.size()) \
					{ \
						vector.push_back(static_cast<T>(lhs_vector[i] op rhs_vector[i])); \
					} \
					else if (i < lhs_vector.size()) \
					{ \
						vector.push_back(lhs_vector[i]); \
					} \
					else \
					{ \
						vector.push_back(rhs_vector[i]); \
					} \
				} \
				return stack_object_t(vector); \
			} \
			const auto rhs_scalar = std::get<T>(rhs.value); \
			for (const auto& value : lhs_vector) \
			{ \
				vector.push_back(static_cast<T>(value op rhs_scalar)); \
			} \
			return stack_object_t(vector); \
		}

#define stack_object_std_func(func) \
		template<typename T> \
		constexpr auto func (const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T> \
		{ \
			if (lhs.type == stack_object_type_e::scalar) \
			{ \
				const auto lhs_scalar = std::get<T>(lhs.value); \
				if (rhs.type == stack_object_type_e::vector) \
				{ \
					const auto rhs_vector = std::get<std::vector<T>>(rhs.value); \
					std::vector<T> vector; \
					for (const auto& value : rhs_vector) \
					{ \
						vector.push_back(std:: func (lhs_scalar, value)); \
					} \
					return stack_object_t(vector); \
				} \
				return stack_object_t(std:: func (lhs_scalar, std::get<T>(rhs.value))); \
			} \
			const auto lhs_vector = std::get<std::vector<T>>(lhs.value); \
			std::vector<T> vector; \
			if (rhs.type == stack_object_type_e::vector) \
			{ \
				const auto rhs_vector = std::get<std::vector<T>>(rhs.value); \
				size_t size = std::max(lhs_vector.size(), rhs_vector.size()); \
				for (size_t i = 0; i < size; i++) \
				{ \
					if (i < lhs_vector.size() && i < rhs_vector.size()) \
					{ \
						vector.push_back(std:: func (lhs_vector[i], rhs_vector[i])); \
					} \
					else if (i < lhs_vector.size()) \
					{ \
						vector.push_back(lhs_vector[i]); \
					} \
					else \
					{ \
						vector.push_back(rhs_vector[i]); \
					} \
				} \
				return stack_object_t(vector); \
			} \
			const auto rhs_scalar = std::get<T>(rhs.value); \
			for (const auto& value : lhs_vector) \
			{ \
				vector.push_back(std:: func (value, rhs_scalar)); \
			} \
			return stack_object_t(vector); \
		}

		stack_object_operator(+);
		stack_object_operator(-);
		stack_object_operator(*);
		stack_object_operator(/);

		stack_object_operator(<);
		stack_object_operator(<=);
		stack_object_operator(>);
		stack_object_operator(>=);

		stack_object_std_func(pow);
		stack_object_std_func(fmod);

		template<typename T>
		constexpr auto operator==(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>
		{
			bool result = false;
			if (lhs.type == rhs.type)
			{
				if (lhs.type == stack_object_type_e::vector)
				{
					const auto lhs_vector = std::get<std::vector<T>>(lhs.value);
					const auto rhs_vector = std::get<std::vector<T>>(rhs.value);

					if (lhs_vector.size() == rhs_vector.size())
					{
						result = true;
						for (size_t i = 0; i < lhs_vector.size(); i++)
						{
							if (lhs_vector[i] != rhs_vector[i])
							{
								result = false;
								break;
							}
						}
					}
				}
				const auto lhs_scalar = std::get<T>(lhs.value);
				const auto rhs_scalar = std::get<T>(rhs.value);
				result = lhs_scalar == rhs_scalar;
			}
			return stack_object_t<T>(static_cast<T>(result));
		}

		template<typename T>
		constexpr auto operator!=(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>
		{
			bool result = true;
			if (lhs.type == rhs.type)
			{
				if (lhs.type == stack_object_type_e::vector)
				{
					const auto lhs_vector = std::get<std::vector<T>>(lhs.value);
					const auto rhs_vector = std::get<std::vector<T>>(rhs.value);

					if (lhs_vector.size() == rhs_vector.size())
					{
						result = true;
						for (size_t i = 0; i < lhs_vector.size(); i++)
						{
							if (lhs_vector[i] == rhs_vector[i])
							{
								result = false;
								break;
							}
						}
					}
				}
				const auto lhs_scalar = std::get<T>(lhs.value);
				const auto rhs_scalar = std::get<T>(rhs.value);
				result = lhs_scalar != rhs_scalar;
			}
			return stack_object_t<T>(static_cast<T>(result));
		}

		template<typename T>
		constexpr auto in(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>
		{
			if (lhs.type != stack_object_type_e::scalar || rhs.type != stack_object_type_e::vector)
			{
				return stack_object_t<T>(T(false));
			}

			const auto scalar = std::get<T>(lhs.value);
			const auto vector = std::get<std::vector<T>>(rhs.value);
			const auto count = std::count(vector.begin(), vector.end(), scalar);
			return stack_object_t<T>(T(count));
		}

		template<typename T>
		constexpr auto not_in(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>
		{
			if (lhs.type != stack_object_type_e::scalar || rhs.type != stack_object_type_e::vector)
			{
				return stack_object_t<T>(T(false));
			}

			const auto scalar = std::get<T>(lhs.value);
			const auto vector = std::get<std::vector<T>>(rhs.value);
			const auto count = std::count(vector.begin(), vector.end(), scalar);
			return stack_object_t<T>(T(count == 0));
		}
	}

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

		if (!m_stack.empty())
		{
			const auto result = m_stack.top();
			m_stack.pop();
			if (result.type == internal::stack_object_type_e::vector)
			{
				const auto vector = std::get<std::vector<T>>(result.value);
				if (vector.size() > 0)
				{
					return vector[0];
				}
			}
			else
			{
				return std::get<T>(result.value);
			}
		}
		return T();
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
		case internal::ast::statement_kind_e::if_else:
		{
			auto if_else = std::get<internal::ast::statement_t::stmt_if_else_t>(statement->value);
			return execute_if_else(if_else.condition, if_else.true_case, if_else.false_case);
		}
		case internal::ast::statement_kind_e::expr:
		{
			auto expr = std::get<internal::ast::statement_t::stmt_expr_t>(statement->value);
			return execute_expression(expr.value);
		}
		}

		return false;
	}

	template<typename T>
	inline auto expression_t<T>::execute_if_else(const internal::ast::expr_ptr_t& condition, const internal::ast::expr_ptr_t& true_case, const internal::ast::expr_ptr_t& false_case) -> bool
	{
		if (condition == nullptr || true_case == nullptr)
		{
			return false;
		}

		if (!execute_expression(condition))
		{
			return false;
		}
		const auto condition_value = m_stack.top();
		m_stack.pop();
		T cond = condition_value.type == internal::stack_object_type_e::scalar ? std::get<T>(condition_value.value) : T(0);
		if (condition_value.type == internal::stack_object_type_e::vector)
		{
			const auto vector = std::get<std::vector<T>>(condition_value.value);
			if (vector.size() > 0)
			{
				cond = vector[0];
			}
		}

		if (cond != 0 && execute_expression(true_case))
		{
			return true;
		}
		else if (false_case != nullptr && execute_expression(false_case))
		{
			return false;
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
		case internal::ast::expression_kind_e::vector:
		{
			auto vector = std::get<internal::ast::expression_t::expr_vector_t>(expression->value);
			return execute_vector(vector.elements);
		}
		case internal::ast::expression_kind_e::call:
		{
			auto call = std::get<internal::ast::expression_t::expr_call_t>(expression->value);
			return execute_call(call.name, call.args);
		}
		case internal::ast::expression_kind_e::slice:
		{
			auto slice = std::get<internal::ast::expression_t::expr_slice_t>(expression->value);
			return execute_slice(slice.vector, slice.start, slice.stop);
		}
		}

		return false;
	}

	template<typename T>
	auto expression_t<T>::execute_bool_op(internal::ast::bool_op_type_e op, const internal::ast::expr_seq_ptr_t& values) -> bool
	{
		if (values == nullptr || values->elements.size() == 0)
		{
			return false;
		}

		std::vector<T> expr_values;
		for (const auto& expr : values->elements)
		{
			if (!execute_expression(expr))
			{
				return false;
			}

			const auto stack_value = m_stack.top();
			m_stack.pop();
			T value = stack_value.type == internal::stack_object_type_e::scalar ? std::get<T>(stack_value.value) : T(0);
			if (stack_value.type == internal::stack_object_type_e::vector)
			{
				const auto vector = std::get<std::vector<T>>(stack_value.value);
				if (vector.size() > 0)
				{
					value = vector[0];
				}
			}

			expr_values.push_back(value);
		}

		T prev_value = expr_values[0];
		for (size_t i = 1; i < expr_values.size(); i++)
		{
			switch (op)
			{
			case internal::ast::bool_op_type_e::And: prev_value = prev_value and expr_values[i]; break;
			case internal::ast::bool_op_type_e::Or: prev_value = prev_value or expr_values[i]; break;
			}
		}
		m_stack.push(internal::stack_object_t<T>(prev_value));
		return true;
	}

	template<typename T>
	auto expression_t<T>::execute_bin_op(const internal::ast::expr_ptr_t& left, internal::ast::operator_type_e op, const internal::ast::expr_ptr_t& right) -> bool
	{
		if ((left == nullptr || right == nullptr) || (!execute_expression(left) || !execute_expression(right)))
		{
			return false;
		}

		const auto right_value = m_stack.top();
		m_stack.pop();
		const auto left_value = m_stack.top();
		m_stack.pop();

		switch (op)
		{
		case internal::ast::operator_type_e::add: m_stack.push(left_value + right_value); return true;
		case internal::ast::operator_type_e::sub: m_stack.push(left_value - right_value); return true;
		case internal::ast::operator_type_e::mult: m_stack.push(left_value * right_value); return true;
		case internal::ast::operator_type_e::div: m_stack.push(left_value / right_value); return true;
		case internal::ast::operator_type_e::mod: m_stack.push(internal::fmod(left_value, right_value)); return true;
		case internal::ast::operator_type_e::pow: m_stack.push(internal::pow(left_value, right_value)); return true;
		}

		return false;
	}

	template<typename T>
	auto expression_t<T>::execute_unary_op(internal::ast::unary_op_type_e op, const internal::ast::expr_ptr_t& right) -> bool
	{
		if (right == nullptr || !execute_expression(right))
		{
			return false;
		}

		const auto right_value = m_stack.top();
		m_stack.pop();

		if (right_value.type != internal::stack_object_type_e::scalar)
		{
			return false;
		}

		T value = right_value.type == internal::stack_object_type_e::scalar ? std::get<T>(right_value.value) : T(0);
		if (right_value.type == internal::stack_object_type_e::vector)
		{
			const auto vector = std::get<std::vector<T>>(right_value.value);
			if (vector.size() > 0)
			{
				value = vector[0];
			}
		}

		switch (op)
		{
		case internal::ast::unary_op_type_e::invert: m_stack.push(internal::stack_object_t<T>(static_cast<T>(~static_cast<uint64_t>(value)))); return true;
		case internal::ast::unary_op_type_e::Not: m_stack.push(internal::stack_object_t<T>(!value)); return true;
		case internal::ast::unary_op_type_e::add: m_stack.push(internal::stack_object_t<T>(+value)); return true;
		case internal::ast::unary_op_type_e::sub: m_stack.push(internal::stack_object_t<T>(-value)); return true;
		}

		return false;
	}

	template<typename T>
	inline auto expression_t<T>::execute_cmp_op(const internal::ast::expr_ptr_t& left, internal::ast::cmp_op_type_e op, const internal::ast::expr_ptr_t& right) -> bool
	{
		if ((left == nullptr || right == nullptr) || (!execute_expression(left) || !execute_expression(right)))
		{
			return false;
		}

		const auto right_value = m_stack.top();
		m_stack.pop();
		const auto left_value = m_stack.top();
		m_stack.pop();

		internal::stack_object_t<T> value = internal::stack_object_t<T>(T(0));
		switch (op)
		{
		case internal::ast::cmp_op_type_e::eq: value = left_value == right_value; break;
		case internal::ast::cmp_op_type_e::Not_eq: value = left_value != right_value; break;
		case internal::ast::cmp_op_type_e::lt: value = left_value < right_value; break;
		case internal::ast::cmp_op_type_e::lt_eq: value = left_value <= right_value; break;
		case internal::ast::cmp_op_type_e::gt: value = left_value > right_value; break;
		case internal::ast::cmp_op_type_e::gt_eq: value = left_value >= right_value; break;
		case internal::ast::cmp_op_type_e::in: value = internal::in(left_value, right_value); break;
		case internal::ast::cmp_op_type_e::not_in: value = internal::not_in(left_value, right_value); break;
		}

		m_stack.push(value);
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
		m_stack.push(internal::stack_object_t<T>(to_number<T>(value)));
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

			m_stack.push(internal::stack_object_t<T>(m_symbol_table[id]));
			return true;
		}
		case internal::ast::expr_context_type_e::store:
		{
			const auto value = m_stack.top();

			T var_value = value.type == internal::stack_object_type_e::scalar ? std::get<T>(value.value) : T(0);
			if (value.type == internal::stack_object_type_e::vector)
			{
				const auto vector = std::get<std::vector<T>>(value.value);
				if (vector.size() > 0)
				{
					var_value = vector[0];
				}
			}

			if (m_symbol_table.has(id))
			{
				m_symbol_table[id] = var_value;
			}
			m_symbol_table.add_variable(id, var_value);
			return true;
		}
		case internal::ast::expr_context_type_e::del:
		{
			return false;
		}

		return false;
		}
	}

	template<typename T>
	auto expression_t<T>::execute_vector(const internal::ast::expr_seq_ptr_t& elements) -> bool
	{
		if (elements == nullptr)
		{
			return false;
		}

		std::vector<T> vector_elements;
		for (const auto& value : elements->elements)
		{
			if (!execute_expression(value))
			{
				return false;
			}
			const auto element = m_stack.top();
			m_stack.pop();
			T value = element.type == internal::stack_object_type_e::scalar ? std::get<T>(element.value) : T(0);
			if (element.type == internal::stack_object_type_e::vector)
			{
				const auto vector = std::get<std::vector<T>>(element.value);
				if (vector.size() > 0)
				{
					value = vector[0];
				}
			}

			vector_elements.push_back(value);
		}

		m_stack.push(internal::stack_object_t<T>(vector_elements));
		return true;
	}

	template<typename T>
	auto expression_t<T>::execute_call(const std::string& name, const internal::ast::expr_seq_ptr_t& args) -> bool
	{
		if (!m_symbol_table.has_function(name))
		{
			return false;
		}

		auto func = m_symbol_table.get_function(name);
		if (args == nullptr)
		{
			T value = (*func)();
			m_stack.push(internal::stack_object_t<T>(value));
			return true;
		}
		else if (args->elements.size() != func->num_args())
		{
			return false;
		}

		std::vector<T> arg_values;
		for (const auto& expr : args->elements)
		{
			if (!execute_expression(expr))
			{
				return false;
			}

			const auto stack_value = m_stack.top();
			m_stack.pop();
			T value = stack_value.type == internal::stack_object_type_e::scalar ? std::get<T>(stack_value.value) : T(0);
			if (stack_value.type == internal::stack_object_type_e::vector)
			{
				const auto vector = std::get<std::vector<T>>(stack_value.value);
				if (vector.size() > 0)
				{
					value = vector[0];
				}
			}
			arg_values.push_back(value);
		}

		T value = T(0);
		switch (args->elements.size())
		{
		case 0: value = (*func)(); break;
		case 1: value = (*func)(arg_values[0]); break;
		case 2: value = (*func)(arg_values[0], arg_values[1]); break;
		case 3: value = (*func)(arg_values[0], arg_values[1], arg_values[2]); break;
		case 4: value = (*func)(arg_values[0], arg_values[1], arg_values[2], arg_values[3]); break;
		default: return false;
		}
		
		m_stack.push(internal::stack_object_t<T>(value));
		return true;
	}

	template<typename T>
	auto expression_t<T>::execute_slice(const internal::ast::expr_ptr_t& vector, const internal::ast::expr_ptr_t& start, const internal::ast::expr_ptr_t& stop) -> bool
	{
		if (vector == nullptr || !execute_expression(vector))
		{
			return false;
		}

		const auto stack_vector = m_stack.top();
		m_stack.pop();
		if (stack_vector.type != internal::stack_object_type_e::vector)
		{
			return false;
		}
		const auto vector_value = std::get<std::vector<T>>(stack_vector.value);
		int start_pos = 0;
		int end_pos = vector_value.size();

		if (start != nullptr && execute_expression(start))
		{
			const auto stack_start = m_stack.top();
			m_stack.pop();
			if (stack_start.type != internal::stack_object_type_e::scalar)
			{
				return false;
			}
			start_pos = std::get<T>(stack_start.value);
			if (start_pos < 0)
			{
				start_pos = vector_value.size() + start_pos;
			}
		}

		if (stop != nullptr && execute_expression(stop))
		{
			const auto stack_stop = m_stack.top();
			m_stack.pop();
			if (stack_stop.type != internal::stack_object_type_e::scalar)
			{
				return false;
			}
			end_pos = std::get<T>(stack_stop.value);
			if (end_pos < 0)
			{
				end_pos = vector_value.size() + end_pos;
			}
		}

		if (start_pos >= end_pos)
		{
			return false;
		}

		size_t size = static_cast<size_t>(end_pos - start_pos);
		if (size == 1)
		{
			T value = vector_value[start_pos];
			m_stack.push(internal::stack_object_t<T>(value));
			return true;
		}
		std::vector<T> values(size);
		size_t i = 0;
		for (size_t pos = start_pos; pos < end_pos; pos++)
		{
			values[i++] = vector_value[pos];
		}
		m_stack.push(internal::stack_object_t<T>(values));
		return true;
	}

}