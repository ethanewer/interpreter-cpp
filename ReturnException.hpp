#ifndef RETURN
#define RETURN

#include <stdexcept>
#include "Obj.hpp"

class ReturnException : public std::exception {
public:
	std::shared_ptr<Obj> val;

    ReturnException(std::shared_ptr<Obj> val) : val(val) {}
};

#endif