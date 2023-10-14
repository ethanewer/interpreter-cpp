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
	Expr* expression;

	Expression(Expr* expression) : expression(expression) {}

	void accept(Visitor* visitor) override {
		visitor->visit_expression_stmt(this);
	}

	~Expression() {
		if (expression != nullptr) delete expression;
	}
};

class Print : public Stmt {
public:
	Expr* expression;

	Print(Expr* expression) : expression(expression) {}

	void accept(Visitor* visitor) override {
		visitor->visit_print_stmt(this);
	}

	~Print() {
		if (expression != nullptr) delete expression;
	}
};

class Var : public Stmt {
public:
	Token* name;
	Expr* initializer;
	
	Var(Token* name, Expr* initializer) : name(name),  initializer(initializer) {}

	void accept(Visitor* visitor) override {
		visitor->visit_var_stmt(this);
	}

	~Var() {
		if (initializer != nullptr) delete initializer;
		if (name != nullptr) delete name;
	}
};

class Block : public Stmt {
public:
	std::vector<Stmt*> stmts;

	Block(std::vector<Stmt*> stmts) : stmts(stmts) {}

	void accept(Visitor* visitor) override {
		visitor->visit_block_stmt(this);
	}

	~Block() {
		for (Stmt* s : stmts) {
			if (s != nullptr) delete s;
		}
	}
};

class If : public Stmt {
public:
	Expr* condition;
	Stmt* then_branch;
	Stmt* else_branch;

	If(Expr* condition, Stmt* then_branch, Stmt* else_branch) 
		: condition(condition), then_branch(then_branch), else_branch(else_branch) {}

	void accept(Visitor* visitor) override {
		visitor->visit_if_stmt(this);
	}

	~If() {
		if (condition != nullptr) delete condition;
		if (then_branch != nullptr) delete then_branch;
		if (else_branch != nullptr) delete else_branch;
	}
};

class While : public Stmt {
public:
	Expr* condition;
	Stmt* body;

	While(Expr* condition, Stmt* body) : condition(condition), body(body) {}

	void accept(Visitor* visitor) override {
		visitor->visit_while_stmt(this);
	}

	~While() {
		if (condition != nullptr) delete condition;
		if (body != nullptr) delete body;
	}
};

class FnStmt : public Stmt {
public:
	Token* name;
	std::vector<Token*> params;
	std::vector<Stmt*> body;

	FnStmt(Token* name, std::vector<Token*> params, std::vector<Stmt*> body)
	 : name(name), params(params), body(body) {}

	void accept(Visitor* visitor) override {
		visitor->visit_fn_stmt(this);
	}

	~FnStmt() {
		if (name != nullptr) delete name;
		for (Token* p : params) {
			if (p != nullptr) delete p;
		}
		for (Stmt* s : body) {
			if (s != nullptr) delete s;
		}
	}
};

class Return : public Stmt {
public:
	Token* keyword;
	Expr* val;

	Return(Token* keyword, Expr* val) : keyword(keyword), val(val) {}

	void accept(Visitor* visitor) override {
		visitor->visit_return_stmt(this);
	}
};

#endif