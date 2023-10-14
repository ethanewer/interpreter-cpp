#ifndef ENVIRONMENT
#define ENVIRONMENT

#include <unordered_map>
#include <iostream>
#include <string>
#include "Token.hpp"
#include "Obj.hpp"
#include "Error.hpp"

class Environment {
public:
	Environment();
	
	Environment(Environment* enclosing);

	void define(std::string name, Obj* val);

	void assign(Token* name, Obj* val);

	Obj* get(Token* name);

	~Environment();

private:
	Environment* enclosing;
	std::unordered_map<std::string, Obj*> values;
};

#endif