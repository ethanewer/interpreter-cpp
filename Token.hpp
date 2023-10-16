#ifndef TOKEN
#define TOKEN

#include <string>
#include "Obj.hpp"

enum TokenType {
	// Single-character tokens
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, MOD,
	// One or two character tokens
	BANG, BANG_EQUAL, EQUAL, EQUAL_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL, STAR_STAR, SLASH_SLASH,
	// Literals
	IDENTIFIER, STRING, NUMBER,
	// Keywords
	AND, CLASS, ELSE, FALSE, FN, FOR, IF, NIL, OR, PRINT, RETURN, SUPER, THIS, TRUE, LET, WHILE,
	// Brackers
	LEFT_BRACKET, RIGHT_BRACKET,
	// += -= *= /= %= //=
	PLUS_EQUAL, MINUS_EQUAL, STAR_EQUAL, SLASH_EQUAL, MOD_EQUAL, SLASH_SLASH_EQUAL,
	
	_EOF,
};

class Token {
public:
	TokenType type;
	std::string lexeme;
	std::shared_ptr<Obj> literal;
	int line;

	Token(TokenType type, std::string lexeme, std::shared_ptr<Obj> literal, int line) 
		: type(type), lexeme(lexeme), literal(literal), line(line) {}
};

#endif