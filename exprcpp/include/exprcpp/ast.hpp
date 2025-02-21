#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace exprcpp::internal::ast
{
	
	struct node_t {};
	struct statement_t;
	struct expression_t;

	typedef std::shared_ptr<node_t> node_ptr_t;
	typedef std::shared_ptr<statement_t> stmt_ptr_t;
	typedef std::shared_ptr<expression_t> expr_ptr_t;

	template<typename T>
	struct sequence_t
	{
		std::vector<T> elements;
	};

	typedef sequence_t<node_ptr_t> generic_seq_t;
	typedef sequence_t<stmt_ptr_t> stmt_seq_t;
	typedef sequence_t<expr_ptr_t> expr_seq_t;

	typedef std::shared_ptr<generic_seq_t> generic_seq_ptr_t;
	typedef std::shared_ptr<stmt_seq_t> stmt_seq_ptr_t;
	typedef std::shared_ptr<expr_seq_t> expr_seq_ptr_t;

	enum class statement_kind_e
	{
		if_else, expr
	};

	struct statement_t : node_t
	{
		struct stmt_if_else_t
		{
			expr_ptr_t condition;
			expr_ptr_t true_case;
			expr_ptr_t false_case;
		};

		struct stmt_expr_t
		{
			expr_ptr_t value;
		};

		statement_kind_e kind;
		std::variant<stmt_if_else_t, stmt_expr_t> value;
	};

	enum class bool_op_type_e { And, Or };

	enum class operator_type_e 
	{
		add,
		sub,
		mult,
		div,
		mod,
		pow
	};

	enum class unary_op_type_e
	{
		invert,
		Not,
		add,
		sub
	};

	enum class cmp_op_type_e
	{
		eq,
		Not_eq,
		lt,
		lt_eq,
		gt,
		gt_eq,
		in,
		not_in
	};

	enum class expr_context_type_e
	{
		load, store, del
	};

	enum class expression_kind_e
	{
		bool_op,
		bin_op,
		unary_op,
		cmp_op,
		assign,
		constant,
		name,
		vector
	};

	struct expression_t : node_t
	{
		struct expr_bool_op_t
		{
			bool_op_type_e op;
			expr_seq_ptr_t values;
		};

		struct expr_bin_op_t
		{
			expr_ptr_t left;
			operator_type_e op;
			expr_ptr_t right;
		};

		struct expr_unary_op_t
		{
			unary_op_type_e op;
			expr_ptr_t right;
		};

		struct expr_cmp_op_t
		{
			expr_ptr_t left;
			cmp_op_type_e op;
			expr_ptr_t right;
		};

		struct expr_assign_t
		{
			std::string id;
			expr_ptr_t value;
		};

		struct expr_constant_t
		{
			std::string value;
		};

		struct expr_name_t
		{
			std::string id;
			expr_context_type_e context;
		};

		struct expr_vector_t
		{
			expr_seq_ptr_t elements;
		};

		expression_kind_e kind;
		std::variant<expr_bool_op_t, expr_bin_op_t, expr_unary_op_t, expr_cmp_op_t, expr_assign_t, 
					 expr_constant_t, expr_name_t, expr_vector_t> value;
	};

	auto if_else(const expr_ptr_t& condition, const expr_ptr_t& true_case, const expr_ptr_t& false_case) -> stmt_ptr_t;
	auto expression(const expr_ptr_t& expr) -> stmt_ptr_t;
	auto bool_op(bool_op_type_e op, const expr_seq_ptr_t& values) -> expr_ptr_t;
	auto bin_op(const expr_ptr_t& left, operator_type_e op, const expr_ptr_t& right) -> expr_ptr_t;
	auto unary_op(unary_op_type_e op, const expr_ptr_t& right) -> expr_ptr_t;
	auto cmp_op(const expr_ptr_t& left, cmp_op_type_e op, const expr_ptr_t& right) -> expr_ptr_t;
	auto assign(const std::string& id, const expr_ptr_t& value) -> expr_ptr_t;
	auto constant(const std::string& value) -> expr_ptr_t;
	auto name(const std::string& id, expr_context_type_e context) -> expr_ptr_t;
	auto vector(const expr_seq_ptr_t& elements) -> expr_ptr_t;
}