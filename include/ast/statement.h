#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "ast/expression.h"

enum class PrintSeparator{
    None, Comma, Semicolon
};

struct PrintItem{
    ExprPtr expression;
    PrintSeparator separator;

    PrintItem(ExprPtr expression, PrintSeparator separator) : 
        expression(std::move(expression)), separator(separator) {} 
};

class StmtVisitor;

struct Stmt{
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) const = 0;
};

using StmtPtr = std::unique_ptr<Stmt>;

struct PrintStmt : Stmt{
    std::vector<PrintItem> items;

    PrintStmt(std::vector<PrintItem> items) :
        items(std::move(items)) {}
    
    void accept(StmtVisitor&) const override;
};

struct LetStmt : Stmt{
    std::string variable;
    ExprPtr value;

    LetStmt(std::string variable, ExprPtr value) :
        variable(std::move(variable)), value(std::move(value)) {}

    void accept(StmtVisitor&) const override;
};

struct AssignmentStmt : Stmt{
    std::string variable;
    ExprPtr value;

    AssignmentStmt(std::string variable, ExprPtr value) :
        variable(std::move(variable)), value(std::move(value)) {}

    void accept(StmtVisitor&) const override;
};

struct InputStmt : Stmt{
    std::string prompt;
    bool hasPrompt = false;
    std::vector<std::string> variables;

    InputStmt(std::string prompt, bool hasPrompt, std::vector<std::string> variables) : 
        prompt(std::move(prompt)), hasPrompt(hasPrompt), variables(std::move(variables)) {}

    void accept(StmtVisitor&) const override;
};

struct GotoStmt : Stmt{
    ExprPtr target;

    GotoStmt(ExprPtr target) : 
        target(std::move(target)) {}

    void accept(StmtVisitor&) const override;
};

struct IfStmt : Stmt{
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;

    IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch = nullptr) :
        condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void accept(StmtVisitor&) const override;
};

struct ForStmt : Stmt{
    std::string variable;
    ExprPtr start;
    ExprPtr end;
    ExprPtr step;

    ForStmt(std::string variable, ExprPtr start, ExprPtr end, ExprPtr step) : 
        variable(std::move(variable)), start(std::move(start)), end(std::move(end)), step(std::move(step)) {} 

    void accept(StmtVisitor&) const override;
};

struct NextStmt : Stmt{
    std::string variable = "";

    NextStmt(std::string variable) : variable(std::move(variable)) {}

    void accept(StmtVisitor&) const override;
};

struct EndStmt : Stmt{
    void accept(StmtVisitor&) const override;
};

struct ReturnStmt : Stmt{
    ExprPtr value;

    ReturnStmt(ExprPtr value = nullptr) :
        value(std::move(value)) {}

    void accept(StmtVisitor&) const override;
};

struct CallStmt : Stmt{
    std::string module;
    std::string function;
    std::vector<ExprPtr> args;

    CallStmt(std::string module, std::string function, std::vector<ExprPtr> args) :
        module(std::move(module)), function(std::move(function)), args(std::move(args)) {}
    
    void accept(StmtVisitor&) const override;
};

struct BlockStmt : Stmt{
    std::vector<StmtPtr> statements;

    BlockStmt(std::vector<StmtPtr> statements) : 
        statements(std::move(statements)) {}

    void accept(StmtVisitor&) const override;
};