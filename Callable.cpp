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

std::shared_ptr<Fn> Fn::bind(std::shared_ptr<Instance> instance) {
	auto env = std::make_shared<Environment>(closure);
	env->define("this", instance);
	return std::make_shared<Fn>(name, params, body, env);
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

Class::Class(std::string name, std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<Callable>>> methods) 
	: name(name), methods(methods) {}

std::shared_ptr<Obj> Class::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	auto instance = std::make_shared<Instance>(name, methods);
	if (methods->count("init")) {
		if (auto method = std::dynamic_pointer_cast<Fn>((*methods)["init"])) {
			method->bind(instance)->call(interpreter, arguments);
		}
	}
	return instance;
}

int Class::num_params() {
	if (methods->count("init")) return (*methods)["init"]->num_params();
	return 0;
}

std::string Class::to_string() {
	return "<class " + name + ">";
}

Instance::Instance(std::string type, std::shared_ptr<std::unordered_map<std::string,std::shared_ptr<Callable>>> methods)
	: type(type), methods(methods) {}

std::shared_ptr<Obj> Instance::get(std::shared_ptr<Token> name) {
	if (feilds.count(name->lexeme)) {
		return feilds[name->lexeme];
	} else if (methods->count(name->lexeme)) {
		if (auto method = std::dynamic_pointer_cast<Fn>((*methods)[name->lexeme])) {
			return method->bind(std::shared_ptr<Instance>(this, [](Instance*) {}));
		}
		return (*methods)[name->lexeme];
	}
	throw RuntimeError(name, "Undefined property '" + name->lexeme + "'");
}

void Instance::set(std::shared_ptr<Token> name, std::shared_ptr<Obj> val) {
	feilds[name->lexeme] = val;
}

std::string Instance::to_string() {
	return "<instance of class " + type + ">";
}

List::List() {}

std::shared_ptr<Obj> List::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	auto vec = std::make_shared<std::vector<std::shared_ptr<Obj>>>();
	auto methods = std::make_shared<std::unordered_map<std::string, std::shared_ptr<Callable>>>();
	(*methods)["size"] = std::make_shared<ListSize>(vec);
	(*methods)["get"] = std::make_shared<ListGet>(vec);
	(*methods)["set"] = std::make_shared<ListSet>(vec);
	(*methods)["push"] = std::make_shared<ListPush>(vec);
	(*methods)["pop"] = std::make_shared<ListPop>(vec);
	return std::make_shared<Instance>("List", methods);
}

int List::num_params() {
	return 0;
}

ListSize::ListSize(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list) : list(list) {}

std::shared_ptr<Obj> ListSize::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	return std::make_shared<DoubleObj>(list->size());
}

int ListSize::num_params() {
	return 0;
}

ListGet::ListGet(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list) : list(list) {}

std::shared_ptr<Obj> ListGet::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	if (auto double_obj = std::dynamic_pointer_cast<DoubleObj>(arguments[0])) {
		int idx = (int) double_obj->val;
		if (idx < 0 || idx >= list->size()) return nullptr;
		return (*list)[idx];
	}
	return nullptr;
}

int ListGet::num_params() {
	return 1;
}

ListSet::ListSet(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list) : list(list) {}

std::shared_ptr<Obj> ListSet::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	if (auto double_obj = std::dynamic_pointer_cast<DoubleObj>(arguments[0])) {
		int idx = (int) double_obj->val;
		if (0 <= idx && idx < list->size()) (*list)[idx] = arguments[1];
	}
	return nullptr;
}

int ListSet::num_params() {
	return 2;
}

ListPush::ListPush(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list) : list(list) {}

std::shared_ptr<Obj> ListPush::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	list->push_back(arguments[0]);
	return nullptr;
}

int ListPush::num_params() {
	return 1;
}

ListPop::ListPop(std::shared_ptr<std::vector<std::shared_ptr<Obj>>> list) : list(list) {}

std::shared_ptr<Obj> ListPop::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	if (list->size() == 0) return nullptr;
	auto back = list->back();
	list->pop_back();
	return back;
}

int ListPop::num_params() {
	return 0;
}

Map::Map() {}

std::shared_ptr<Obj> Map::call(Interpreter * interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	auto map = std::make_shared<std::unordered_map<size_t, std::shared_ptr<Obj>>>();
	auto methods = std::make_shared<std::unordered_map<std::string, std::shared_ptr<Callable>>>();
	(*methods)["get"] = std::make_shared<MapGet>(map);
	(*methods)["set"] = std::make_shared<MapSet>(map);
	(*methods)["remove"] = std::make_shared<MapRemove>(map);
	(*methods)["has"] = std::make_shared<MapContains>(map);
	return std::make_shared<Instance>("Map", methods);
}

int Map::num_params() {
	return 0;
}

MapGet::MapGet(std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map) : map(map) {}

std::shared_ptr<Obj> MapGet::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	size_t key = arguments[0]->hash();
	if (map->count(key)) return (*map)[key];
	return nullptr;
}

int MapGet::num_params() {
	return 1;
}

MapSet::MapSet(std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map) : map(map) {}

std::shared_ptr<Obj> MapSet::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	size_t key = arguments[0]->hash();
	(*map)[key] = arguments[1];
	return nullptr;
}

int MapSet::num_params() {
	return 2;
}

MapRemove::MapRemove(std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map) : map(map) {}

std::shared_ptr<Obj> MapRemove::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	size_t key = arguments[0]->hash();
	if (map->count(key)) map->erase(key);
	return nullptr;
}

int MapRemove::num_params() {
	return 1;
}

MapContains::MapContains(std::shared_ptr<std::unordered_map<size_t, std::shared_ptr<Obj>>> map) : map(map) {}

std::shared_ptr<Obj> MapContains::call(Interpreter* interpreter, std::vector<std::shared_ptr<Obj>> arguments) {
	size_t key = arguments[0]->hash();
	return std::make_shared<BoolObj>(map->count(key));
}

int MapContains::num_params() {
	return 1;
}
	