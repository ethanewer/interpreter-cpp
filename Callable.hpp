#ifndef CALLABLE
#define CALLABLE

#include <vector>
#include <chrono>
#include "Interpreter.hpp"
#include "Obj.hpp"
#include "ReturnException.hpp"

class Interpreter;
struct Instance;

struct Callable : public Obj { 
	virtual std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) = 0;

	virtual int num_params() = 0;
    
	virtual ~Callable() {}
};

struct Fn : public Callable {
	Fn(
		std::shared_ptr<Token> name, 
		std::vector<std::shared_ptr<Token>> params, 
		std::vector<std::shared_ptr<Stmt>> body, 
		std::shared_ptr<Environment> closure
	);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
	
	std::shared_ptr<Fn> bind(std::shared_ptr<Instance> instance);

	std::shared_ptr<Token> name;
	std::vector<std::shared_ptr<Token>> params;
	std::vector<std::shared_ptr<Stmt>> body;
	std::shared_ptr<Environment> closure;
};

struct Lambda : public Callable {
	Lambda(std::vector<std::shared_ptr<Token>> params, std::vector<std::shared_ptr<Stmt>> body);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;

	std::vector<std::shared_ptr<Token>> params;
	std::vector<std::shared_ptr<Stmt>> body;
};

struct Clock : public Callable {
	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;
	
	int num_params() override;
};

struct Class : public Callable {
    std::string name;
	std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Callable>>> methods;

    Class(std::string name, std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Callable>>> methods);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;

    std::string to_string();
};

struct Instance : public Obj {
	std::string type;
	std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Callable>>> methods;
	std::unordered_map<std::string, std::shared_ptr<Obj>> feilds;

	Instance(std::string type, std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Callable>>> methods);

	std::shared_ptr<Obj> get(std::shared_ptr<Token> name);
	
	void set(std::shared_ptr<Token> name, std::shared_ptr<Obj> val);

	std::string to_string();
};

struct List : public Callable {
	List();

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct ListSize : Callable {
	std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list;

	ListSize(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct ListGet : Callable {
	std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list;

	ListGet(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct ListSet : Callable {
	std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list;

	ListSet(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct ListPush : Callable {
	std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list;

	ListPush(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct ListPop : Callable {
	std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list;

	ListPop(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct Map : public Callable {
	Map();

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct MapGet : Callable {
	std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map;

	MapGet(std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct MapSet : Callable {
	std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map;

	MapSet(std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct MapRemove : Callable {
	std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map;

	MapRemove(std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

struct MapContains : Callable {
	std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map;

	MapContains(std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map);

	std::shared_ptr<Obj> call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) override;

	int num_params() override;
};

#endif