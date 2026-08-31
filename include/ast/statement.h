#pragma once

#include <memory>
#include <vector>
#include <optional>

#include "ast/expression.h"
#include "runtime/core/symbol_table.h"

enum class PrintSeparator{
    None, Comma, Semicolon
};

struct PrintItem{
    ExprPtr expression;
    PrintSeparator separator;

    PrintItem(ExprPtr expression, PrintSeparator separator) : 
        expression(expression), separator(separator) {} 
};

class StmtVisitor;

struct Stmt{
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

using StmtPtr = Stmt*;

struct PrintStmt : Stmt{
    std::vector<PrintItem> items;

    PrintStmt(std::vector<PrintItem> items) :
        items(std::move(items)) {}
    
    void accept(StmtVisitor&) const override;
};

struct LetStmt : Stmt{
    Symbol variable;
    ExprPtr value;
    LetStmt(Symbol variable, ExprPtr value) : variable(variable), value(value) {}
    void accept(StmtVisitor&) const override;
};

struct AssignmentStmt : Stmt{
    Symbol variable;
    ExprPtr value;
    AssignmentStmt(Symbol variable, ExprPtr value) : variable(variable), value(value) {}
    void accept(StmtVisitor&) const override;
};

struct InputStmt : Stmt{
    Symbol promptId;
    bool hasPrompt = false;
    std::vector<Symbol> variables;
    InputStmt(Symbol promptId, bool hasPrompt, std::vector<Symbol> variables) : 
        promptId(promptId), hasPrompt(hasPrompt), variables(variables) {}
    void accept(StmtVisitor&) const override;
};

struct GotoStmt : Stmt{
    ExprPtr target;
    GotoStmt(ExprPtr target) : target(target) {}
    void accept(StmtVisitor&) const override;
};

struct IfStmt : Stmt{
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;

    IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch = nullptr) :
        condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    void accept(StmtVisitor&) const override;
};

struct ForStmt : Stmt{
    Symbol variable;
    ExprPtr start;
    ExprPtr end;
    ExprPtr step;

    ForStmt(Symbol variable, ExprPtr start, ExprPtr end, ExprPtr step) : 
        variable(variable), start(start), end(end), step(step) {} 

    void accept(StmtVisitor&) const override;
};

struct NextStmt : Stmt{
    Symbol variable;
    NextStmt(Symbol variable) : variable(variable) {}
    void accept(StmtVisitor&) const override;
};

struct EndStmt : Stmt{
    void accept(StmtVisitor&) const override;
};

struct ReturnStmt : Stmt{
    ExprPtr value;
    ReturnStmt(ExprPtr value = nullptr) : value(value) {}
    void accept(StmtVisitor&) const override;
};

struct CallStmt : Stmt{
    Symbol module;
    Symbol function;
    std::vector<ExprPtr> args;
    CallStmt(Symbol module, Symbol function, std::vector<ExprPtr> args) :
        module(module), function(function), args(std::move(args)) {}
    void accept(StmtVisitor&) const override;
};

struct BlockStmt : Stmt{
    std::vector<StmtPtr> statements;
    BlockStmt(std::vector<StmtPtr> statements) : 
        statements(std::move(statements)) {}
    void accept(StmtVisitor&) const override;
};