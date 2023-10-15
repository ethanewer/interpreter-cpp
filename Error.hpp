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
	std::shared_ptr<Token> token;

    RuntimeError(std::shared_ptr<Token> token, const std::string& message)
        : std::runtime_error(message), token(token) {}
};

#endif