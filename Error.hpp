#ifndef RUNTIMEERROR
#define RUNTIMEERROR

#include <stdexcept>
#include <string>
#include "Token.hpp"

class SyntaxError : public std::runtime_error {
public:
	int line;

    SyntaxError(int line, const std::string& message)
        : std::runtime_error(message), line(line) {}
};

class RuntimeError : public std::runtime_error {
public:
	Token* token;

    RuntimeError(Token* token, const std::string& message)
        : std::runtime_error(message), token(token) {}
};

#endif