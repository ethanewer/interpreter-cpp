
#include "Parser.hpp"

Parser::Parser(std::vector<std::shared_ptr<Token>> tokens) : tokens(tokens), curr(0) {}

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
	std::vector<std::shared_ptr<Stmt>> stmts;
	while (!is_at_end()) stmts.push_back(declaration());
	return stmts;
}

std::shared_ptr<Stmt> Parser::declaration() {
	if (match(CLASS)) return class_declaration();
	if (match(LET)) return var_declaration();
	if (match(FN)) return fn_declaration("fn");
	return stmt();
}

std::shared_ptr<Stmt> Parser::class_declaration() {
	std::shared_ptr<Token> name = consume(IDENTIFIER, "Expect class name");
	consume(LEFT_BRACE, "Expect '{' before class body");
	std::vector<std::shared_ptr<FnStmt>> methods;
	while (!check(RIGHT_BRACE)) methods.push_back(fn_declaration("method"));
	consume(RIGHT_BRACE, "Expect '}' after class body");
	return std::make_shared<ClassStmt>(name, methods);
}

std::shared_ptr<Stmt> Parser::var_declaration() {
	std::shared_ptr<Token> name = consume(IDENTIFIER, "Expect variable name");
	std::shared_ptr<Expr> initializer = nullptr;
	if (match(EQUAL)) initializer = expression();
	consume(SEMICOLON, "Expect ';' after variable declaration");
	return std::make_shared<Var>(name, initializer);
}

std::shared_ptr<FnStmt> Parser::fn_declaration(std::string kind) {
	std::shared_ptr<Token> name = consume(IDENTIFIER, "Expect " + kind + " name");
	consume(LEFT_PAREN, "Expect '(' after " + kind + " name");
	std::vector<std::shared_ptr<Token>> params;
	if (!check(RIGHT_PAREN)) {
		do {
			params.push_back(consume(IDENTIFIER, "Expect parameter name"));
		} while (match(COMMA));
	}
	consume(RIGHT_PAREN, "Expect ')' after parameters");
	consume(LEFT_BRACE, "Expect '{' before " + kind + " body");
	std::vector<std::shared_ptr<Stmt>> body = block();
	return std::make_shared<FnStmt>(name, params, body);
}

std::shared_ptr<Stmt> Parser::stmt() {
	if (match(FOR)) return for_stmt();
	if (match(IF)) return if_stmt();
	if (match(PRINT)) return print_stmt();
	if (match(RETURN)) return return_stmt();
	if (match(WHILE)) return while_stmt();
	if (match(LEFT_BRACE)) return std::make_shared<Block>(block());
	return expr_stmt();
}

std::shared_ptr<Stmt> Parser::for_stmt() {
	consume(LEFT_PAREN, "Expect '(' after 'for'");
	
	std::shared_ptr<Stmt> initializer;
	if (match(SEMICOLON)) initializer = nullptr;
	else if (match(LET)) initializer = var_declaration();
	else initializer = expr_stmt();

	std::shared_ptr<Expr> condition = nullptr;
	if (!check(SEMICOLON)) condition = expression();
	else condition = std::make_shared<Literal>(std::make_shared<BoolObj>(true));
	consume(SEMICOLON, "Expect ';' after for loop condition");

	std::shared_ptr<Expr> increment = nullptr;
	if (!check(RIGHT_PAREN)) increment = expression();
	consume(RIGHT_PAREN, "Expect ')' after for clauses");

	std::shared_ptr<Stmt> body = stmt();

	if (increment != nullptr) {
		body = std::make_shared<Block>(std::vector<std::shared_ptr<Stmt>> {
			body, std::make_shared<Expression>(increment)
		});
	}

	body = std::make_shared<While>(condition, body);
	
	if (initializer != nullptr) {
		body = std::make_shared<Block>(std::vector<std::shared_ptr<Stmt>> {
			initializer, body
		});
	}
	
	return body;
}

