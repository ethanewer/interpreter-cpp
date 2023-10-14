#ifndef INTERPRETER
#define INTERPRETER

#include <vector>
#include <iostream>
#include <typeinfo>
#include <string>
#include <algorithm>
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
	Environment* globals;
	
	Interpreter();

	void interpret(std::vector<Stmt*> stmts);

	void execute_block(std::vector<Stmt*>& stmts, Environment* env);

	Obj* visit_literal_expr(Literal* expr) override;

	Obj* visit_grouping_expr(Grouping* expr) override;

	Obj* visit_unary_expr(Unary* expr) override;

	Obj* visit_binary_expr(Binary* expr) override;

	Obj* visit_variable_expr(Variable* expr) override;

	Obj* visit_assign_expr(Assign* expr) override;

	Obj* visit_logical_expr(Logical* expr) override;

	Obj* visit_call_expr(Call* expr) override;

	Obj* visit_lambda_expr(LambdaExpr* expr) override;

	void visit_expression_stmt(Expression* stmt) override;

	void visit_print_stmt(Print* stmt) override;

	void visit_var_stmt(Var* stmt) override;

	void visit_block_stmt(Block* stmt) override;

	void visit_if_stmt(If* stmt) override;

	void visit_while_stmt(While* stmt) override;

	void visit_fn_stmt(FnStmt* stmt) override;

	void visit_return_stmt(Return* stmt) override;

	~Interpreter();

private:
	Environment* env;

	Obj* evaluate(Expr* expr);

	void execute(Stmt* stmt);

	bool is_truthy(Obj* val);

	bool is_equal(Obj* a, Obj* b);

	void check_num_operand(Token* op, Obj* operand);

	void check_num_operands(Token* op, Obj* a, Obj* b);

	std::string stringify(Obj* val);
};

#endif