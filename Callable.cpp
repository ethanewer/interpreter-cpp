#include "Callable.hpp"

Fn::Fn(FnStmt* stmt, Environment* closure) : stmt(stmt), closure(closure) {}

Obj* Fn::call(Interpreter* interpreter, std::vector<Obj*> arguments) {
	Environment* env = new Environment(closure);
	for (int i = 0; i < stmt->params.size(); i++) {
		env->define(stmt->params[i]->lexeme, arguments[i]);
	}
	try {
		interpreter->execute_block(stmt->body, env);
	} catch (ReturnException return_value) {
		return return_value.val;
	}
	return nullptr;
}

int Fn::num_params() {
	return stmt->params.size();
}

Lambda::Lambda(LambdaExpr* expr) : expr(expr) {}

Obj* Lambda::call(Interpreter* interpreter, std::vector<Obj*> arguments) {
	Environment* env = new Environment();
	for (int i = 0; i < expr->params.size(); i++) {
		env->define(expr->params[i]->lexeme, arguments[i]);
	}
	try {
		interpreter->execute_block(expr->body, env);
	} catch (ReturnException return_value) {
		return return_value.val;
	}
	return nullptr;
}

int Lambda::num_params() {
	return expr->params.size();
}

Obj* Clock::call(Interpreter* interpreter, std::vector<Obj*> arguments) {
	auto time = std::chrono::system_clock::now();
	auto duration = time.time_since_epoch();
	double seconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
	return new DoubleObj(seconds);
}

int Clock::num_params() {
	return 0;
}