#include "ast/statement.h"
#include "ast/statement_visitor.h"

void PrintStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void LetStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void AssignmentStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void InputStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void GotoStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void IfStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void ForStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void EndStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void ReturnStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void CallStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}

void BlockStmt::accept(StmtVisitor& v) const {
    v.visit(*this);
}