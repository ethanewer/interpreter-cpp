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
	
	Environment(std::shared_ptr<Environment> enclosing);

	void define(std::string name, std::shared_ptr<Obj> val);

	void assign(std::shared_ptr<Token> name, std::shared_ptr<Obj> val);

	std::shared_ptr<Obj> get(std::shared_ptr<Token> name);

	~Environment() {}

private:
	std::shared_ptr<Environment> enclosing;
	std::unordered_map<std::string, std::shared_ptr<Obj>> values;
};

#endif