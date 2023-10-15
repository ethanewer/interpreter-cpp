#include "Interpreter.hpp"

Interpreter::Interpreter() {
	globals = std::make_shared<Environment>();
	env = globals;
	globals->define("clock", std::make_shared<Clock>());
}

void Interpreter::interpret(std::vector<std::shared_ptr<Stmt>> stmts) {
	for (auto stmt : stmts) execute(stmt);
}

void Interpreter::execute_block(std::vector<std::shared_ptr<Stmt>>& stmts, std::shared_ptr<Environment> env) {
	std::shared_ptr<Environment> prev_env = this->env;
	try {
		this->env = env;
		for (std::shared_ptr<Stmt> s : stmts) execute(s);
		this->env = prev_env;
	} catch (ReturnException& e) {
		this->env = prev_env;
		throw e;
	} catch (RuntimeError& e) {
		this->env = prev_env;
		throw e;
	}
};

std::shared_ptr<Obj> Interpreter::visit_literal_expr(Literal* expr) {
	return expr->val;
}

std::shared_ptr<Obj> Interpreter::visit_grouping_expr(Grouping* expr) {
	return evaluate(expr->expression);
}

std::shared_ptr<Obj> Interpreter::visit_unary_expr(Unary* expr) {
	std::shared_ptr<Obj> right = evaluate(expr->right);
	std::shared_ptr<DoubleObj> double_val;
	switch (expr->op->type) {
		case BANG: 
			return std::make_shared<BoolObj>(!is_truthy(right));
		case MINUS:
			check_num_operand(expr->op, right);
			double_val = std::dynamic_pointer_cast<DoubleObj>(right);
			return std::make_shared<DoubleObj>(-double_val->val);
	}
	return nullptr;
}

std::shared_ptr<Obj> Interpreter::visit_binary_expr(Binary* expr) {
	std::shared_ptr<Obj> left = evaluate(expr->left);
	std::shared_ptr<Obj> right = evaluate(expr->right); 

	if (left == nullptr || right == nullptr) throw RuntimeError(expr->op, "nil can not be added");

	std::shared_ptr<DoubleObj> double_left;
	std::shared_ptr<DoubleObj> double_right;
	std::shared_ptr<StringObj> string_left;
	std::shared_ptr<StringObj> string_right;
	std::shared_ptr<BoolObj> bool_right;

	switch (expr->op->type) {
		case BANG_EQUAL: 
			return std::make_shared<BoolObj>(!is_equal(left, right));
		case EQUAL_EQUAL: 
			return std::make_shared<BoolObj>(is_equal(left, right));
		case GREATER:
			check_num_operands(expr->op, left, right);
			double_left = std::dynamic_pointer_cast<DoubleObj>(left);
			double_right = std::dynamic_pointer_cast<DoubleObj>(right);
			return std::make_shared<BoolObj>(double_left->val > double_right->val);
		case GREATER_EQUAL:
			check_num_operands(expr->op, left, right);
			double_left = std::dynamic_pointer_cast<DoubleObj>(left);
			double_right = std::dynamic_pointer_cast<DoubleObj>(right);
			return std::make_shared<BoolObj>(double_left->val >= double_right->val);
		case LESS:
			check_num_operands(expr->op, left, right);
			double_left = std::dynamic_pointer_cast<DoubleObj>(left);
			double_right = std::dynamic_pointer_cast<DoubleObj>(right);
			return std::make_shared<BoolObj>(double_left->val < double_right->val);
		case LESS_EQUAL:
			check_num_operands(expr->op, left, right);
			double_left = std::dynamic_pointer_cast<DoubleObj>(left);
			double_right = std::dynamic_pointer_cast<DoubleObj>(right);
			return std::make_shared<BoolObj>(double_left->val <= double_right->val);
		case PLUS:
			if ((double_left = std::dynamic_pointer_cast<DoubleObj>(left))) {
				if ((double_right = std::dynamic_pointer_cast<DoubleObj>(right))) {
					return std::make_shared<DoubleObj>(double_left->val + double_right->val);
				}
			} 
			if ((string_left = std::dynamic_pointer_cast<StringObj>(left))) {
				if ((string_right = std::dynamic_pointer_cast<StringObj>(right))) {
					return std::make_shared<StringObj>(string_left->val + string_right->val);
				}
				if ((double_right = std::dynamic_pointer_cast<DoubleObj>(right))) {
					return std::make_shared<StringObj>(string_left->val + stringify(double_right));
				}
				if ((bool_right = std::dynamic_pointer_cast<BoolObj>(right))) {
					return std::make_shared<StringObj>(string_left->val + stringify(bool_right));
				}
			} 
			throw  RuntimeError(expr->op, "Operands can not be added with '+'");
		case MINUS:
			check_num_operands(expr->op, left, right);
			double_left = std::dynamic_pointer_cast<DoubleObj>(left);
			double_right = std::dynamic_pointer_cast<DoubleObj>(right);
			return std::make_shared<DoubleObj>(double_left->val - double_right->val);
		case SLASH:
			check_num_operands(expr->op, left, right);
			double_left = std::dynamic_pointer_cast<DoubleObj>(left);
			double_right = std::dynamic_pointer_cast<DoubleObj>(right);
			return std::make_shared<DoubleObj>(double_left->val / double_right->val);
		case STAR:
			check_num_operands(expr->op, left, right);
			double_left = std::dynamic_pointer_cast<DoubleObj>(left);
			double_right = std::dynamic_pointer_cast<DoubleObj>(right);
			return std::make_shared<DoubleObj>(double_left->val * double_right->val);
	}
	return nullptr;
}

