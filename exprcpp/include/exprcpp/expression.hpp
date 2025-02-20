#pragma once

#include "exprcpp/symbol_table.hpp"
#include "exprcpp/ast.hpp"
#include <stack>

namespace exprcpp
{

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
		auto execute_expression(const internal::ast::expr_ptr_t& expression) -> bool;
		auto execute_bool_op(internal::ast::bool_op_type_t op, const internal::ast::expr_seq_ptr_t& values) -> bool;
		auto execute_bin_op(const internal::ast::expr_ptr_t& left, internal::ast::operator_type_t op, const internal::ast::expr_ptr_t& right) -> bool;
		auto execute_unary_op(internal::ast::unary_op_type_t op, const internal::ast::expr_ptr_t& right) -> bool;
		auto execute_cmp_op(const internal::ast::expr_ptr_t& left, internal::ast::cmp_op_type_t op, const internal::ast::expr_ptr_t& right) -> bool;
		auto execute_assign(const std::string& id, const internal::ast::expr_ptr_t& value) -> bool;
		auto execute_constant(const std::string& value) -> bool;
		auto execute_name(const std::string& id, internal::ast::expr_context_type_e context) -> bool;
	private:
		symbol_table_t<T> m_symbol_table;
		internal::ast::stmt_seq_ptr_t m_ast;

		std::stack<T> m_stack;
	};

}

#include "expression.inl"