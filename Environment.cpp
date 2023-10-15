#include "Environment.hpp"

Environment::Environment() : enclosing(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

void Environment::define(std::string name, std::shared_ptr<Obj> val) {
	if (val != nullptr) val->has_ref = true;
	values[name] = val;
}

void Environment::assign(std::shared_ptr<Token> name, std::shared_ptr<Obj> val) {
	if (values.count(name->lexeme)) {
		if (val != nullptr) val->has_ref = true;
		values[name->lexeme] = val;
		return;
	}
	if (enclosing != nullptr) {
		enclosing->assign(name, val);
		return;
	}
	throw RuntimeError(name, "Undefined variable '" + name->lexeme + "'");
}

std::shared_ptr<Obj> Environment::get(std::shared_ptr<Token> name) {
	if (values.count(name->lexeme)) return values[name->lexeme];
	if (enclosing != nullptr) return enclosing->get(name);
	throw RuntimeError(name, "Undefined variable '" + name->lexeme + "'");
}
