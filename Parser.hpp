#ifndef PARSER
#define PARSER

#include <iostream>
#include <vector>
#include <stdexcept>
#include <utility>
#include "Error.hpp"
#include "Token.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"


class Parser {
public:
	Parser(std::vector<std::shared_ptr<Token>> tokens);

	std::vector<std::shared_ptr<Stmt>> parse();

private:
	std::vector<std::shared_ptr<Token>> tokens;
	int curr;

	std::shared_ptr<Stmt> declaration();

	std::shared_ptr<Stmt> var_declaration();

	std::shared_ptr<FnStmt> fn_declaration(std::string kind);

	std::shared_ptr<Stmt> stmt();

	std::shared_ptr<Stmt> for_stmt();

	std::shared_ptr<Stmt> if_stmt();

	std::shared_ptr<Stmt> print_stmt();

	std::shared_ptr<Stmt> return_stmt();

	std::shared_ptr<Stmt> while_stmt();

	std::vector<std::shared_ptr<Stmt>> block();

	std::shared_ptr<Stmt> expr_stmt();

	std::shared_ptr<Expr> expression();

	std::shared_ptr<Expr> lambda_expr();

	std::shared_ptr<Expr> assignment();

	std::shared_ptr<Expr> logical_or();

	std::shared_ptr<Expr> logical_and();

	std::shared_ptr<Expr> equality();

	std::shared_ptr<Expr> comparison();

	std::shared_ptr<Expr> term();

	std::shared_ptr<Expr> factor();

	std::shared_ptr<Expr> unary();

	std::shared_ptr<Expr> call();

	std::shared_ptr<Expr> finish_call_expr(std::shared_ptr<Expr> callee);

	std::shared_ptr<Expr> primary();

	std::shared_ptr<Token> consume(TokenType token, std::string msg);

	void synchronize();

	template <typename... Args>
	bool match(Args... types);

	bool check(TokenType type);

	std::shared_ptr<Token> advance();

	bool is_at_end();

	std::shared_ptr<Token> peek();

	std::shared_ptr<Token> prev();
};

#endif