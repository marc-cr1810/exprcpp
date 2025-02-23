#pragma once

#include "exprcpp/symbol_table.hpp"
#include "exprcpp/ast.hpp"
#include <stack>

namespace exprcpp
{

	namespace internal
	{
		enum class stack_object_type_e
		{
			scalar, vector
		};

		template<typename T>
		struct stack_object_t
		{
			typedef T scalar_t;
			typedef std::vector<T> vector_t;

			stack_object_type_e type;
			std::variant<T, vector_t> value;

			explicit stack_object_t(T scalar);
			explicit stack_object_t(vector_t vector);
		};

		template<typename T>
		constexpr auto operator+(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
		template<typename T>
		constexpr auto operator-(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
		template<typename T>
		constexpr auto operator*(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
		template<typename T>
		constexpr auto operator/(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;

		template<typename T>
		constexpr auto pow(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
		template<typename T>
		constexpr auto fmod(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;

		template<typename T>
		constexpr auto operator==(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
		template<typename T>
		constexpr auto operator!=(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;

		template<typename T>
		constexpr auto operator<(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
		template<typename T>
		constexpr auto operator<=(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
		template<typename T>
		constexpr auto operator>(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
		template<typename T>
		constexpr auto operator>=(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;

		template<typename T>
		constexpr auto in(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
		template<typename T>
		constexpr auto not_in(const stack_object_t<T>& lhs, const stack_object_t<T>& rhs) -> stack_object_t<T>;
	}

	template<typename T>
	class expression_t
	{
	public:
		expression_t() = default;
		~expression_t() = default;

		auto value() -> T;

		auto register_symbol_table(const symbol_table_t<T> symbol_table) -> void;
		auto set_ast(const internal::ast::stmt_seq_ptr_t& ast) -> void;
	private:
		auto execute_statement(const internal::ast::stmt_ptr_t& statement) -> bool;
		auto execute_if_else(const internal::ast::expr_ptr_t& condition, const internal::ast::expr_ptr_t& true_case, const internal::ast::expr_ptr_t& false_case) -> bool;
		auto execute_expression(const internal::ast::expr_ptr_t& expression) -> bool;
		auto execute_bool_op(internal::ast::bool_op_type_e op, const internal::ast::expr_seq_ptr_t& values) -> bool;
		auto execute_bin_op(const internal::ast::expr_ptr_t& left, internal::ast::operator_type_e op, const internal::ast::expr_ptr_t& right) -> bool;
		auto execute_unary_op(internal::ast::unary_op_type_e op, const internal::ast::expr_ptr_t& right) -> bool;
		auto execute_cmp_op(const internal::ast::expr_ptr_t& left, internal::ast::cmp_op_type_e op, const internal::ast::expr_ptr_t& right) -> bool;
		auto execute_assign(const std::string& id, const internal::ast::expr_ptr_t& value) -> bool;
		auto execute_constant(const std::string& value) -> bool;
		auto execute_name(const std::string& id, internal::ast::expr_context_type_e context) -> bool;
		auto execute_vector(const internal::ast::expr_seq_ptr_t& elements) -> bool;
		auto execute_call(const std::string& name, const internal::ast::expr_seq_ptr_t& args) -> bool;
		auto execute_slice(const internal::ast::expr_ptr_t& vector, const internal::ast::expr_ptr_t& start, const internal::ast::expr_ptr_t& stop) -> bool;
	private:
		symbol_table_t<T> m_symbol_table;
		internal::ast::stmt_seq_ptr_t m_ast;

		std::stack<internal::stack_object_t<T>> m_stack;
	};

}

#include "expression.inl"