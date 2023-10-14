
#include "Parser.hpp"

Parser::Parser(std::vector<Token*> tokens) : tokens(tokens), curr(0) {}

std::vector<Stmt*> Parser::parse() {
	std::vector<Stmt*> stmts;
	while (!is_at_end()) stmts.push_back(declaration());
	return stmts;
}

Stmt* Parser::declaration() {
	try {
		if (match(VAR)) return var_declaration();
		if (match(FN)) return fn_declaration("fn");
		return stmt();
	} catch (RuntimeError& e) {
		synchronize();
		return nullptr;
	}
}

Stmt* Parser::var_declaration() {
	Token* name = consume(IDENTIFIER, "Expect variable name");
	Expr* initializer = nullptr;
	if (match(EQUAL)) initializer = expression();
	consume(SEMICOLON, "Expect ';' after variable declaration");
	return new Var(name, initializer);
}

FnStmt* Parser::fn_declaration(std::string kind) {
	Token* name = consume(IDENTIFIER, "Expect " + kind + " name");
	consume(LEFT_PAREN, "Expect '(' after " + kind + " name");
	std::vector<Token*> parameters;
	if (!check(RIGHT_PAREN)) {
		do {
			parameters.push_back(consume(IDENTIFIER, "Expect parameter name"));
		} while (match(COMMA));
	}
	consume(RIGHT_PAREN, "Expect ')' after parameters");
	consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
	std::vector<Stmt*> body = block();
	return new FnStmt(name, parameters, body);
}

Stmt* Parser::stmt() {
	if (match(FOR)) return for_stmt();
	if (match(IF)) return if_stmt();
	if (match(PRINT)) return print_stmt();
	if (match(RETURN)) return return_stmt();
	if (match(WHILE)) return while_stmt();
	if (match(LEFT_BRACE)) return new Block(block());
	return expr_stmt();
}

Stmt* Parser::for_stmt() {
	consume(LEFT_PAREN, "Expect '(' after 'for'");
	
	Stmt* initializer;
	if (match(SEMICOLON)) initializer = nullptr;
	else if (match(VAR)) initializer = var_declaration();
	else initializer = expr_stmt();

	Expr* condition;
	if (!check(SEMICOLON)) condition = expression();
	else condition = new Literal(new BoolObj(true));
	consume(SEMICOLON, "Expect ';' after for loop condition");

	Expr* increment = nullptr;
	if (!check(RIGHT_PAREN)) increment = expression();
	consume(RIGHT_PAREN, "Expect ')' after for clauses");

	Stmt* body = stmt();

	if (increment != nullptr) {
		body = new Block({
			body,
			new Expression(increment)
		});
	}

	body = new While(condition, body);
	
	if (initializer != nullptr) body = new Block({initializer, body});
	
	return body;
}

Stmt* Parser::if_stmt() {
	consume(LEFT_PAREN, "Expect '(' after 'if'");
	Expr* condition = expression();
	consume(RIGHT_PAREN, "Expect ')' after if contidion");
	Stmt* then_branch = stmt();
	Stmt* else_branch = nullptr;
	if (match(ELSE)) else_branch = stmt();
	return new If(condition, then_branch, else_branch);
}

Stmt* Parser::print_stmt() {
	Expr* val = expression();
	consume(SEMICOLON, "Expect ';' after value");
	return new Print(val);
}

Stmt* Parser::return_stmt() {
	Token* keyword = prev();
	Expr* val = nullptr;
	if (!check(SEMICOLON)) val = expression();
	consume(SEMICOLON, "Expect ';' after return expression");
	return new Return(keyword, val);
}

Stmt* Parser::while_stmt() {
	consume(LEFT_PAREN, "Expect '(' after 'while'");
	Expr* condition = expression();
	consume(RIGHT_PAREN, "Expect ')' after condition");
	Stmt* body = stmt();
	return new While(condition, body);
}

std::vector<Stmt*> Parser::block() {
	std::vector<Stmt*> stmts;
	while (!check(RIGHT_BRACE) && !is_at_end()) stmts.push_back(declaration());
	consume(RIGHT_BRACE, "Expect '{' after block");
	return stmts;
}

