#ifndef RESOLVER
#define RESOLVER

#include <vector>
#include <unordered_map>
#include <iostream>
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Interpreter.hpp"

enum FnType { FnType_NONE, FnType_FN, FnType_METHOD, FnType_INIT };

enum ClassType { ClassType_NONE, ClassType_CLASS };

class Resolver : Expr::Visitor, Stmt::Visitor {
public:
	Resolver(Interpreter* interpreter);

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

	void resolve(std::vector<std::shared_ptr<Stmt>>& stmts);

private:
	Interpreter* interpreter;
	std::vector<std::unordered_map<std::string, bool>> scopes;
	FnType curr_fn;
	ClassType curr_class;

	void resolve(std::shared_ptr<Stmt> stmt);

	void resolve(std::shared_ptr<Expr> expr);

	void begin_scope();

	void end_scope();

	void declare(std::shared_ptr<Token> name);

	void define(std::shared_ptr<Token> name);

	void resolve_local(Expr* expr, std::shared_ptr<Token> name);

	void resolve_fn_stmt(FnStmt* fn, FnType type);
	
	void resolve_lambda_expr(LambdaExpr* fn, FnType type);
};

#endif