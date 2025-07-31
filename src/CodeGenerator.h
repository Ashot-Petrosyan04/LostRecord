#pragma once

#include "AST.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>

struct Stmt;

struct VariableInfo 
{
    int offset;
    std::string type;
};

class CodeGenerator : public ExprVisitor, public StmtVisitor 
{
public:
    void generate(const std::vector<std::unique_ptr<Stmt>>& statements);

    void visitBinaryExpr(const BinaryExpr& expr) override;
    void visitComparisonExpr(const ComparisonExpr& expr) override;
    void visitLiteralExpr(const LiteralExpr& expr) override;
    void visitVariableExpr(const VariableExpr& expr) override;
    void visitAssignExpr(const AssignExpr& expr) override;
    void visitFunctionCallExpr(const FunctionCallExpr& expr) override;
    void visitUnaryExpr(const UnaryExpr& expr) override;

    void visitDeclarationStmt(const DeclarationStmt& stmt) override;
    void visitExpressionStmt(const ExpressionStmt& stmt) override;
    void visitIfStmt(const IfStmt& stmt) override;
    void visitWhileStmt(const WhileStmt& stmt) override;
    void visitBlockStmt(const BlockStmt& stmt) override;
    void visitPrintStmt(const PrintStmt& stmt) override;
    void visitNewlineStmt(const NewlineStmt& stmt) override;
    void visitProcedureDeclStmt(const ProcedureDeclStmt& stmt) override;
    void visitProcedureCallStmt(const ProcedureCallStmt& stmt) override;
    void visitReturnStmt(const ReturnStmt& stmt) override;
    void visitBreakStmt(const BreakStmt& stmt) override;

private:
    void enterScope();
    void exitScope();
    VariableInfo* findVariable(const std::string& name);
    void findStringLiterals(const std::vector<std::unique_ptr<Stmt>>& statements);
    void emit(const std::string& code);
    void emitLabel(const std::string& label);
    std::string newLabel();
    
    std::vector<std::unordered_map<std::string, VariableInfo>> m_symbol_scopes;
    int m_scope_level = 0;
    int m_stack_offset = 0;

    int m_label_counter = 0;
    std::vector<std::string> m_string_literals;
    std::vector<std::string> m_break_labels;
};