Stmt* Parser::expr_stmt() {
	Expr* expr = expression();
	consume(SEMICOLON, "Expect ';' after value");
	return new Expression(expr);
}

Expr* Parser::expression() {
	return assignment();
}

Expr* Parser::assignment() {
	Expr* expr = logical_or();
	if (match(EQUAL)) {
		Token* equals = prev();
		Expr* val = assignment();
		if (typeid(*expr) == typeid(Variable)) {
			auto variable = dynamic_cast<Variable*>(expr);
			return new Assign(variable->name, val);
		}
		throw RuntimeError(equals, "Invalid assignment target");
	}
	return expr;
}

Expr* Parser::logical_or() {
	Expr* expr = logical_and();
	while (match(OR)) {
		Token* op = prev();
		Expr* right = logical_and();
		expr = new Logical(expr, op, right);
	}
	return expr;
}

Expr* Parser::logical_and() {
	Expr* expr = equality();
	while (match(AND)) {
		Token* op = prev();
		Expr* right = equality();
		expr = new Logical(expr, op, right);
	}
	return expr;
}

Expr* Parser::equality() {
	Expr* expr = comparison();
	while (match(BANG_EQUAL, EQUAL_EQUAL)) {
		Token* op = prev();
		Expr* right = comparison();
		expr = new Binary(expr, op, right);
	}
	return expr;
}

Expr* Parser::comparison() {
	Expr* expr = term();
	while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
		Token* op = prev();
		Expr* right = term();
		expr = new Binary(expr, op, right);
	}
	return expr;
}

Expr* Parser::term() {
	Expr* expr = factor();
	while (match(MINUS, PLUS)) {
		Token* op = prev();
		Expr* right = factor();
		expr = new Binary(expr, op, right);
	}
	return expr;
}

Expr* Parser::factor() {
	Expr* expr = unary();
	while (match(SLASH, STAR)) {
		Token* op = prev();
		Expr* right = unary();
		expr = new Binary(expr, op, right);
	}
	return expr;
}

Expr* Parser::unary() {
	if (match(BANG, MINUS)) {
		Token* op = prev();
		Expr* right = unary();
		return new Unary(op, right);
	}
	return call();
}

Expr* Parser::call() {
	Expr* expr = primary();
	for (;;) {
		if (match(LEFT_PAREN)) expr = finish_call_expr(expr);
		else break;
	}
	return expr;
}

Expr* Parser::finish_call_expr(Expr* callee) {
	std::vector<Expr*> arguments;
	if (!check(RIGHT_PAREN)) {
		do {
			arguments.push_back(expression());
		} while (match(COMMA));
	}
	Token* paren = consume(RIGHT_PAREN, "Expect ')' after arguments");
	return new Call(callee, paren, arguments);
}

Expr* Parser::primary() {
	if (match(FALSE)) return new Literal(new BoolObj(false));
	if (match(TRUE)) return new Literal(new BoolObj(true));
	if (match(NIL)) return new Literal(nullptr);
	if (match(NUMBER, STRING)) return new Literal(prev()->literal);
	if (match(IDENTIFIER)) return new Variable(prev());
	if (match(LEFT_PAREN)) {
		Expr* expr = expression();
		consume(RIGHT_PAREN, "Expect ')' after expression");
		return new Grouping(expr);
	}
	throw RuntimeError(peek(), "Expect expression");
}

Token* Parser::consume(TokenType token, std::string msg) {
	if (check(token)) return advance();
	throw RuntimeError(peek(), msg);
}

void Parser::synchronize() {
	advance();
	while (!is_at_end()) {
		if (prev()->type == SEMICOLON) return;
		switch (peek()->type) {
			case CLASS:
			case FN:
			case VAR:
			case FOR:
			case IF:
			case WHILE:
			case PRINT:
			case RETURN:
			return;
		}
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

Token* Parser::advance() {
	if (!is_at_end()) curr++;
	return prev();
}

bool Parser::is_at_end() {
	return peek()->type == _EOF;
}

Token* Parser::peek() {
	return tokens[curr];
}

Token* Parser::prev() {
	return tokens[curr - 1];
}