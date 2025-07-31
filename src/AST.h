#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Token.h"

struct BinaryExpr;
struct ComparisonExpr;
struct LiteralExpr;
struct VariableExpr;
struct AssignExpr;
struct FunctionCallExpr;
struct UnaryExpr;
struct DeclarationStmt;
struct ExpressionStmt;
struct IfStmt;
struct WhileStmt;
struct BlockStmt;
struct PrintStmt;
struct NewlineStmt;
struct ProcedureDeclStmt;
struct ProcedureCallStmt;
struct ReturnStmt;
struct BreakStmt;

struct Param 
{
    Token name;
    Token type;
};

struct ExprVisitor 
{
    virtual void visitBinaryExpr(const BinaryExpr& expr) = 0;
    virtual void visitComparisonExpr(const ComparisonExpr& expr) = 0;
    virtual void visitLiteralExpr(const LiteralExpr& expr) = 0;
    virtual void visitVariableExpr(const VariableExpr& expr) = 0;
    virtual void visitAssignExpr(const AssignExpr& expr) = 0;
    virtual void visitFunctionCallExpr(const FunctionCallExpr& expr) = 0;
    virtual void visitUnaryExpr(const UnaryExpr& expr) = 0;
};

struct Expr {
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;
};

struct BinaryExpr : Expr 
{ 
    std::unique_ptr<Expr> left; 
    Token op; 
    std::unique_ptr<Expr> right; 
    BinaryExpr(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r) : left(std::move(l)), op(o), right(std::move(r)) {}
    void accept(ExprVisitor& v) const override 
    { 
        v.visitBinaryExpr(*this); 
    } 
};
struct ComparisonExpr : Expr 
{ 
    std::unique_ptr<Expr> left; 
    Token op; 
    std::unique_ptr<Expr> right; 
    ComparisonExpr(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r) : left(std::move(l)), op(o), right(std::move(r)) {}
    void accept(ExprVisitor& v) const override 
    { 
        v.visitComparisonExpr(*this); 
    } 
};
struct LiteralExpr : Expr 
{ 
    Token value; 
    explicit LiteralExpr(Token val) : value(val) {} 
    void accept(ExprVisitor& v) const override 
    { 
        v.visitLiteralExpr(*this); 
    } 
};
struct VariableExpr : Expr 
{ 
    Token name; 
    explicit VariableExpr(Token n) : name(n) {}
    void accept(ExprVisitor& v) const override 
    { 
        v.visitVariableExpr(*this); 
    } 
};
struct AssignExpr : Expr 
{
    Token name; 
    std::unique_ptr<Expr> value; 
    AssignExpr(Token n, std::unique_ptr<Expr> v) : name(n), value(std::move(v)) {} 
    void accept(ExprVisitor& v) const override 
    {
        v.visitAssignExpr(*this); 
    } 
};
struct FunctionCallExpr : Expr 
{ 
    Token callee_name; 
    std::vector<std::unique_ptr<Expr>> arguments; 
    FunctionCallExpr(Token n, std::vector<std::unique_ptr<Expr>> a) : callee_name(n), arguments(std::move(a)) {} 
    void accept(ExprVisitor& v) const override 
    { 
        v.visitFunctionCallExpr(*this); 
    } 
};
struct UnaryExpr : Expr 
{ 
    Token op; 
    std::unique_ptr<Expr> right; 
    UnaryExpr(Token o, std::unique_ptr<Expr> r) : op(std::move(o)), right(std::move(r)) {} 
    void accept(ExprVisitor& v) const override 
    { 
        v.visitUnaryExpr(*this); 
    } 
};

struct StmtVisitor {
    virtual void visitDeclarationStmt(const DeclarationStmt& stmt) = 0;
    virtual void visitExpressionStmt(const ExpressionStmt& stmt) = 0;
    virtual void visitIfStmt(const IfStmt& stmt) = 0;
    virtual void visitWhileStmt(const WhileStmt& stmt) = 0;
    virtual void visitBlockStmt(const BlockStmt& stmt) = 0;
    virtual void visitPrintStmt(const PrintStmt& stmt) = 0;
    virtual void visitNewlineStmt(const NewlineStmt& stmt) = 0;
    virtual void visitProcedureDeclStmt(const ProcedureDeclStmt& stmt) = 0;
    virtual void visitProcedureCallStmt(const ProcedureCallStmt& stmt) = 0;
    virtual void visitReturnStmt(const ReturnStmt& stmt) = 0;
    virtual void visitBreakStmt(const BreakStmt& stmt) = 0;
};

struct Stmt {
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

struct BlockStmt : Stmt 
{ 
    std::vector<std::unique_ptr<Stmt>> statements; 
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> s) : statements(std::move(s)) {} 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitBlockStmt(*this); 
    } 
};
struct DeclarationStmt : Stmt 
{ 
    Token name; 
    Token type; 
    std::unique_ptr<Expr> initializer;
    bool is_mutable; 
    DeclarationStmt(Token n, Token t, std::unique_ptr<Expr> i, bool m) : name(n), type(t), initializer(std::move(i)), is_mutable(m) {} 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitDeclarationStmt(*this);
    }
};
struct ExpressionStmt : Stmt 
{ 
    std::unique_ptr<Expr> expression; 
    explicit ExpressionStmt(std::unique_ptr<Expr> e) : expression(std::move(e)) {} 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitExpressionStmt(*this); 
    }
};
struct IfStmt : Stmt 
{ 
    std::unique_ptr<Expr> condition; 
    std::unique_ptr<Stmt> then_branch; 
    IfStmt(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> t) : condition(std::move(c)), then_branch(std::move(t)) {} 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitIfStmt(*this); 
    } 
};
struct WhileStmt : Stmt 
{
    std::unique_ptr<Expr> condition; 
    std::unique_ptr<Stmt> body; 
    WhileStmt(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> b) : condition(std::move(c)), body(std::move(b)) {} 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitWhileStmt(*this); 
    } 
};
struct PrintStmt : Stmt 
{ 
    std::unique_ptr<Expr> expression; 
    explicit PrintStmt(std::unique_ptr<Expr> e) : expression(std::move(e)) {} 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitPrintStmt(*this); 
    } 
};
struct NewlineStmt : Stmt 
{ 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitNewlineStmt(*this); 
    } 
};
struct ProcedureCallStmt : Stmt 
{ 
    Token callee_name; 
    std::vector<std::unique_ptr<Expr>> arguments; 
    ProcedureCallStmt(Token n, std::vector<std::unique_ptr<Expr>> a) : callee_name(n), arguments(std::move(a)) {} 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitProcedureCallStmt(*this); 
    } 
};
struct ProcedureDeclStmt : Stmt 
{ 
    Token name; 
    std::vector<Param> params; 
    Token return_type; 
    std::unique_ptr<Stmt> body; 
    ProcedureDeclStmt(Token n, std::vector<Param> p, Token rt, std::unique_ptr<Stmt> b) : name(n), params(std::move(p)), return_type(rt), body(std::move(b)) {} 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitProcedureDeclStmt(*this); 
    } 
};
struct ReturnStmt : Stmt 
{ 
    std::unique_ptr<Expr> value; 
    explicit ReturnStmt(std::unique_ptr<Expr> v) : value(std::move(v)) {} 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitReturnStmt(*this); 
    } 
};
struct BreakStmt : Stmt 
{ 
    void accept(StmtVisitor& v) const override 
    { 
        v.visitBreakStmt(*this); 
    } 
};