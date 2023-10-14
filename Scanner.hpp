#ifndef SCANNER
#define SCANNER

#include <string>
#include <vector>
#include <unordered_map>
#include "Token.hpp"
#include "Error.hpp"
#include "Obj.hpp"

class Scanner {
public:
	Scanner(std::string source);

	std::vector<Token*> scan_tokens();

private:
	std::string source;
	std::unordered_map<std::string, TokenType> keywords;
	int start;
	int curr;
	int line;

	bool is_at_end();
	
	void scan_token(std::vector<Token*>& tokens);

	char advance();

	void add_token(std::vector<Token*>& tokens, TokenType type);

	void add_token(std::vector<Token*>& tokens, TokenType type, Obj* literal);

	bool match(char expected);

	char peek();

	char peek_next();

	void string(std::vector<Token*>& tokens, char quote);

	bool is_digit(char c); 

	bool is_alpha(char c);

	bool is_alpha_numeric(char c);

	void number(std::vector<Token*>& tokens);

	void identifier(std::vector<Token*>& tokens);
};

#endif