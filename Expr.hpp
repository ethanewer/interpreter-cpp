#ifndef EXPR
#define EXPR

#include <vector>
#include "Token.hpp"
#include "Obj.hpp"
#include "Stmt.hpp"

class Stmt;
class Binary;
class Grouping;
class Literal;
class Logical;
class Unary;
class Variable;
class Assign;
class Call;
class LambdaExpr;

class Expr {
public: 
	class Visitor {
	public:
		virtual Obj* visit_binary_expr(Binary* expr) = 0;
        virtual Obj* visit_grouping_expr(Grouping* expr) = 0;
        virtual Obj* visit_literal_expr(Literal* expr) = 0;
        virtual Obj* visit_logical_expr(Logical* expr) = 0;
        virtual Obj* visit_unary_expr(Unary* expr) = 0;
        virtual Obj* visit_variable_expr(Variable* expr) = 0;
        virtual Obj* visit_assign_expr(Assign* expr) = 0;
        virtual Obj* visit_call_expr(Call* expr) = 0;
        virtual Obj* visit_lambda_expr(LambdaExpr* expr) = 0;
	};

	virtual Obj* accept(Visitor* visitor) = 0;

    virtual ~Expr() {}
};

class Binary : public Expr {
public:
    Expr* left;
    Token* op;
    Expr* right;
    
    Binary(Expr* left, Token* op, Expr* right) : left(left), op(op), right(right) {}

    Obj* accept(Visitor* visitor) override {
        return visitor->visit_binary_expr(this);
    }

    ~Binary() {
        if (left != nullptr) delete left;
        if (op != nullptr) delete op;
        if (right != nullptr) delete right;
    }
};

class Grouping : public Expr {
public:
    Expr* expression;
    
    Grouping(Expr* expression) : expression(expression) {}

    Obj* accept(Visitor* visitor) override {
        return visitor->visit_grouping_expr(this);
    }

    ~Grouping() {
        if (expression != nullptr) delete expression;
    }
};

class Literal : public Expr {
public:
    Obj* val;
    
    Literal(Obj* val) : val(val) {}

    Obj* accept(Visitor* visitor) override {
        return visitor->visit_literal_expr(this);
    }

    ~Literal() {
       if (val != nullptr && !val->has_ref) delete val;
    }
};

class Logical : public Expr {
public:
   Expr* left;
   Token* op;
   Expr* right;
    
    Logical(Expr* left, Token* op, Expr* right) : left(left), op(op), right(right) {}

    Obj* accept(Visitor* visitor) override {
        return visitor->visit_logical_expr(this);
    }

    ~Logical() {
       if (left != nullptr) delete left;
       if (op != nullptr) delete op;
       if (right != nullptr) delete right;
    }
};

class Unary : public Expr {
public:
    Token* op;
    Expr* right;
    
    Unary(Token* op, Expr* right) : op(op), right(right) {}

    Obj* accept(Visitor* visitor) override {
        return visitor->visit_unary_expr(this);
    }

    ~Unary() {
        if (op != nullptr) delete op;
        if (right != nullptr) delete right;
    }
};

class Variable : public Expr {
public:
    Token* name;
    
    Variable(Token* name) : name(name) {}

    Obj* accept(Visitor* visitor) override {
        return visitor->visit_variable_expr(this);
    }

    ~Variable() {
        if (name != nullptr) delete name;
    }
};

class Assign : public Expr {
public:
    Token* name;
    Expr* val;
    
    Assign(Token* name, Expr* val) : name(name), val(val) {}

    Obj* accept(Visitor* visitor) override {
        return visitor->visit_assign_expr(this);
    }

    ~Assign() {
        if (name != nullptr) delete name;
        if (val != nullptr) delete val;
    }
};

class Call : public Expr {
public:
    Expr* callee;
    Token* paren;
    std::vector<Expr*> arguments;
    
    Call(Expr* callee, Token* paren, std::vector<Expr*> arguments) 
        : callee(callee), paren(paren), arguments(arguments) {}

    Obj* accept(Visitor* visitor) override {
        return visitor->visit_call_expr(this);
    }

    ~Call() {
        if (callee != nullptr) delete callee;
        if (paren != nullptr) delete paren;
    }
};

class LambdaExpr : public Expr {
public:
	std::vector<Token*> params;
	std::vector<Stmt*> body;

	LambdaExpr(std::vector<Token*> params, std::vector<Stmt*> body)
	 : params(params), body(body) {}

	Obj* accept(Visitor* visitor) override {
		return visitor->visit_lambda_expr(this);
	}

	~LambdaExpr() {
		for (Token* p : params) {
			if (p != nullptr) delete p;
		}
		for (Stmt* s : body) {
			if (s != nullptr) delete s;
		}
	}
};

#endif