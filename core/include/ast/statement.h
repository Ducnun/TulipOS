#pragma once

#include <memory>
#include <vector>
#include <optional>

#include "ast/expression.h"
#include "runtime/core/identifier_id.h"

enum class PrintSeparator{
    None, Comma, Semicolon
};

struct PrintItem{
    ExprPtr expression = nullptr;
    PrintSeparator separator = PrintSeparator::None;

    PrintItem() = default;  // Allow inits in arrays

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
    IdentifierId variable;
    ExprPtr value;
    LetStmt(IdentifierId variable, ExprPtr value) : variable(variable), value(value) {}
    void accept(StmtVisitor&) const override;
};

struct AssignmentStmt : Stmt{
    IdentifierId variable;
    ExprPtr value;
    AssignmentStmt(IdentifierId variable, ExprPtr value) : variable(variable), value(value) {}
    void accept(StmtVisitor&) const override;
};

struct InputStmt : Stmt{
    IdentifierId promptId;
    bool hasPrompt = false;
    std::vector<IdentifierId> variables;
    InputStmt(IdentifierId promptId, bool hasPrompt, std::vector<IdentifierId> variables) : 
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
    IdentifierId variable;
    ExprPtr start;
    ExprPtr end;
    ExprPtr step;

    ForStmt(IdentifierId variable, ExprPtr start, ExprPtr end, ExprPtr step) : 
        variable(variable), start(start), end(end), step(step) {} 

    void accept(StmtVisitor&) const override;
};

struct NextStmt : Stmt{
    IdentifierId variable;
    NextStmt(IdentifierId variable) : variable(variable) {}
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
    IdentifierId module;
    IdentifierId function;
    std::vector<ExprPtr> args;
    CallStmt(IdentifierId module, IdentifierId function, std::vector<ExprPtr> args) :
        module(module), function(function), args(std::move(args)) {}
    void accept(StmtVisitor&) const override;
};

struct BlockStmt : Stmt{
    std::vector<StmtPtr> statements;
    BlockStmt(std::vector<StmtPtr> statements) : 
        statements(std::move(statements)) {}
    void accept(StmtVisitor&) const override;
};