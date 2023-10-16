#ifndef INTERPRETER
#define INTERPRETER

#include <vector>
#include <iostream>
#include <typeinfo>
#include <string>
#include <algorithm>
#include <math.h>
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Error.hpp"
#include "Token.hpp"
#include "Obj.hpp"
#include "Environment.hpp"
#include "Callable.hpp"
#include "ReturnException.hpp"

class Interpreter : Expr::Visitor, Stmt::Visitor {
public:
	std::shared_ptr<Environment> globals;
	
	Interpreter();

	void interpret(std::vector<std::shared_ptr<Stmt>> stmts);

	void execute_block(std::vector<std::shared_ptr<Stmt>>& stmts, std::shared_ptr<Environment> env);

	std::shared_ptr<Obj> visit_literal_expr(Literal* expr) override;

	std::shared_ptr<Obj> visit_grouping_expr(Grouping* expr) override;

	std::shared_ptr<Obj> visit_unary_expr(Unary* expr) override;

	std::shared_ptr<Obj> visit_binary_expr(Binary* expr) override;

	std::shared_ptr<Obj> visit_variable_expr(Variable* expr) override;

	std::shared_ptr<Obj> visit_assign_expr(Assign* expr) override;

	std::shared_ptr<Obj> visit_logical_expr(Logical* expr) override;

	std::shared_ptr<Obj> visit_call_expr(Call* expr) override;

	std::shared_ptr<Obj> visit_lambda_expr(LambdaExpr* expr) override;

	std::shared_ptr<Obj> visit_get_expr(Get* expr) override;
	
	std::shared_ptr<Obj> visit_set_expr(Set* expr) override;
	
	std::shared_ptr<Obj> visit_this_expr(This* expr) override;

	void visit_expression_stmt(Expression* stmt) override;

	void visit_print_stmt(Print* stmt) override;

	void visit_var_stmt(Var* stmt) override;

	void visit_block_stmt(Block* stmt) override;

	void visit_if_stmt(If* stmt) override;

	void visit_while_stmt(While* stmt) override;

	void visit_fn_stmt(FnStmt* stmt) override;

	void visit_return_stmt(Return* stmt) override;
	
	void visit_class_stmt(ClassStmt* stmt) override;

	void resolve(Expr* expr, int depth);

	bool is_truthy(std::shared_ptr<Obj> val);

private:
	std::shared_ptr<Environment> env;
	std::unordered_map<Expr*, int> locals;

	std::shared_ptr<Obj> evaluate(std::shared_ptr<Expr> expr);

	void execute(std::shared_ptr<Stmt> stmt);

	bool is_equal(std::shared_ptr<Obj> a, std::shared_ptr<Obj> b);

	void check_num_operand(std::shared_ptr<Token> op, std::shared_ptr<Obj> operand);

	void check_num_operands(std::shared_ptr<Token> op, std::shared_ptr<Obj> a, std::shared_ptr<Obj> b);

	std::string stringify(std::shared_ptr<Obj> val);
};

#endif