std::shared_ptr<Obj> Interpreter::visit_variable_expr(Variable* expr) {
	return env->get(expr->name);
}

std::shared_ptr<Obj> Interpreter::visit_assign_expr(Assign* expr) {
	std::shared_ptr<Obj> val = evaluate(expr->val);
	env->assign(expr->name, val);
	return val;
}

std::shared_ptr<Obj> Interpreter::visit_logical_expr(Logical* expr) {
	std::shared_ptr<Obj> left = evaluate(expr->left);
	if (expr->op->type == OR) {
		if (is_truthy(left)) return left;
	} else {
		if (!is_truthy(left)) return left;
	}
	return evaluate(expr->right);
}

std::shared_ptr<Obj> Interpreter::visit_call_expr(Call* expr) {
	std::shared_ptr<Obj> callee = evaluate(expr->callee);
	std::vector<std::shared_ptr<Obj>> arguments;
	for (auto a : expr->arguments) arguments.push_back(evaluate(a));
	if (auto fn = std::dynamic_pointer_cast<Callable>(callee)) {
		if (arguments.size() != fn->num_params()) throw RuntimeError(expr->paren, "Incorect number of arguments");
		return fn->call(this, arguments);
	} else throw RuntimeError(expr->paren, "Object is not callable");
}

std::shared_ptr<Obj> Interpreter::visit_lambda_expr(LambdaExpr* expr) {
	return std::make_shared<Lambda>(expr->params, expr->body);
}

void Interpreter::visit_expression_stmt(Expression* stmt) {
	evaluate(stmt->expression);
}

void Interpreter::visit_print_stmt(Print* stmt) {
	std::shared_ptr<Obj> val = evaluate(stmt->expression);
	std::cout << stringify(val) << '\n';
}

void Interpreter::visit_var_stmt(Var* stmt) {
	std::shared_ptr<Obj> val = nullptr;
	if (stmt->initializer != nullptr) val = evaluate(stmt->initializer);
	env->define(stmt->name->lexeme, val);
}

void Interpreter::visit_block_stmt(Block* stmt) {
	execute_block(stmt->stmts, std::make_shared<Environment>(env));
}

void Interpreter::visit_if_stmt(If* stmt) {
	if (is_truthy(evaluate(stmt->condition))) execute(stmt->then_branch);
	else if (stmt->else_branch != nullptr) execute(stmt->else_branch);
}

void Interpreter::visit_while_stmt(While* stmt) {
	while (is_truthy(evaluate(stmt->condition))) execute(stmt->body);
}

void Interpreter::visit_fn_stmt(FnStmt* stmt) {
	auto fn = std::make_shared<Fn>(stmt->name, stmt->params, stmt->body, env);
	env->define(stmt->name->lexeme, fn);
}

void Interpreter::visit_return_stmt(Return* stmt) {
	std::shared_ptr<Obj> val = nullptr;
	if (stmt->val != nullptr) val = evaluate(stmt->val);
	throw ReturnException(val);
}

std::shared_ptr<Obj> Interpreter::evaluate(std::shared_ptr<Expr> expr) {
	return expr->accept(this);
}

void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
	stmt->accept(this);
}

bool Interpreter::is_truthy(std::shared_ptr<Obj> val) {
	if (val == nullptr) return false;
	if (auto bool_val = std::dynamic_pointer_cast<BoolObj>(val)) return bool_val->val;
	return true;
}

bool Interpreter::is_equal(std::shared_ptr<Obj> a, std::shared_ptr<Obj> b) {
	if (a == nullptr && b == nullptr) return true;
	if (a == nullptr || b == nullptr) return false;
	if (auto bool_a = std::dynamic_pointer_cast<BoolObj>(a)) {
		if (auto bool_b = std::dynamic_pointer_cast<BoolObj>(b)) {
			return bool_a->val == bool_b->val;
		}
	}
	if (auto double_a = std::dynamic_pointer_cast<DoubleObj>(a)) {
		if (auto double_b = std::dynamic_pointer_cast<DoubleObj>(b)) {
			return double_a->val == double_b->val;
		}
	}
	return false;
}

void Interpreter::check_num_operand(std::shared_ptr<Token> op, std::shared_ptr<Obj> operand) {
	if (auto _ = std::dynamic_pointer_cast<DoubleObj>(operand)) return;
	throw RuntimeError(op, "Operand must be a number"); 
}

void Interpreter::check_num_operands(std::shared_ptr<Token> op, std::shared_ptr<Obj> a, std::shared_ptr<Obj> b) {
	if (auto _ = std::dynamic_pointer_cast<DoubleObj>(a)) {
		if (auto _ = std::dynamic_pointer_cast<DoubleObj>(b)) {
			return;
		}
	}
	throw RuntimeError(op, "Operands must be a number"); 
}

std::string Interpreter::stringify(std::shared_ptr<Obj> val) {
	if (val == nullptr) return "nil";
	if (auto double_val = std::dynamic_pointer_cast<DoubleObj>(val)) {
		auto num_str = std::to_string(double_val->val);
		while (num_str.back() == '0') num_str.pop_back();
		if (num_str.back() == '.') num_str.pop_back();
		return num_str;
	}
	if (auto bool_val = std::dynamic_pointer_cast<BoolObj>(val)) {
		return bool_val->val ? "true" : "false";
	}
	if (auto string_val = std::dynamic_pointer_cast<StringObj>(val)) {
		return string_val->val;
	}
	return "__Obj__";
}