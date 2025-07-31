#include "CodeGenerator.h"

class StackSizeCalculator : public StmtVisitor
{
public:
    int count = 0;

    void calculate(const std::vector<std::unique_ptr<Stmt>>& statements) 
    { 
        for (const auto& stmt : statements) 
        { 
            stmt->accept(*this); 
        } 
    }
    void calculate(const Stmt& stmt) 
    { 
        stmt.accept(*this); 
    }
    void visitDeclarationStmt(const DeclarationStmt& /*stmt*/) override 
    { 
        count++; 
    }
    void visitIfStmt(const IfStmt& stmt) override 
    { 
        stmt.then_branch->accept(*this); 
    }
    void visitWhileStmt(const WhileStmt& stmt) override 
    { 
        stmt.body->accept(*this); 
    }
    void visitBlockStmt(const BlockStmt& stmt) override 
    { 
        for(const auto& s : stmt.statements) 
        {
            s->accept(*this); 
        }
    }
    void visitProcedureDeclStmt(const ProcedureDeclStmt& /*stmt*/) override {}
    void visitExpressionStmt(const ExpressionStmt& /*stmt*/) override {}
    void visitPrintStmt(const PrintStmt& /*stmt*/) override {}
    void visitNewlineStmt(const NewlineStmt& /*stmt*/) override {}
    void visitProcedureCallStmt(const ProcedureCallStmt& /*stmt*/) override {}
    void visitReturnStmt(const ReturnStmt& /*stmt*/) override {}
    void visitBreakStmt(const BreakStmt& /*stmt*/) override {}
};

class StringFinder : public StmtVisitor, public ExprVisitor 
{
public:
    std::vector<std::string> string_literals;

    void find(const std::vector<std::unique_ptr<Stmt>>& statements) 
    {
        for (const auto& stmt : statements) 
        {
            stmt->accept(*this);
        }
    }

    void visitPrintStmt(const PrintStmt& stmt) override 
    { 
        stmt.expression->accept(*this); 
    }
    void visitDeclarationStmt(const DeclarationStmt& stmt) override 
    { 
        stmt.initializer->accept(*this); 
    }
    void visitExpressionStmt(const ExpressionStmt& stmt) override 
    { 
        stmt.expression->accept(*this); 
    }
    void visitIfStmt(const IfStmt& stmt) override 
    { 
        stmt.condition->accept(*this); stmt.then_branch->accept(*this); 
    }
    void visitWhileStmt(const WhileStmt& stmt) override 
    { 
        stmt.condition->accept(*this); stmt.body->accept(*this); 
    }
    void visitBlockStmt(const BlockStmt& stmt) override 
    { 
        for(const auto& s : stmt.statements)
        { 
            s->accept(*this);
        } 
    }
    void visitNewlineStmt(const NewlineStmt& /*stmt*/) override {}
    void visitProcedureDeclStmt(const ProcedureDeclStmt& stmt) override 
    { 
        stmt.body->accept(*this); 
    }
    void visitProcedureCallStmt(const ProcedureCallStmt& stmt) override 
    {
        for(const auto& arg : stmt.arguments) 
        {
            arg->accept(*this);
        }
    }
    void visitReturnStmt(const ReturnStmt& stmt) override 
    {
        stmt.value->accept(*this);
    }
    void visitBreakStmt(const BreakStmt& /*stmt*/) override {}

    void visitLiteralExpr(const LiteralExpr& expr) override 
    {
        if (expr.value.type == TokenType::STRING_LITERAL) 
        {
            if (std::find(string_literals.begin(), string_literals.end(), expr.value.literal_value) == string_literals.end()) 
            {
                string_literals.push_back(expr.value.literal_value);
            }
        }
    }
    void visitBinaryExpr(const BinaryExpr& expr) override 
    { 
        expr.left->accept(*this); 
        expr.right->accept(*this); 
    }
    void visitComparisonExpr(const ComparisonExpr& expr) override 
    { 
        expr.left->accept(*this); 
        expr.right->accept(*this); 
    }
    void visitVariableExpr(const VariableExpr& /*expr*/) override {}
    void visitAssignExpr(const AssignExpr& expr) override 
    { 
        expr.value->accept(*this); 
    }
    void visitFunctionCallExpr(const FunctionCallExpr& expr) override 
    {
        for (const auto& arg : expr.arguments) 
        {
            arg->accept(*this);
        }
    }
    void visitUnaryExpr(const UnaryExpr& expr) override 
    {
        expr.right->accept(*this);
    }
};

