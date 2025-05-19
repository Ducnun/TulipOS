#include "executor.h"

ExecutionResult Executor::execute(const ParsedStatement& stmt){
    static std::unordered_map<StatementType, std::function<ExecutionResult(const ParsedStatement&)>> handlers = {
        {StatementType::Print, [this](const ParsedStatement& stmt) {return handlePrint(stmt);}},
        {StatementType::Assignment, [this](const ParsedStatement& stmt) {return handleAssignment(stmt);}},
        {StatementType::IfThen, [this](const ParsedStatement& stmt) {return handleIfThen(stmt);}},
        {StatementType::Goto, [this](const ParsedStatement& stmt) {return handleGoto(stmt);}},
        {StatementType::End, [this](const ParsedStatement& stmt) {return handleEnd(stmt);}},
    };

    // Look for the right handler
    auto it = handlers.find(stmt.type);
    if (it != handlers.end()){
        ExecutionResult result = it->second(stmt);
        if (result.error && !result.output.empty()){
            _io.print(result.output + "\n");
        }
        return result;
    }

    return {};
}

EvalResult Executor::safeEvaluate(const std::vector<Token>& tokens, ExecutionResult& result){
    ExpressionEvaluator evaluator(_variables);
    EvalResult eval = evaluator.evaluate(tokens);
    if (eval.error){
        result.error = true;
        result.action = ExecutionAction::Error;
        result.output = eval.errorMsg;
    }
    return eval;
}

ExecutionResult Executor::handlePrint(const ParsedStatement& stmt){
    ExecutionResult result;
    if (stmt.printTokens.empty()){
        _io.print("\n");
        return result;
    }

    EvalResult eval = safeEvaluate(stmt.printTokens, result);
    if (!result.error){
        _io.print(eval.value.toString() + "\n");
    }

    return result;
}

ExecutionResult Executor::handleAssignment(const ParsedStatement& stmt){
    ExecutionResult result;
    EvalResult eval = safeEvaluate(stmt.expressionTokens, result);
    if (!result.error){
        _variables[stmt.identifier] = eval.value;
    }
    return result;
}

ExecutionResult Executor::handleIfThen(const ParsedStatement& stmt){
    ExecutionResult result;
    EvalResult condition = safeEvaluate(stmt.conditionTokens, result);
    if (result.error) return result;

    if (condition.value.type != ValueType::Boolean) {
        result.error = true;
        result.action = ExecutionAction::Error;
        result.output = "?ERROR: IF CONDITION MUST BE OF BOOLEAN TYPE";
        return result;
    }

    if (condition.value.boolValue) {
        Parser parser(stmt.thenTokens);
        ParsedStatement thenStmt = parser.parse();
        ExecutionResult nestedResult = execute(thenStmt);
        if (nestedResult.error) return nestedResult;
    }

    return result;
}

ExecutionResult Executor::handleGoto(const ParsedStatement& stmt){
    ExecutionResult result;
    EvalResult eval = safeEvaluate(stmt.expressionTokens, result);
    if (!result.error) {
        result.action = ExecutionAction::Jump;
        result.jumpTarget = eval.value.intValue;
    }
    return result;
}

ExecutionResult Executor::handleEnd(const ParsedStatement& stmt){
    ExecutionResult execResult;
    execResult.action = ExecutionAction::Halt;
    return execResult;
}