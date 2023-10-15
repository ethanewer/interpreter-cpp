#ifndef CALLABLE
#define CALLABLE

#include <vector>
#include <chrono>
#include "Interpreter.hpp"
#include "Obj.hpp"
#include "ReturnException.hpp"

class Interpreter;

class Callable : public Obj {
public: 
	virtual std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) = 0;

	virtual int num_params() = 0;
    
	virtual ~Callable() {}
};

class Fn : public Callable {
public:
	Fn(
		std::shared_ptr<Token> name, 
		std::vector<std::shared_ptr<Token>> params, 
		std::vector<std::shared_ptr<Stmt>> body, 
		std::shared_ptr<Environment> closure
	);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;

private:
	std::shared_ptr<Token> name;
	std::vector<std::shared_ptr<Token>> params;
	std::vector<std::shared_ptr<Stmt>> body;
	std::shared_ptr<Environment> closure;
};

class Lambda : public Callable {
public:
	Lambda(std::vector<std::shared_ptr<Token>> params, std::vector<std::shared_ptr<Stmt>> body);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;

private:
	std::vector<std::shared_ptr<Token>> params;
	std::vector<std::shared_ptr<Stmt>> body;
};

class Clock : public Callable {
public:
	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;
	
	int num_params() override;
};
#endif