void CodeGenerator::emit(const std::string& code) 
{ 
    std::cout << "    " << code << std::endl; 
}
void CodeGenerator::emitLabel(const std::string& label) 
{ 
    std::cout << label << ":" << std::endl; 
}
std::string CodeGenerator::newLabel() 
{ 
    return "L" + std::to_string(m_label_counter++); 
}

void CodeGenerator::enterScope() 
{
    m_scope_level++;
    m_symbol_scopes.emplace_back();
}

void CodeGenerator::exitScope() 
{
    m_scope_level--;
    m_symbol_scopes.pop_back();
}

VariableInfo* CodeGenerator::findVariable(const std::string& name) 
{
    for (int i = m_symbol_scopes.size() - 1; i >= 0; --i) 
    {
        if (m_symbol_scopes[i].count(name))
        {
            return &m_symbol_scopes[i].at(name);
        }
    }

    return nullptr;
}

void CodeGenerator::findStringLiterals(const std::vector<std::unique_ptr<Stmt>>& statements) 
{
    StringFinder finder;
    finder.find(statements);
    m_string_literals = finder.string_literals;
}

void CodeGenerator::generate(const std::vector<std::unique_ptr<Stmt>>& statements) 
{
    findStringLiterals(statements);
    std::cout << "section .rodata" << std::endl;
    emitLabel("NL");
    emit("db 10");
    for (size_t i = 0; i < m_string_literals.size(); ++i) 
    {
        std::cout << "  str" << i << ": db `" << m_string_literals[i] << "`, 0" << std::endl;
    }
    
    std::cout << "\nsection .bss" << std::endl;
    emitLabel("int_buffer");
    emit("resb 21");

    std::cout << "\nsection .text" << std::endl;
    std::cout << "; --- Helper Functions ---" << std::endl;
    emitLabel("_print_integer");
    emit("mov rdi, int_buffer + 20"); 
    emit("mov byte [rdi], 0"); 
    emit("dec rdi"); 
    emit("test rax, rax");
    emit("jns .utoa_loop"); 
    emit("neg rax"); 
    emit("mov r11, 1");
    emitLabel(".utoa_loop");
    emit("mov rdx, 0"); 
    emit("mov rbx, 10"); 
    emit("div rbx"); 
    emit("add dl, '0'");
    emit("mov [rdi], dl"); 
    emit("dec rdi"); 
    emit("test rax, rax"); 
    emit("jnz .utoa_loop");
    emit("cmp r11, 1"); 
    emit("jne .skip_minus"); 
    emit("mov byte [rdi], '-'"); 
    emit("dec rdi");
    emitLabel(".skip_minus");
    emit("inc rdi"); 
    emit("mov rsi, rdi"); 
    emit("mov rdx, int_buffer + 21"); 
    emit("sub rdx, rsi");
    emit("mov rax, 1"); 
    emit("mov rdi, 1"); 
    emit("syscall"); 
    emit("ret");
    
    emitLabel("_strlen");
    emit("xor rcx, rcx");
    emitLabel(".strlen_loop");
    emit("cmp byte [rdi + rcx], 0"); 
    emit("je .strlen_end"); 
    emit("inc rcx"); 
    emit("jmp .strlen_loop");
    emitLabel(".strlen_end");
    emit("mov rax, rcx"); 
    emit("ret");
    
    std::cout << "\n; --- Procedures ---" << std::endl;
    for (const auto& stmt : statements) 
    {
        if (auto proc_decl = dynamic_cast<const ProcedureDeclStmt*>(stmt.get())) 
        {
            proc_decl->accept(*this);
        }
    }

    std::cout << "\n; --- Main Program ---" << std::endl;
    std::cout << "global _start" << std::endl;
    emitLabel("_start");
    enterScope();
    m_stack_offset = 0;
    emit("push rbp");
    emit("mov rbp, rsp");

    StackSizeCalculator main_stack_calc;
    main_stack_calc.calculate(statements);
    int total_stack_size = main_stack_calc.count * 8;

    if (total_stack_size > 0) 
    {
        int aligned_size = (total_stack_size + 15) & ~15;
        emit("sub rsp, " + std::to_string(aligned_size));
    }

    for (const auto& stmt : statements) 
    {
        if (!dynamic_cast<const ProcedureDeclStmt*>(stmt.get())) 
        {
            stmt->accept(*this);
        }
    }

    emit("\n; Exit program");
    emit("mov rsp, rbp");
    emit("pop rbp");
    emit("mov rax, 60");
    emit("xor rdi, rdi");
    emit("syscall");
    exitScope();
}

