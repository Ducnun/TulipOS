#include "interpreter/interpreter.h"

void Interpreter::visit(const LiteralExpr& expr){
    _lastValue = expr.value;
}

void Interpreter::visit(const VariableExpr& expr){
    Value* val = _ctx.get(expr.nameId);
    if (val) _lastValue = *val;
    else _lastValue = Value::Float(0.0f);
}

void Interpreter::visit(const BinaryExpr& expr){
    Value left = evaluate(expr.left);
    Value right = evaluate(expr.right);

    switch (expr.op){
        case TokenType::Plus:
            if (isAnyString(left) && isAnyString(right)){
                // Fetch copies of strings, regardless of current allocated arena
                const char* s1 = getString(left);
                const char* s2 = getString(right);
                size_t len = left.length + right.length + 1;

                // Write into transient arena
                char* buf = (char*)_ctx.res.tranArena.alloc(len);
                if (buf){
                    memcpy(buf, s1, left.length);
                    memcpy(buf + left.length, s2, right.length + 1);
                    _lastValue = Value::TransientStr(buf, len);
                }
            } else {
                _lastValue = Value::Float(left.number + right.number);
            }
            break;

        case TokenType::Minus: _lastValue = Value::Float(left.number - right.number); break;
        case TokenType::Star:  _lastValue = Value::Float(left.number * right.number); break;
        case TokenType::Slash: _lastValue = Value::Float(left.number / right.number); break;

        case TokenType::Greater:      _lastValue = Value::Bool(checkGreater(left, right)); break;
        case TokenType::GreaterEqual: _lastValue = Value::Bool(checkGreater(left, right) || checkEqual(left, right)); break;
        case TokenType::Less:         _lastValue = Value::Bool(!checkGreater(left, right) && !checkEqual(left, right)); break;
        case TokenType::LessEqual:    _lastValue = Value::Bool(!checkGreater(left, right)); break;
        case TokenType::Equal:        _lastValue = Value::Bool(checkEqual(left, right)); break;
        case TokenType::NotEqual:     _lastValue = Value::Bool(!checkEqual(left, right)); break;

        case TokenType::And: _lastValue = Value::Bool(left.isTruthy() && right.isTruthy()); break;
        case TokenType::Or:  _lastValue = Value::Bool(left.isTruthy() || right.isTruthy()); break;
        default: break;
    }
}

void Interpreter::visit(const UnaryExpr& expr){
    Value val = evaluate(expr.val);
    switch (expr.op) {
        case TokenType::Minus: _lastValue = Value::Float(-val.number); break;
        case TokenType::Not:   _lastValue = Value::Bool(!val.isTruthy()); break;
        default: break;
    }
}

void Interpreter::visit(const PrintStmt& stmt){
    for (const auto& item : stmt.items) {
        Value val = evaluate(item.expression);
        
        if (isAnyString(val)){
            _sys.console().print(getString(val));
        } else if (val.type == ValueType::Number) {
            _sys.console().print(std::to_string(val.number));
        } else if (val.type == ValueType::Boolean) {
            _sys.console().print(val.boolean ? "true" : "false");
        }

        if (item.separator == PrintSeparator::Comma) _sys.console().print("\t");
    }
    _sys.console().print("\n");
}

void Interpreter::visit(const LetStmt& stmt){
    Value val = evaluate(stmt.value);
    _ctx.assign(stmt.variable, val);
}

void Interpreter::visit(const AssignmentStmt& stmt){
    Value val = evaluate(stmt.value);
    _ctx.assign(stmt.variable, val);
}

void Interpreter::visit(const InputStmt& stmt) {
    if (stmt.hasPrompt) {
        _sys.console().print(_symTable.getString(stmt.promptId));
        _sys.console().print("\n");
    }
    std::string input = _sys.console().readLine("");

    // Wrap it as a Transient String
    Value inputVal = Value::TransientStr(input.c_str(), input.length());

    // _ctx.assign internally calls res.promoteValue(), 
    // which calls persArena.dupString() with all the correct root parameters.
    _ctx.assign(stmt.variables[0], inputVal);
}

