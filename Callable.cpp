#include "Callable.hpp"

Fn::Fn(
		std::shared_ptr<Token> name, 
		std::vector<std::shared_ptr<Token>> params, 
		std::vector<std::shared_ptr<Stmt>> body, 
		std::shared_ptr<Environment> closure
	) : name(name), params(params), body(body), closure(closure) {}

std::shared_ptr<Obj> Fn::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	auto env = std::make_shared<Environment>(closure);
	for (int i = 0; i < params.size(); i++) {
		env->define(params[i]->lexeme, arguments[i]);
	}
	try {
		interpreter->execute_block(body, env);
	} catch (ReturnException return_value) {
		return return_value.val;
	}
	return nullptr;
}

int Fn::num_params() {
	return params.size();
}

Lambda::Lambda(std::vector<std::shared_ptr<Token>> params, std::vector<std::shared_ptr<Stmt>> body)
	: params(params), body(body) {}

std::shared_ptr<Obj> Lambda::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	auto env = std::make_shared<Environment>();
	for (int i = 0; i < params.size(); i++) {
		env->define(params[i]->lexeme, arguments[i]);
	}
	try {
		interpreter->execute_block(body, env);
	} catch (ReturnException return_value) {
		return return_value.val;
	}
	return nullptr;
}

int Lambda::num_params() {
	return params.size();
}

std::shared_ptr<Obj> Clock::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	auto time = std::chrono::system_clock::now();
	auto duration = time.time_since_epoch();
	double seconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
	return std::make_shared<DoubleObj>(seconds);
}

int Clock::num_params() {
	return 0;
}