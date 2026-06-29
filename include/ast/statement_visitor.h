#pragma once

#include "statement.h"

class StmtVisitor{
public:
    virtual ~StmtVisitor() = default;

    virtual void visit(const PrintStmt&) = 0;
    virtual void visit(const LetStmt&) = 0;
    virtual void visit(const AssignmentStmt&) = 0;
    virtual void visit(const InputStmt&) = 0;
    virtual void visit(const GotoStmt&) = 0;
    virtual void visit(const IfStmt&) = 0;
    virtual void visit(const ForStmt&) = 0;
    virtual void visit(const EndStmt&) = 0;
    virtual void visit(const ReturnStmt&) = 0;
    virtual void visit(const CallStmt&) = 0;
    virtual void visit(const BlockStmt&) = 0;
};