void Interpreter::visit(const GotoStmt& stmt){
    _ctx.jumpTarget = (int)evaluate(stmt.target).number;
    _ctx.jumpRequested = true;
}

void Interpreter::visit(const IfStmt& stmt){
    if (evaluate(stmt.condition).isTruthy()) {
        stmt.thenBranch->accept(*this);
    } else if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
}

void Interpreter::visit(const ForStmt& stmt) {
    bool isReEntry = !_ctx.forLoopStack.empty() && 
                      _ctx.forLoopStack.back().variable == stmt.variable;

    if (!isReEntry) {
        float startVal = evaluate(stmt.start).number;
        float endVal = evaluate(stmt.end).number;
        float stepVal = stmt.step ? evaluate(stmt.step).number : 1.0f;
        
        _ctx.assign(stmt.variable, Value::Float(startVal));
        _ctx.forLoopStack.push_back({stmt.variable, endVal, stepVal, _ctx.currentLine});
    }
}

void Interpreter::visit(const NextStmt& stmt) {
    if (_ctx.forLoopStack.empty()) {
        printf("Error: NEXT without FOR\n");
        return;
    }

    ForLoopFrame& frame = _ctx.forLoopStack.back();
    
    // If a variable was provided (e.g., NEXT I), it MUST match.
    // If stmt.variable is INVALID_SYMBOL, we skip this check and match any loop.
    if (stmt.variable != INVALID_SYMBOL && stmt.variable != frame.variable) { 
        printf("NEXT Error: Variable mismatch. Expected ID %d, got %d\n", 
            frame.variable, stmt.variable);
        return; 
    }

    // Verify var exists or kill loop
    Value* var = _ctx.get(frame.variable);
    if (!var || var->type != ValueType::Number) {
        printf("NEXT Error: Variable didnt exist or was not of right type. Type: %d, Ptr: %f\n",
            (int)_ctx.get(frame.variable)->type, // Cast enum to int
            _ctx.get(stmt.variable)->number      // Use %f for double
        );
        _ctx.get(frame.variable)->type, _ctx.get(stmt.variable)->number;
        _ctx.forLoopStack.pop_back();
        return; 
    }

    // Use ptr directly to modify loop var
    var->number += frame.stepValue;
    float current = var->number;

    bool loopAgain = (frame.stepValue >= 0) ? (current <= frame.endValue) : (current >= frame.endValue);    
    if (loopAgain) {
        _ctx.jumpTarget = frame.bodyStartLine;
        _ctx.jumpRequested = true;
    } else {
        _ctx.forLoopStack.pop_back();
    }
}

Value Interpreter::handleCall(Symbol mod, Symbol func, const std::vector<Value>& args, RuntimeContext& ctx) {
    // Convert IDs back to strings only when calling native modules
    // const char* modName = _symTable.getString(mod);
    // const char* funcName = _symTable.getString(func);

    ModuleResult res;
    res.result = CallResult::NotHandled;

    // TODO: Use new sys api to build new modules

    if (res.result != CallResult::Success) return Value::Float(0.0f);
    else return res.val;
}

void Interpreter::visit(const CallStmt& stmt) {
    std::vector<Value> argValues;
    for (const auto& a : stmt.args) argValues.push_back(evaluate(a));
    handleCall(stmt.module, stmt.function, argValues, _ctx);
}

void Interpreter::visit(const CallExpr& expr) {
    std::vector<Value> argValues;
    for (const auto& a : expr.args) argValues.push_back(evaluate(a));
    _lastValue = handleCall(expr.module, expr.function, argValues, _ctx);
}

void Interpreter::visit(const GroupingExpr& expr){
    expr.expression->accept(*this);
}

void Interpreter::visit(const BlockStmt& stmt) {
    for (const auto& s : stmt.statements) {
        if (s) s->accept(*this);
    }
}

void Interpreter::visit(const EndStmt& expr){

}

void Interpreter::visit(const ReturnStmt& expr){

}

