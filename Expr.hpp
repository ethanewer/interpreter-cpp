#ifndef EXPR
#define EXPR

#include <vector>
#include <memory>
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
class Get;
class Set;
class This;

class Expr {
public: 
	class Visitor {
	public:
		virtual std::shared_ptr<Obj> visit_binary_expr(Binary* expr) = 0;
        virtual std::shared_ptr<Obj> visit_grouping_expr(Grouping* expr) = 0;
        virtual std::shared_ptr<Obj> visit_literal_expr(Literal* expr) = 0;
        virtual std::shared_ptr<Obj> visit_logical_expr(Logical* expr) = 0;
        virtual std::shared_ptr<Obj> visit_unary_expr(Unary* expr) = 0;
        virtual std::shared_ptr<Obj> visit_variable_expr(Variable* expr) = 0;
        virtual std::shared_ptr<Obj> visit_assign_expr(Assign* expr) = 0;
        virtual std::shared_ptr<Obj> visit_call_expr(Call* expr) = 0;
        virtual std::shared_ptr<Obj> visit_lambda_expr(LambdaExpr* expr) = 0;
        virtual std::shared_ptr<Obj> visit_get_expr(Get* expr) = 0;
        virtual std::shared_ptr<Obj> visit_set_expr(Set* expr) = 0;
        virtual std::shared_ptr<Obj> visit_this_expr(This* expr) = 0;
	};

	virtual std::shared_ptr<Obj> accept(Visitor* visitor) = 0;

    virtual ~Expr() {}
};

class Binary : public Expr {
public:
    std::shared_ptr<Expr> left;
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;
    
    Binary(std::shared_ptr<Expr> left, std::shared_ptr<Token> op, std::shared_ptr<Expr> right) : left(left), op(op), right(right) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_binary_expr(this);
    }
};

class Grouping : public Expr {
public:
    std::shared_ptr<Expr> expression;
    
    Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_grouping_expr(this);
    }
};

class Literal : public Expr {
public:
    std::shared_ptr<Obj> val;
    
    Literal(std::shared_ptr<Obj> val) : val(val) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_literal_expr(this);
    }
};

class Logical : public Expr {
public:
   std::shared_ptr<Expr> left;
   std::shared_ptr<Token> op;
   std::shared_ptr<Expr> right;
    
    Logical(std::shared_ptr<Expr> left, std::shared_ptr<Token> op, std::shared_ptr<Expr> right) : left(left), op(op), right(right) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_logical_expr(this);
    }
};

class Unary : public Expr {
public:
    std::shared_ptr<Token> op;
    std::shared_ptr<Expr> right;
    
    Unary(std::shared_ptr<Token> op, std::shared_ptr<Expr> right) : op(op), right(right) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_unary_expr(this);
    }
};

class Variable : public Expr {
public:
    std::shared_ptr<Token> name;
    
    Variable(std::shared_ptr<Token> name) : name(name) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_variable_expr(this);
    }
};

class Assign : public Expr {
public:
    std::shared_ptr<Token> name;
    std::shared_ptr<Expr> val;
    
    Assign(std::shared_ptr<Token> name, std::shared_ptr<Expr> val) : name(name), val(val) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_assign_expr(this);
    }
};

class Call : public Expr {
public:
    std::shared_ptr<Expr> callee;
    std::shared_ptr<Token> paren;
    std::vector<std::shared_ptr<Expr>> arguments;
    
    Call(std::shared_ptr<Expr> callee, std::shared_ptr<Token> paren, std::vector<std::shared_ptr<Expr>> arguments) 
        : callee(callee), paren(paren), arguments(arguments) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_call_expr(this);
    }
};

class LambdaExpr : public Expr {
public:
	std::vector<std::shared_ptr<Token>> params;
	std::vector<std::shared_ptr<Stmt>> body;

	LambdaExpr(std::vector<std::shared_ptr<Token>> params, std::vector<std::shared_ptr<Stmt>> body)
	    : params(params), body(body) {}

	std::shared_ptr<Obj> accept(Visitor* visitor) override {
		return visitor->visit_lambda_expr(this);
	}
};

class Get : public Expr {
public:
    std::shared_ptr<Expr> obj;
    std::shared_ptr<Token> name;
    
    Get(std::shared_ptr<Expr> obj, std::shared_ptr<Token> name) : obj(obj), name(name) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_get_expr(this);
    }
};

class Set : public Expr {
public:
    std::shared_ptr<Expr> obj;
    std::shared_ptr<Token> name;
    std::shared_ptr<Expr> val;
    
    Set(std::shared_ptr<Expr> obj, std::shared_ptr<Token> name, std::shared_ptr<Expr> val) 
        : obj(obj), name(name), val(val) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_set_expr(this);
    }
};

class This : public Expr {
public:
    std::shared_ptr<Token> keyword;
    
    This(std::shared_ptr<Token> keyword) : keyword(keyword) {}

    std::shared_ptr<Obj> accept(Visitor* visitor) override {
        return visitor->visit_this_expr(this);
    }
};

#endif