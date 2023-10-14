#ifndef PARSER
#define PARSER

#include <iostream>
#include <vector>
#include <stdexcept>
#include <utility>
#include "Error.hpp"
#include "Token.hpp"
#include "Stmt.hpp"


class Parser {
public:
	Parser(std::vector<Token*> tokens);

	std::vector<Stmt*> parse();

private:
	std::vector<Token*> tokens;
	int curr;

	Stmt* declaration();

	Stmt* var_declaration();

	FnStmt* fn_declaration(std::string kind);

	Stmt* stmt();

	Stmt* for_stmt();

	Stmt* if_stmt();

	Stmt* print_stmt();

	Stmt* return_stmt();

	Stmt* while_stmt();

	std::vector<Stmt*> block();

	Stmt* expr_stmt();

	Expr* expression();

	Expr* assignment();

	Expr* logical_or();

	Expr* logical_and();

	Expr* equality();

	Expr* comparison();

	Expr* term();

	Expr* factor();

	Expr* unary();

	Expr* call();

	Expr* finish_call_expr(Expr* callee);

	Expr* primary();

	Token* consume(TokenType token, std::string msg);

	void synchronize();

	template <typename... Args>
	bool match(Args... types);

	bool check(TokenType type);

	Token* advance();

	bool is_at_end();

	Token* peek();

	Token* prev();
};

#endif