void CodeGenerator::visitProcedureDeclStmt(const ProcedureDeclStmt& stmt) 
{
    enterScope();
    emitLabel("proc_" + stmt.name.text);
    emit("push rbp");
    emit("mov rbp, rsp");

    StackSizeCalculator proc_stack_calc;
    proc_stack_calc.calculate(*stmt.body);
    int local_stack_size = (stmt.params.size() + proc_stack_calc.count) * 8;
    
    if (local_stack_size > 0) 
    {
        int aligned_size = (local_stack_size + 15) & ~15;
        emit("sub rsp, " + std::to_string(aligned_size));
    }
    
    const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    m_stack_offset = 0;
    for(size_t i = 0; i < stmt.params.size(); ++i) 
    {
        m_stack_offset += 8;
        m_symbol_scopes.back()[stmt.params[i].name.text] = {m_stack_offset, stmt.params[i].type.text};
        emit("mov [rbp - " + std::to_string(m_stack_offset) + "], " + arg_regs[i]);
    }
    
    stmt.body->accept(*this);

    emit("mov rsp, rbp");
    emit("pop rbp");
    emit("ret");
    exitScope();
}

void CodeGenerator::visitProcedureCallStmt(const ProcedureCallStmt& stmt) 
{
    const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    if (stmt.arguments.size() > 6) 
    {
        throw std::runtime_error("More than 6 arguments are not supported.");
    }
    
    for (size_t i = 0; i < stmt.arguments.size(); ++i) 
    {
        stmt.arguments[i]->accept(*this);
        emit("mov " + std::string(arg_regs[i]) + ", rax");
    }

    emit("call proc_" + stmt.callee_name.text);
}

void CodeGenerator::visitBlockStmt(const BlockStmt& stmt) 
{
    for (const auto& statement : stmt.statements) 
    {
        statement->accept(*this);
    }
}

void CodeGenerator::visitIfStmt(const IfStmt& stmt) 
{
    std::string endIfLabel = newLabel();

    stmt.condition->accept(*this);
    
    emit("cmp rax, 0");
    emit("je " + endIfLabel);
    
    stmt.then_branch->accept(*this);
    emitLabel(endIfLabel);
}

void CodeGenerator::visitWhileStmt(const WhileStmt& stmt) 
{
    std::string startLabel = newLabel();
    std::string endLabel = newLabel();
    
    m_break_labels.push_back(endLabel);

    emitLabel(startLabel);
    
    stmt.condition->accept(*this);

    emit("cmp rax, 0");
    emit("je " + endLabel);
    
    stmt.body->accept(*this);
    emit("jmp " + startLabel);
    emitLabel(endLabel);

    m_break_labels.pop_back();
}

void CodeGenerator::visitPrintStmt(const PrintStmt& stmt) 
{
    std::string expr_type = "int";
    if (auto lit_expr = dynamic_cast<const LiteralExpr*>(stmt.expression.get())) 
    {
        if (lit_expr->value.type == TokenType::STRING_LITERAL) expr_type = "string";
        else if (lit_expr->value.type == TokenType::BOOL_LITERAL) expr_type = "bool";
    } 
    else if (auto var_expr = dynamic_cast<const VariableExpr*>(stmt.expression.get())) 
    {
        VariableInfo* var = findVariable(var_expr->name.text);
        if (var) 
        {
            expr_type = var->type;
        } 
        else 
        {
            throw std::runtime_error("Undeclared variable '" + var_expr->name.text + "' in print statement.");
        }
    }

    stmt.expression->accept(*this);

    if (expr_type == "string") 
    {
        emit("push rax");
        emit("mov rdi, rax");
        emit("call _strlen");
        emit("mov rdx, rax");
        emit("pop rsi");
        emit("mov rax, 1");
        emit("mov rdi, 1");
        emit("syscall");
    } 
    else 
    {
        emit("mov r11, 0");
        emit("call _print_integer");
    }
}

void CodeGenerator::visitNewlineStmt(const NewlineStmt& /*stmt*/) 
{
    emit("mov rax, 1");
    emit("mov rdi, 1");
    emit("mov rsi, NL");
    emit("mov rdx, 1");
    emit("syscall");
}

void CodeGenerator::visitDeclarationStmt(const DeclarationStmt& stmt) 
{
    if (m_symbol_scopes.back().count(stmt.name.text)) 
    {
        throw std::runtime_error("Variable '" + stmt.name.text + "' already declared in this scope.");
    }
    m_stack_offset += 8;
    m_symbol_scopes.back()[stmt.name.text] = {m_stack_offset, stmt.type.text};
    stmt.initializer->accept(*this);
    emit("mov [rbp - " + std::to_string(m_stack_offset) + "], rax");
}

