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

	std::vector<std::shared_ptr<Token>> scan_tokens();

private:
	std::string source;
	std::unordered_map<std::string, TokenType> keywords;
	int start;
	int curr;
	int line;

	bool is_at_end();
	
	void scan_token(std::vector<std::shared_ptr<Token>>& tokens);

	char advance();

	void add_token(std::vector<std::shared_ptr<Token>>& tokens, TokenType type);

	void add_token(std::vector<std::shared_ptr<Token>>& tokens, TokenType type, std::shared_ptr<Obj> literal);

	bool match(char expected);

	char peek();

	char peek_next();

	void string(std::vector<std::shared_ptr<Token>>& tokens, char quote);

	bool is_digit(char c); 

	bool is_alpha(char c);

	bool is_alpha_numeric(char c);

	void number(std::vector<std::shared_ptr<Token>>& tokens);

	void identifier(std::vector<std::shared_ptr<Token>>& tokens);
};

#endif