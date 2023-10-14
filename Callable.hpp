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
	virtual Obj* call(Interpreter* interpreter, std::vector<Obj*> arguments) = 0;

	virtual int num_params() = 0;
    
	virtual ~Callable() {}
};

class Fn : public Callable {
public:
	Fn(FnStmt* stmt, Environment* closure);

	Obj* call(Interpreter* interpreter, std::vector<Obj*> arguments) override;

	int num_params() override;

private:
	FnStmt* stmt;
	Environment* closure;
};

class Lambda : public Callable {
public:
	Lambda(LambdaExpr* expr);

	Obj* call(Interpreter* interpreter, std::vector<Obj*> arguments) override;

	int num_params() override;

private:
	LambdaExpr* expr;
};

class Clock : public Callable {
public:
	Obj* call(Interpreter* interpreter, std::vector<Obj*> arguments) override;
	
	int num_params() override;
};
#endif