std::shared_ptr<Stmt> Parser::if_stmt() {
	consume(LEFT_PAREN, "Expect '(' after 'if'");
	std::shared_ptr<Expr> condition = expression();
	consume(RIGHT_PAREN, "Expect ')' after if contidion");
	std::shared_ptr<Stmt> then_branch = stmt();
	std::shared_ptr<Stmt> else_branch = nullptr;
	if (match(ELSE)) else_branch = stmt();
	return std::make_shared<If>(condition, then_branch, else_branch);
}

std::shared_ptr<Stmt> Parser::print_stmt() {
	std::shared_ptr<Expr> val = expression();
	consume(SEMICOLON, "Expect ';' after value");
	return std::make_shared<Print>(val);
}

std::shared_ptr<Stmt> Parser::return_stmt() {
	std::shared_ptr<Token> keyword = prev();
	std::shared_ptr<Expr> val = nullptr;
	if (!check(SEMICOLON)) val = expression();
	consume(SEMICOLON, "Expect ';' after return expression");
	return std::make_shared<Return>(keyword, val);
}

std::shared_ptr<Stmt> Parser::while_stmt() {
	consume(LEFT_PAREN, "Expect '(' after 'while'");
	std::shared_ptr<Expr> condition = expression();
	consume(RIGHT_PAREN, "Expect ')' after condition");
	std::shared_ptr<Stmt> body = stmt();
	return std::make_shared<While>(condition, body);
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
	std::vector<std::shared_ptr<Stmt>> stmts;
	while (!check(RIGHT_BRACE) && !is_at_end()) stmts.push_back(declaration());
	consume(RIGHT_BRACE, "Expect '}' after block");
	return stmts;
}

std::shared_ptr<Stmt> Parser::expr_stmt() {
	std::shared_ptr<Expr> expr = expression();
	consume(SEMICOLON, "Expect ';' after value");
	return std::make_shared<Expression>(expr);
}

std::shared_ptr<Expr> Parser::expression() {
	return lambda_expr();
}

std::shared_ptr<Expr> Parser::lambda_expr() {
	if (match(FN)) {
		consume(LEFT_PAREN, "Expect '(' after anonymous fn");
		std::vector<std::shared_ptr<Token>> params;
		if (!check(RIGHT_PAREN)) {
			do {
				params.push_back(consume(IDENTIFIER, "Expect parameter name"));
			} while (match(COMMA));
		}
		consume(RIGHT_PAREN, "Expect ')' after parameeters");
		consume(LEFT_BRACE, "Expect '{' before fn body.");
		std::vector<std::shared_ptr<Stmt>> body = block();
		return std::make_shared<LambdaExpr>(params, body);
	}
	return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
	std::shared_ptr<Expr> expr = logical_or();
	if (match(EQUAL)) {
		std::shared_ptr<Token> equals = prev();
		std::shared_ptr<Expr> val = assignment();
		if (auto variable = std::dynamic_pointer_cast<Variable>(expr)) {
			return std::make_shared<Assign>(variable->name, val);
		} else if (auto get = std::dynamic_pointer_cast<Get>(expr)) {
			return std::make_shared<Set>(get->obj, get->name, val);
		}
		throw RuntimeError(equals, "Invalid assignment target");
	}
	return expr;
}

