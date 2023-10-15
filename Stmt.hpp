#ifndef STMT
#define STMT

#include "Expr.hpp"

class Expr;
class Expression;
class Print;
class Var;
class Block;
class If;
class While;
class FnStmt;
class Return;

class Stmt {
public:
	class Visitor {
	public:
		virtual void visit_expression_stmt(Expression* stmt) = 0;
		virtual void visit_print_stmt(Print* stmt) = 0;
		virtual void visit_var_stmt(Var* stmt) = 0;
		virtual void visit_block_stmt(Block* stmt) = 0;
		virtual void visit_if_stmt(If* stmt) = 0;
		virtual void visit_while_stmt(While* stmt) = 0;
		virtual void visit_fn_stmt(FnStmt* stmt) = 0;
		virtual void visit_return_stmt(Return* stmt) = 0;
	};

	virtual void accept(Visitor* visitor) = 0;

	virtual ~Stmt() {}
};

class Expression : public Stmt {
public:
	std::shared_ptr<Expr> expression;

	Expression(std::shared_ptr<Expr> expression) : expression(expression) {}

	void accept(Visitor* visitor) override {
		visitor->visit_expression_stmt(this);
	}
};

class Print : public Stmt {
public:
	std::shared_ptr<Expr> expression;

	Print(std::shared_ptr<Expr> expression) : expression(expression) {}

	void accept(Visitor* visitor) override {
		visitor->visit_print_stmt(this);
	}
};

class Var : public Stmt {
public:
	std::shared_ptr<Token> name;
	std::shared_ptr<Expr> initializer;
	
	Var(std::shared_ptr<Token> name, std::shared_ptr<Expr> initializer) : name(name),  initializer(initializer) {}

	void accept(Visitor* visitor) override {
		visitor->visit_var_stmt(this);
	}
};

class Block : public Stmt {
public:
	std::vector<std::shared_ptr<Stmt>> stmts;

	Block(std::vector<std::shared_ptr<Stmt>> stmts) : stmts(stmts) {}

	void accept(Visitor* visitor) override {
		visitor->visit_block_stmt(this);
	}
};

class If : public Stmt {
public:
	std::shared_ptr<Expr> condition;
	std::shared_ptr<Stmt> then_branch;
	std::shared_ptr<Stmt> else_branch;

	If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> then_branch, std::shared_ptr<Stmt> else_branch) 
		: condition(condition), then_branch(then_branch), else_branch(else_branch) {}

	void accept(Visitor* visitor) override {
		visitor->visit_if_stmt(this);
	}
};

class While : public Stmt {
public:
	std::shared_ptr<Expr> condition;
	std::shared_ptr<Stmt> body;

	While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body) : condition(condition), body(body) {}

	void accept(Visitor* visitor) override {
		visitor->visit_while_stmt(this);
	}
};

class FnStmt : public Stmt {
public:
	std::shared_ptr<Token> name;
	std::vector<std::shared_ptr<Token>> params;
	std::vector<std::shared_ptr<Stmt>> body;

	FnStmt(std::shared_ptr<Token> name, std::vector<std::shared_ptr<Token>> params, std::vector<std::shared_ptr<Stmt>> body)
	 : name(name), params(params), body(body) {}

	void accept(Visitor* visitor) override {
		visitor->visit_fn_stmt(this);
	}
};

class Return : public Stmt {
public:
	std::shared_ptr<Token> keyword;
	std::shared_ptr<Expr> val;

	Return(std::shared_ptr<Token> keyword, std::shared_ptr<Expr> val) : keyword(keyword), val(val) {}

	void accept(Visitor* visitor) override {
		visitor->visit_return_stmt(this);
	}
};

#endif