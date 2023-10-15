#include "Environment.hpp"

Environment::Environment() : enclosing(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

void Environment::define(std::string name, std::shared_ptr<Obj> val) {
	values[name] = val;
}

void Environment::assign(std::shared_ptr<Token> name, std::shared_ptr<Obj> val) {
	if (values.count(name->lexeme)) {
		values[name->lexeme] = val;
		return;
	}
	if (enclosing != nullptr) {
		enclosing->assign(name, val);
		return;
	}
	throw RuntimeError(name, "Undefined variable '" + name->lexeme + "'");
}

void Environment::assign_at(int dist, std::shared_ptr<Token> name, std::shared_ptr<Obj> val) {
	if (dist == 0) {
		values[name->lexeme] = val;
		return;
	}
	std::shared_ptr<Environment> env = enclosing;
	for (int i = 1; i < dist; i++) env = env->enclosing;
	env->values[name->lexeme] = val;
}

std::shared_ptr<Obj> Environment::get(std::shared_ptr<Token> name) {
	if (values.count(name->lexeme)) return values[name->lexeme];
	if (enclosing != nullptr) return enclosing->get(name);
	throw RuntimeError(name, "Undefined variable '" + name->lexeme + "'");
}

std::shared_ptr<Obj> Environment::get_at(int dist, std::string name) {
	if (dist == 0) return values[name];
	std::shared_ptr<Environment> env = enclosing;
	for (int i = 1; i < dist; i++) env = env->enclosing;
	return env->values[name];
}
