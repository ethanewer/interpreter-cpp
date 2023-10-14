#include "Environment.hpp"

Environment::Environment() : enclosing(nullptr) {}

Environment::Environment(Environment* enclosing) : enclosing(enclosing) {}

void Environment::define(std::string name, Obj* val) {
	if (val != nullptr) val->has_ref = true;
	values[name] = val;
}

void Environment::assign(Token* name, Obj* val) {
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

Obj* Environment::get(Token* name) {
	if (values.count(name->lexeme)) return values[name->lexeme];
	if (enclosing != nullptr) return enclosing->get(name);
	throw RuntimeError(name, "Undefined variable '" + name->lexeme + "'");
}

Environment::~Environment() {
	for (auto it = values.begin(); it != values.end(); it++) {
		if (it->second != nullptr) delete it->second;
	}
}
