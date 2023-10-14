#include "Interpreter.hpp"

Interpreter::Interpreter() {
	globals = new Environment();
	env = globals;
	globals->define("clock", new Clock());
}

void Interpreter::interpret(std::vector<Stmt*> stmts) {
	for (auto stmt : stmts) execute(stmt);
}

void Interpreter::execute_block(std::vector<Stmt*>& stmts, Environment* env) {
	Environment* prev_env = this->env;
	try {
		this->env = env;
		for (Stmt* s : stmts) execute(s);
		this->env = prev_env;
	} catch (ReturnException& e) {
		this->env = prev_env;
		throw e;
	} catch (RuntimeError& e) {
		this->env = prev_env;
		throw e;
	}
};

Obj* Interpreter::visit_literal_expr(Literal* expr) {
	return expr->val;
}

Obj* Interpreter::visit_grouping_expr(Grouping* expr) {
	return evaluate(expr->expression);
}

Obj* Interpreter::visit_unary_expr(Unary* expr) {
	Obj* right = evaluate(expr->right);
	DoubleObj* double_val;
	switch (expr->op->type) {
		case BANG: 
			return new BoolObj(!is_truthy(right));
		case MINUS:
			check_num_operand(expr->op, right);
			double_val = dynamic_cast<DoubleObj*>(right);
			return new DoubleObj(-double_val->val);
	}
	return nullptr;
}

Obj* Interpreter::visit_binary_expr(Binary* expr) {
	Obj* left = evaluate(expr->left);
	Obj* right = evaluate(expr->right); 

	if (left == nullptr || right == nullptr) throw RuntimeError(expr->op, "nil can not be added");

	DoubleObj* double_left;
	DoubleObj* double_right;
	StringObj* string_left;
	StringObj* string_right;
	BoolObj* bool_right;

	switch (expr->op->type) {
		case BANG_EQUAL: 
			return new BoolObj(!is_equal(left, right));
		case EQUAL_EQUAL: 
			return new BoolObj(is_equal(left, right));
		case GREATER:
			check_num_operands(expr->op, left, right);
			double_left = dynamic_cast<DoubleObj*>(left);
			double_right = dynamic_cast<DoubleObj*>(right);
			return new BoolObj(double_left->val > double_right->val);
		case GREATER_EQUAL:
			check_num_operands(expr->op, left, right);
			double_left = dynamic_cast<DoubleObj*>(left);
			double_right = dynamic_cast<DoubleObj*>(right);
			return new BoolObj(double_left->val >= double_right->val);
		case LESS:
			check_num_operands(expr->op, left, right);
			double_left = dynamic_cast<DoubleObj*>(left);
			double_right = dynamic_cast<DoubleObj*>(right);
			return new BoolObj(double_left->val < double_right->val);
		case LESS_EQUAL:
			check_num_operands(expr->op, left, right);
			double_left = dynamic_cast<DoubleObj*>(left);
			double_right = dynamic_cast<DoubleObj*>(right);
			return new BoolObj(double_left->val <= double_right->val);
		case PLUS:
			if (typeid(*left) == typeid(DoubleObj) && typeid(*right) == typeid(DoubleObj)) {
				double_left = dynamic_cast<DoubleObj*>(left);
				double_right = dynamic_cast<DoubleObj*>(right);
				return new DoubleObj(double_left->val + double_right->val);
			} 
			if (typeid(*left) == typeid(StringObj) && typeid(*right) == typeid(StringObj)) {
				string_left = dynamic_cast<StringObj*>(left);
				string_right = dynamic_cast<StringObj*>(right);
				return new StringObj(string_left->val + string_right->val);
			} 
			if (typeid(*left) == typeid(StringObj) && typeid(*right) == typeid(DoubleObj)) {
				string_left = dynamic_cast<StringObj*>(left);
				double_right = dynamic_cast<DoubleObj*>(right);
				return new StringObj(string_left->val + std::to_string(double_right->val));
			} 
			if (typeid(*left) == typeid(StringObj) && typeid(*right) == typeid(BoolObj)) {
				string_left = dynamic_cast<StringObj*>(left);
				bool_right = dynamic_cast<BoolObj*>(right);
				std::string bool_str = bool_right->val ? "true" : "false";
				return new StringObj(string_left->val + bool_str);
			}
			throw  RuntimeError(expr->op, "Operands can not be added with '+'");
		case MINUS:
			check_num_operands(expr->op, left, right);
			double_left = dynamic_cast<DoubleObj*>(left);
			double_right = dynamic_cast<DoubleObj*>(right);
			return new DoubleObj(double_left->val - double_right->val);
		case SLASH:
			check_num_operands(expr->op, left, right);
			double_left = dynamic_cast<DoubleObj*>(left);
			double_right = dynamic_cast<DoubleObj*>(right);
			return new DoubleObj(double_left->val / double_right->val);
		case STAR:
			check_num_operands(expr->op, left, right);
			double_left = dynamic_cast<DoubleObj*>(left);
			double_right = dynamic_cast<DoubleObj*>(right);
			return new DoubleObj(double_left->val * double_right->val);
	}
	return nullptr;
}