std::shared_ptr<Expr> Parser::logical_or() {
	std::shared_ptr<Expr> expr = logical_and();
	while (match(OR)) {
		std::shared_ptr<Token> op = prev();
		std::shared_ptr<Expr> right = logical_and();
		expr = std::make_shared<Logical>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr> Parser::logical_and() {
	std::shared_ptr<Expr> expr = equality();
	while (match(AND)) {
		std::shared_ptr<Token> op = prev();
		std::shared_ptr<Expr> right = equality();
		expr = std::make_shared<Logical>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr> Parser::equality() {
	std::shared_ptr<Expr> expr = comparison();
	while (match(BANG_EQUAL, EQUAL_EQUAL)) {
		std::shared_ptr<Token> op = prev();
		std::shared_ptr<Expr> right = comparison();
		expr = std::make_shared<Binary>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
	std::shared_ptr<Expr> expr = term();
	while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
		std::shared_ptr<Token> op = prev();
		std::shared_ptr<Expr> right = term();
		expr = std::make_shared<Binary>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr> Parser::term() {
	std::shared_ptr<Expr> expr = factor();
	while (match(MINUS, PLUS)) {
		std::shared_ptr<Token> op = prev();
		std::shared_ptr<Expr> right = factor();
		expr = std::make_shared<Binary>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr> Parser::factor() {
	std::shared_ptr<Expr> expr = unary();
	while (match(SLASH, STAR)) {
		std::shared_ptr<Token> op = prev();
		std::shared_ptr<Expr> right = unary();
		expr = std::make_shared<Binary>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<Expr> Parser::unary() {
	if (match(BANG, MINUS)) {
		std::shared_ptr<Token> op = prev();
		std::shared_ptr<Expr> right = unary();
		return std::make_shared<Unary>(op, right);
	}
	return call();
}

std::shared_ptr<Expr> Parser::call() {
	std::shared_ptr<Expr> expr = primary();
	for (;;) {
		if (match(LEFT_PAREN)) {
			expr = finish_call_expr(expr);
		} else if (match(DOT)) {
			std::shared_ptr<Token>  name = consume(IDENTIFIER, "Expect property after '.'");
			expr = std::make_shared<Get>(expr, name);
		} else {
			break;
		}
	}
	return expr;
}

std::shared_ptr<Expr> Parser::finish_call_expr(std::shared_ptr<Expr> callee) {
	std::vector<std::shared_ptr<Expr>> arguments;
	if (!check(RIGHT_PAREN)) {
		do {
			arguments.push_back(expression());
		} while (match(COMMA));
	}
	std::shared_ptr<Token> paren = consume(RIGHT_PAREN, "Expect ')' after arguments");
	return std::make_shared<Call>(callee, paren, arguments);
}

std::shared_ptr<Expr> Parser::primary() {
	if (match(FALSE)) return std::make_shared<Literal>(std::make_shared<BoolObj>(false));
	if (match(TRUE)) return std::make_shared<Literal>(std::make_shared<BoolObj>(true));
	if (match(NIL)) return std::make_shared<Literal>(nullptr);
	if (match(NUMBER, STRING)) return std::make_shared<Literal>(prev()->literal);
	if (match(IDENTIFIER)) return std::make_shared<Variable>(prev());
	if (match(LEFT_PAREN)) {
		std::shared_ptr<Expr> expr = expression();
		consume(RIGHT_PAREN, "Expect ')' after expression");
		return std::make_shared<Grouping>(expr);
	}
	throw RuntimeError(peek(), "Expect expression");
}

std::shared_ptr<Token> Parser::consume(TokenType token, std::string msg) {
	if (check(token)) return advance();
	throw RuntimeError(peek(), msg);
}

void Parser::synchronize() {
	advance();
	while (!is_at_end()) {
		if (prev()->type == SEMICOLON) return;
		advance();
	}
}

template <typename... Args>
bool Parser::match(Args... types) {
	for (auto type : {types...}) {
		if (check(type)) {
			advance();
			return true;
		}
	}
	return false;
}

bool Parser::check(TokenType type) {
	if (is_at_end()) return false;
	return peek()->type == type;
}

std::shared_ptr<Token> Parser::advance() {
	if (!is_at_end()) curr++;
	return prev();
}

bool Parser::is_at_end() {
	return peek()->type == _EOF;
}

std::shared_ptr<Token> Parser::peek() {
	return tokens[curr];
}

std::shared_ptr<Token> Parser::prev() {
	return tokens[curr - 1];
}