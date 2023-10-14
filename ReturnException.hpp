#ifndef RETURN
#define RETURN

#include <stdexcept>
#include "Obj.hpp"

class ReturnException : public std::exception {
public:
	Obj* val;

    ReturnException(Obj* val) : val(val) {}
};

#endif