Obj* Interpreter::visit_variable_expr(Variable* expr) {
	return env->get(expr->name);
}

Obj* Interpreter::visit_assign_expr(Assign* expr) {
	Obj* val = evaluate(expr->val);
	env->assign(expr->name, val);
	return val;
}

Obj* Interpreter::visit_logical_expr(Logical* expr) {
	Obj* left = evaluate(expr->left);
	if (expr->op->type == OR) {
		if (is_truthy(left)) return left;
	} else {
		if (!is_truthy(left)) return left;
	}
	return evaluate(expr->right);
}

Obj* Interpreter::visit_call_expr(Call* expr) {
	Obj* callee = evaluate(expr->callee);
	std::vector<Obj*> arguments;
	for (Expr* a : expr->arguments) arguments.push_back(evaluate(a));
	if (Callable* fn = dynamic_cast<Callable*>(callee)) {
		if (arguments.size() != fn->num_params()) throw RuntimeError(expr->paren, "Incorect number of arguments");
		return fn->call(this, arguments);
	} else throw RuntimeError(expr->paren, "Object is not callable");
}

Obj* Interpreter::visit_lambda_expr(LambdaExpr* expr) {
	return new Lambda(expr);
}

void Interpreter::visit_expression_stmt(Expression* stmt) {
	evaluate(stmt->expression);
}

void Interpreter::visit_print_stmt(Print* stmt) {
	Obj* val = evaluate(stmt->expression);
	std::cout << stringify(val) << '\n';
}

void Interpreter::visit_var_stmt(Var* stmt) {
	Obj* val = nullptr;
	if (stmt->initializer != nullptr) val = evaluate(stmt->initializer);
	env->define(stmt->name->lexeme, val);
}

void Interpreter::visit_block_stmt(Block* stmt) {
	execute_block(stmt->stmts, new Environment(env));
}

void Interpreter::visit_if_stmt(If* stmt) {
	if (is_truthy(evaluate(stmt->condition))) execute(stmt->then_branch);
	else if (stmt->else_branch != nullptr) execute(stmt->else_branch);
}

void Interpreter::visit_while_stmt(While* stmt) {
	while (is_truthy(evaluate(stmt->condition))) execute(stmt->body);
}

void Interpreter::visit_fn_stmt(FnStmt* stmt) {
	Fn* fn = new Fn(stmt, env);
	env->define(stmt->name->lexeme, fn);
}

void Interpreter::visit_return_stmt(Return* stmt) {
	Obj* val = nullptr;
	if (stmt->val != nullptr) val = evaluate(stmt->val);
	throw ReturnException(val);
}

Obj* Interpreter::evaluate(Expr* expr) {
	return expr->accept(this);
}

void Interpreter::execute(Stmt* stmt) {
	stmt->accept(this);
}

bool Interpreter::is_truthy(Obj* val) {
	if (val == nullptr) return false;
	if (typeid(*val) == typeid(BoolObj)) {
		auto bool_val = dynamic_cast<BoolObj*>(val);
		return bool_val->val;
	}
	return true;
}

bool Interpreter::is_equal(Obj* a, Obj* b) {
	if (a == nullptr && b == nullptr) return true;
	if (a == nullptr || b == nullptr) return false;
	if (typeid(*a) == typeid(BoolObj) && typeid(*b) == typeid(BoolObj)) {
		auto bool_a = dynamic_cast<BoolObj*>(a);
		auto bool_b = dynamic_cast<BoolObj*>(b);
		return bool_a->val == bool_b->val;
	}
	if (typeid(*a) == typeid(DoubleObj) && typeid(*b) == typeid(DoubleObj)) {
		auto double_a = dynamic_cast<DoubleObj*>(a);
		auto double_b = dynamic_cast<DoubleObj*>(b);
		return double_a->val == double_b->val;
	}
	return false;
}

void Interpreter::check_num_operand(Token* op, Obj* operand) {
	if (typeid(*operand) == typeid(DoubleObj)) return;
	throw RuntimeError(op, "Operand must be a number"); 
}

void Interpreter::check_num_operands(Token* op, Obj* a, Obj* b) {
	if (typeid(*a) == typeid(DoubleObj) && typeid(*b) == typeid(DoubleObj)) return;
	throw RuntimeError(op, "Operands must be a number"); 
}

std::string Interpreter::stringify(Obj* val) {
	if (val == nullptr) return "nil";
	if (typeid(*val) == typeid(DoubleObj)) {
		auto double_val = dynamic_cast<DoubleObj*>(val);
		auto num_str = std::to_string(double_val->val);
		while (num_str.back() == '0') num_str.pop_back();
		if (num_str.back() == '.') num_str.pop_back();
		return num_str;
	}
	if (typeid(*val) == typeid(BoolObj)) {
		auto bool_val = dynamic_cast<BoolObj*>(val);
		return bool_val->val ? "true" : "false";
	}
	if (typeid(*val) == typeid(StringObj)) {
		auto string_val = dynamic_cast<StringObj*>(val);
		return string_val->val;
	}
	return "__Obj__";
}

Interpreter::~Interpreter() {
	delete globals;
}