#include "interpreter/interpreter.h"

#include <cstdio>
#include <cstring>

namespace {
constexpr size_t MAX_CALL_ARGUMENTS = 8;
}

void Interpreter::visit(const LiteralExpr& expr) {
    _lastValue = expr.value;
}

void Interpreter::visit(const VariableExpr& expr) {
    Value* value = _ctx.get(expr.nameId);
    _lastValue = value ? *value : Value::Float(0.0f);
}

void Interpreter::visit(const BinaryExpr& expr) {
    const Value left = evaluate(expr.left);
    const Value right = evaluate(expr.right);

    switch (expr.op) {
        case TokenType::Plus:
            if (isAnyString(left) && isAnyString(right)) {
                const size_t length = static_cast<size_t>(left.length) + right.length;
                if (length >= 0xffffu) {
                    _lastValue = Value::Bool(false);
                    return;
                }
                char* result = static_cast<char*>(_ctx.transientArena.alloc(length + 1));
                if (!result) {
                    _lastValue = Value::Bool(false);
                    return;
                }
                std::memcpy(result, getString(left), left.length);
                std::memcpy(result + left.length, getString(right), right.length);
                result[length] = '\0';
                _lastValue = Value::TransientStr(result, static_cast<uint16_t>(length));
            } else if (left.type == ValueType::Number && right.type == ValueType::Number) {
                _lastValue = Value::Float(left.number + right.number);
            } else {
                _lastValue = Value::Bool(false);
            }
            break;
        case TokenType::Minus:
            _lastValue = left.type == ValueType::Number && right.type == ValueType::Number
                ? Value::Float(left.number - right.number) : Value::Bool(false);
            break;
        case TokenType::Star:
            _lastValue = left.type == ValueType::Number && right.type == ValueType::Number
                ? Value::Float(left.number * right.number) : Value::Bool(false);
            break;
        case TokenType::Slash:
            _lastValue = left.type == ValueType::Number && right.type == ValueType::Number &&
                         right.number != 0.0f
                ? Value::Float(left.number / right.number) : Value::Bool(false);
            break;
        case TokenType::Greater:
            _lastValue = Value::Bool(checkGreater(left, right));
            break;
        case TokenType::GreaterEqual:
            _lastValue = Value::Bool(checkGreater(left, right) || checkEqual(left, right));
            break;
        case TokenType::Less:
            _lastValue = Value::Bool(!checkGreater(left, right) && !checkEqual(left, right));
            break;
        case TokenType::LessEqual:
            _lastValue = Value::Bool(!checkGreater(left, right));
            break;
        case TokenType::Equal:
            _lastValue = Value::Bool(checkEqual(left, right));
            break;
        case TokenType::NotEqual:
            _lastValue = Value::Bool(!checkEqual(left, right));
            break;
        case TokenType::And:
            _lastValue = Value::Bool(left.isTruthy() && right.isTruthy());
            break;
        case TokenType::Or:
            _lastValue = Value::Bool(left.isTruthy() || right.isTruthy());
            break;
        default:
            _lastValue = Value::Bool(false);
            break;
    }
}

void Interpreter::visit(const UnaryExpr& expr) {
    const Value value = evaluate(expr.val);
    if (expr.op == TokenType::Minus && value.type == ValueType::Number) {
        _lastValue = Value::Float(-value.number);
    } else if (expr.op == TokenType::Not) {
        _lastValue = Value::Bool(!value.isTruthy());
    } else {
        _lastValue = Value::Bool(false);
    }
}

void Interpreter::visit(const CallExpr& expr) {
    if (expr.args.size() > MAX_CALL_ARGUMENTS) {
        _ctx.programIO.write("? Too many function arguments\n");
        _lastValue = Value::Bool(false);
        return;
    }

    Value arguments[MAX_CALL_ARGUMENTS];
    for (size_t i = 0; i < expr.args.size(); ++i) {
        arguments[i] = evaluate(expr.args[i]);
    }
    _lastValue = handleCall(expr.module, expr.function,
                            util::span<const Value>(arguments, expr.args.size()));
}

void Interpreter::visit(const GroupingExpr& expr) {
    _lastValue = evaluate(expr.expression);
}

void Interpreter::visit(const PrintStmt& stmt) {
    char numberBuffer[32];
    for (const PrintItem& item : stmt.items) {
        const Value value = evaluate(item.expression);
        if (isAnyString(value)) {
            _ctx.programIO.write(util::StringView(getString(value), value.length));
        } else if (value.type == ValueType::Number) {
            const int length = std::snprintf(numberBuffer, sizeof(numberBuffer), "%g",
                                             static_cast<double>(value.number));
            if (length > 0) {
                const size_t count = static_cast<size_t>(length) < sizeof(numberBuffer)
                    ? static_cast<size_t>(length) : sizeof(numberBuffer) - 1;
                _ctx.programIO.write(util::StringView(numberBuffer, count));
            }
        } else if (value.type == ValueType::Boolean) {
            _ctx.programIO.write(value.boolean ? "true" : "false");
        }

        if (item.separator == PrintSeparator::Comma) _ctx.programIO.write("\t");
    }
    _ctx.programIO.write("\n");
}

void Interpreter::visit(const LetStmt& stmt) {
    _ctx.assign(stmt.variable, evaluate(stmt.value));
}

void Interpreter::visit(const AssignmentStmt& stmt) {
    _ctx.assign(stmt.variable, evaluate(stmt.value));
}

