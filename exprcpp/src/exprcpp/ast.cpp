#include "exprcpp/ast.hpp"

namespace exprcpp::internal::ast
{
	auto if_else(const expr_ptr_t& condition, const expr_ptr_t& true_case, const expr_ptr_t& false_case) -> stmt_ptr_t
	{
		statement_t::stmt_if_else_t if_else;
		if_else.condition = condition;
		if_else.true_case = true_case;
		if_else.false_case = false_case;

		auto stmt = std::make_shared<statement_t>();
		stmt->kind = statement_kind_e::if_else;
		stmt->value = if_else;
		return stmt;
	}

	auto expression(const expr_ptr_t& value) -> stmt_ptr_t
	{
		statement_t::stmt_expr_t expr;
		expr.value = value;

		auto stmt = std::make_shared<statement_t>();
		stmt->kind = statement_kind_e::expr;
		stmt->value = expr;
		return stmt;
	}

	auto bool_op(bool_op_type_e op, const expr_seq_ptr_t& values) -> expr_ptr_t
	{
		expression_t::expr_bool_op_t bool_op;
		bool_op.op = op;
		bool_op.values = values;

		auto expr = std::make_shared<expression_t>();
		expr->kind = expression_kind_e::bool_op;
		expr->value = bool_op;
		return expr;
	}

	auto bin_op(const expr_ptr_t& left, operator_type_e op, const expr_ptr_t& right) -> expr_ptr_t
	{
		expression_t::expr_bin_op_t bin_op;
		bin_op.left = left;
		bin_op.op = op;
		bin_op.right = right;

		auto expr = std::make_shared<expression_t>();
		expr->kind = expression_kind_e::bin_op;
		expr->value = bin_op;
		return expr;
	}

	auto unary_op(unary_op_type_e op, const expr_ptr_t& right) -> expr_ptr_t
	{
		expression_t::expr_unary_op_t unary_op;
		unary_op.op = op;
		unary_op.right = right;

		auto expr = std::make_shared<expression_t>();
		expr->kind = expression_kind_e::unary_op;
		expr->value = unary_op;
		return expr;
	}

	auto cmp_op(const expr_ptr_t& left, cmp_op_type_e op, const expr_ptr_t& right) -> expr_ptr_t
	{
		expression_t::expr_cmp_op_t cmp_op;
		cmp_op.left = left;
		cmp_op.op = op;
		cmp_op.right = right;

		auto expr = std::make_shared<expression_t>();
		expr->kind = expression_kind_e::cmp_op;
		expr->value = cmp_op;
		return expr;
	}

	auto assign(const std::string& id, const expr_ptr_t& value) -> expr_ptr_t
	{
		expression_t::expr_assign_t assign;
		assign.id = id;
		assign.value = value;

		auto expr = std::make_shared<expression_t>();
		expr->kind = expression_kind_e::assign;
		expr->value = assign;
		return expr;
	}

	auto constant(const std::string& value) -> expr_ptr_t
	{
		expression_t::expr_constant_t constant;
		constant.value = value;

		auto expr = std::make_shared<expression_t>();
		expr->kind = expression_kind_e::constant;
		expr->value = constant;
		return expr;
	}

	auto name(const std::string& id, expr_context_type_e context) -> expr_ptr_t
	{
		expression_t::expr_name_t name;
		name.id = id;
		name.context = context;

		auto expr = std::make_shared<expression_t>();
		expr->kind = expression_kind_e::name;
		expr->value = name;
		return expr;
	}

	auto vector(const expr_seq_ptr_t& elements) -> expr_ptr_t
	{
		expression_t::expr_vector_t vector;
		vector.elements = elements;

		auto expr = std::make_shared<expression_t>();
		expr->kind = expression_kind_e::vector;
		expr->value = vector;
		return expr;
	}

}