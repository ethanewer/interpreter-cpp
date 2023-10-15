#include "Resolver.hpp"

Resolver::Resolver(Interpreter* interpreter) : interpreter(interpreter), curr_fn(NONE) {}

std::shared_ptr<Obj> Resolver::visit_literal_expr(Literal* expr) {
	return nullptr;
}

std::shared_ptr<Obj> Resolver::visit_grouping_expr(Grouping* expr) {
	resolve(expr->expression);
	return nullptr;
}

std::shared_ptr<Obj> Resolver::visit_unary_expr(Unary* expr) {
	resolve(expr->right);
	return nullptr;
}

std::shared_ptr<Obj> Resolver::visit_binary_expr(Binary* expr) {
	resolve(expr->left);
	resolve(expr->right);
	return nullptr;
}

std::shared_ptr<Obj> Resolver::visit_variable_expr(Variable* expr) {
	if (!scopes.empty()) {
		if (scopes.back().count(expr->name->lexeme) && scopes.back()[expr->name->lexeme] == false) {
			std::cout << expr->name->lexeme << " was declared, but not defined\n";
			throw RuntimeError(expr->name, "Can't read local variable in its own initializer");
		}
	}
	resolve_local(expr, expr->name);
	return nullptr;
}

std::shared_ptr<Obj> Resolver::visit_assign_expr(Assign* expr) {
	resolve(expr->val);
	resolve_local(expr, expr->name);
	return nullptr;
}

std::shared_ptr<Obj> Resolver::visit_logical_expr(Logical* expr) {
	resolve(expr->left);
	resolve(expr->right);
	return nullptr;
}

std::shared_ptr<Obj> Resolver::visit_call_expr(Call* expr) {
	resolve(expr->callee);
	for (auto a : expr->arguments) resolve(a);
	return nullptr;
}

std::shared_ptr<Obj> Resolver::visit_lambda_expr(LambdaExpr* expr) {
	resolve_lambda_expr(expr, FUNCTION);
	return nullptr;
}

void Resolver::visit_expression_stmt(Expression* stmt) {
	resolve(stmt->expression);
}

void Resolver::visit_print_stmt(Print* stmt) {
	resolve(stmt->expression);
}

void Resolver::visit_var_stmt(Var* stmt) {
	declare(stmt->name);
	if (stmt->initializer != nullptr) resolve(stmt->initializer);
	define(stmt->name);
}

void Resolver::visit_block_stmt(Block* stmt) {
	begin_scope();
	resolve(stmt->stmts);
	end_scope();
}

void Resolver::visit_if_stmt(If* stmt) {
	resolve(stmt->condition);
	resolve(stmt->then_branch);
	if (stmt->else_branch != nullptr) resolve(stmt->else_branch);
}

void Resolver::visit_while_stmt(While* stmt) {
	resolve(stmt->condition);
	resolve(stmt->body);
}

void Resolver::visit_fn_stmt(FnStmt* stmt) {
	declare(stmt->name);
	define(stmt->name);
	resolve_fn_stmt(stmt, FUNCTION);
}

void Resolver::visit_return_stmt(Return* stmt) {
	if (curr_fn == NONE) throw RuntimeError(stmt->keyword, "Can't return from top level");
	if (stmt->val != nullptr) resolve(stmt->val); 
}

void Resolver::resolve(std::vector<std::shared_ptr<Stmt>>& stmts) {
	for (auto s : stmts) resolve(s);
} 

void Resolver::resolve(std::shared_ptr<Stmt> stmt) {
	stmt->accept(this);
}

void Resolver::resolve(std::shared_ptr<Expr> expr) {
	expr->accept(this);
}

void Resolver::begin_scope() {
	scopes.push_back(std::unordered_map<std::string, bool>());
}

void Resolver::end_scope() {
	scopes.pop_back();
}

void Resolver::declare(std::shared_ptr<Token> name) {
	if (!scopes.empty() && scopes.back().count(name->lexeme)) {
		throw RuntimeError(name, "A variable with this name already exists in this scope");
	}
	if (!scopes.empty()) scopes.back()[name->lexeme] = false;
}

void Resolver::define(std::shared_ptr<Token> name) {
	if (!scopes.empty()) scopes.back()[name->lexeme] = true;
}

void Resolver::resolve_local(Expr* expr, std::shared_ptr<Token> name) {
	for (int i = scopes.size() - 1; i >= 0; i--) {
		if (scopes[i].count(name->lexeme)) {
			interpreter->resolve(expr, scopes.size() - 1 - i);
			return;
		}
	}
}

void Resolver::resolve_fn_stmt(FnStmt* fn, FnType type) {
	FnType enclosing_fn = curr_fn;
	curr_fn = type;
	begin_scope();
	for (auto p : fn->params) {
		declare(p);
		define(p);
	}
	resolve(fn->body);
	end_scope();
	curr_fn = enclosing_fn;
}

void Resolver::resolve_lambda_expr(LambdaExpr * lambda, FnType type) {
	FnType enclosing_fn = curr_fn;
	curr_fn = type;
	begin_scope();
	for (auto p : lambda->params) {
		declare(p);
		define(p);
	}
	resolve(lambda->body);
	end_scope();
	curr_fn = enclosing_fn;
}