void Interpreter::visit(const InputStmt& stmt) {
    if (stmt.variables.empty()) return;
    if (stmt.hasPrompt) _ctx.programIO.write(_symTable.getString(stmt.promptId));

    char input[128];
    if (!_ctx.programIO.readLine(input, sizeof(input))) {
        _ctx.halted = true;
        return;
    }

    const size_t length = std::strlen(input);
    const char* stored = _ctx.persistentArena.dupString(input, length, _ctx.variables);
    if (!stored) {
        _ctx.programIO.write("? Input buffer full\n");
        return;
    }
    _ctx.assign(stmt.variables[0], Value::PersistentStr(stored, static_cast<uint16_t>(length)));
}

void Interpreter::visit(const GotoStmt& stmt) {
    const Value target = evaluate(stmt.target);
    if (target.type != ValueType::Number) {
        _ctx.programIO.write("? GOTO requires a line number\n");
        return;
    }
    _ctx.jumpTarget = static_cast<int>(target.number);
    _ctx.jumpRequested = true;
}

void Interpreter::visit(const IfStmt& stmt) {
    if (evaluate(stmt.condition).isTruthy()) {
        if (stmt.thenBranch) stmt.thenBranch->accept(*this);
    } else if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
}

void Interpreter::visit(const ForStmt& stmt) {
    const bool isReentry = _ctx.forLoopDepth > 0 &&
        _ctx.forLoopStack[_ctx.forLoopDepth - 1].variable == stmt.variable;
    if (isReentry) return;

    const Value start = evaluate(stmt.start);
    const Value end = evaluate(stmt.end);
    const Value step = stmt.step ? evaluate(stmt.step) : Value::Float(1.0f);
    if (start.type != ValueType::Number || end.type != ValueType::Number ||
        step.type != ValueType::Number || step.number == 0.0f) {
        _ctx.programIO.write("? Invalid FOR range\n");
        _ctx.halted = true;
        return;
    }
    if (_ctx.forLoopDepth == RuntimeContext::FOR_LOOP_STACK_CAPACITY) {
        _ctx.programIO.write("? FOR nesting limit exceeded\n");
        _ctx.halted = true;
        return;
    }

    _ctx.assign(stmt.variable, start);
    _ctx.forLoopStack[_ctx.forLoopDepth++] = {
        stmt.variable, end.number, step.number, _ctx.currentLine
    };
}

void Interpreter::visit(const NextStmt& stmt) {
    if (_ctx.forLoopDepth == 0) {
        _ctx.programIO.write("? NEXT without FOR\n");
        return;
    }

    ForLoopFrame& frame = _ctx.forLoopStack[_ctx.forLoopDepth - 1];
    if (stmt.variable != INVALID_SYMBOL && stmt.variable != frame.variable) {
        _ctx.programIO.write("? NEXT variable does not match FOR\n");
        return;
    }

    Value* variable = _ctx.get(frame.variable);
    if (!variable || variable->type != ValueType::Number) {
        _ctx.programIO.write("? FOR variable is invalid\n");
        --_ctx.forLoopDepth;
        return;
    }

    variable->number += frame.stepValue;
    const bool continueLoop = frame.stepValue > 0.0f
        ? variable->number <= frame.endValue
        : variable->number >= frame.endValue;
    if (continueLoop) {
        _ctx.jumpTarget = frame.bodyStartLine;
        _ctx.jumpRequested = true;
    } else {
        --_ctx.forLoopDepth;
    }
}

Value Interpreter::handleCall(Symbol module, Symbol function, util::span<const Value> args) {
    const char* moduleName = _symTable.getString(module);
    const char* functionName = _symTable.getString(function);
    ModuleResult result{CallResult::NotHandled, Value::Bool(false)};

    if (std::strcmp(moduleName, "MATH") == 0) {
        result = _math.call(functionName, args);
    } else if (std::strcmp(moduleName, "GPIO") == 0) {
        result = _gpio.call(functionName, args);
    } else if (std::strcmp(moduleName, "SYS") == 0) {
        result = _sys.call(functionName, args);
    }

    if (result.result == CallResult::Error) {
        _ctx.programIO.write("? Module is unavailable\n");
        return Value::Bool(false);
    }
    if (result.result == CallResult::NotHandled) {
        _ctx.programIO.write("? Unknown module function\n");
        return Value::Bool(false);
    }
    return result.val;
}

void Interpreter::visit(const CallStmt& stmt) {
    if (stmt.args.size() > MAX_CALL_ARGUMENTS) {
        _ctx.programIO.write("? Too many function arguments\n");
        return;
    }

    Value arguments[MAX_CALL_ARGUMENTS];
    for (size_t i = 0; i < stmt.args.size(); ++i) {
        arguments[i] = evaluate(stmt.args[i]);
    }
    handleCall(stmt.module, stmt.function,
               util::span<const Value>(arguments, stmt.args.size()));
}

void Interpreter::visit(const BlockStmt& stmt) {
    for (const StmtPtr statement : stmt.statements) {
        if (_ctx.halted) break;
        if (statement) statement->accept(*this);
    }
}

void Interpreter::visit(const EndStmt&) {
    _ctx.halted = true;
}

void Interpreter::visit(const ReturnStmt& stmt) {
    _ctx.returnValue = stmt.value ? evaluate(stmt.value) : Value::Float(0.0f);
    _ctx.halted = true;
}