void CodeGenerator::visitExpressionStmt(const ExpressionStmt& stmt) 
{
    stmt.expression->accept(*this);
}

void CodeGenerator::visitReturnStmt(const ReturnStmt& stmt) 
{
    stmt.value->accept(*this);
    emit("mov rsp, rbp");
    emit("pop rbp");
    emit("ret");
}

void CodeGenerator::visitBreakStmt(const BreakStmt& /*stmt*/) 
{
    if (m_break_labels.empty()) 
    {
        throw std::runtime_error("'the story ends at this moment' can only be used inside a loop.");
    }
    emit("jmp " + m_break_labels.back());
}

void CodeGenerator::visitAssignExpr(const AssignExpr& expr) 
{
    VariableInfo* var = findVariable(expr.name.text);
    if (!var) 
    {
        throw std::runtime_error("Undeclared variable '" + expr.name.text + "'.");
    }
    expr.value->accept(*this);
    emit("mov [rbp - " + std::to_string(var->offset) + "], rax");
}

void CodeGenerator::visitBinaryExpr(const BinaryExpr& expr) 
{
    expr.left->accept(*this);
    emit("push rax");
    expr.right->accept(*this);
    emit("pop rbx"); 
    
    if (expr.op.text == "plus")
    { 
        emit("add rax, rbx");
    }
    else if (expr.op.text == "minus") 
    {
        emit("sub rbx, rax");
        emit("mov rax, rbx");
    }
    else if (expr.op.text == "multiplied") 
    {
        emit("imul rax, rbx");
    }
    else if (expr.op.text == "divided") 
    { 
        emit("mov r8, rax");
        emit("mov rax, rbx");
        emit("cqo");
        emit("idiv r8"); 
    }
    else if (expr.op.text == "and")
    { 
        emit("and rax, rbx");
    }
    else if (expr.op.text == "or")
    { 
        emit("or rax, rbx");
    }
}

void CodeGenerator::visitComparisonExpr(const ComparisonExpr& expr) 
{
    expr.left->accept(*this);
    emit("push rax");
    expr.right->accept(*this);
    emit("pop rbx");

    emit("cmp rbx, rax");

    std::string op_text = expr.op.text;
    std::string set_instruction;
    if (op_text == "is equal to") 
    {
        set_instruction = "sete";
    }
    else if (op_text == "is greater than")
    { 
        set_instruction = "setg";
    }
    else if (op_text == "is less than")
    { 
        set_instruction = "setl";
    }
    else 
    {
        throw std::runtime_error("Unsupported comparison operator.");
    }

    emit(set_instruction + " al");
    emit("movzx rax, al");
}

void CodeGenerator::visitUnaryExpr(const UnaryExpr& expr) 
{
    expr.right->accept(*this);
    if (expr.op.text == "not") 
    {
        emit("xor rax, 1");
    }
}

void CodeGenerator::visitFunctionCallExpr(const FunctionCallExpr& expr) 
{
    const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    if (expr.arguments.size() > 6) 
    {
        throw std::runtime_error("More than 6 arguments are not supported.");
    }
    
    for (size_t i = 0; i < expr.arguments.size(); ++i) 
    {
        expr.arguments[i]->accept(*this);
        emit("mov " + std::string(arg_regs[i]) + ", rax");
    }

    emit("call proc_" + expr.callee_name.text);
}

void CodeGenerator::visitLiteralExpr(const LiteralExpr& expr) 
{
    if (expr.value.type == TokenType::INT_LITERAL) 
    {
        emit("mov rax, " + expr.value.literal_value);
    } 
    else if (expr.value.type == TokenType::BOOL_LITERAL) 
    {
        emit(std::string("mov rax, ") + (expr.value.text == "true" ? "1" : "0"));
    }
    else if (expr.value.type == TokenType::STRING_LITERAL) 
    {
        for(size_t i = 0; i < m_string_literals.size(); ++i) 
        {
            if (m_string_literals[i] == expr.value.literal_value) 
            {
                emit("mov rax, str" + std::to_string(i));
                return;
            }
        }
        throw std::runtime_error("Internal compiler error: string literal not found.");
    }
}

void CodeGenerator::visitVariableExpr(const VariableExpr& expr) 
{
    VariableInfo* var = findVariable(expr.name.text);
    if (!var)
    {
        throw std::runtime_error("Undeclared variable '" + expr.name.text + "'.");
    }
    
    emit("mov rax, [rbp - " + std::to_string(var->offset) + "]");
}