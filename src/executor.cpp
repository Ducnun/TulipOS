#include "executor.h"

ExecutionResult Executor::execute(const ParsedStatement& stmt){
    static std::unordered_map<StatementType, std::function<ExecutionResult(const ParsedStatement&)>> handlers = {
        {StatementType::Print, [this](const ParsedStatement& stmt) {return handlePrint(stmt);}},
        {StatementType::Assignment, [this](const ParsedStatement& stmt) {return handleAssignment(stmt);}},
        {StatementType::IfThen, [this](const ParsedStatement& stmt) {return handleIfThen(stmt);}},
        {StatementType::Goto, [this](const ParsedStatement& stmt) {return handleGoto(stmt);}},
        {StatementType::End, [this](const ParsedStatement& stmt) {return handleEnd(stmt);}},
        {StatementType::Input, [this](const ParsedStatement& stmt) {return handleInput(stmt);}},
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
        _io.print(eval.value->toString() + "\n");
    }

    return result;
}

ExecutionResult Executor::handleAssignment(const ParsedStatement& stmt){
    ExecutionResult result;
    EvalResult eval = safeEvaluate(stmt.expressionTokens, result);
    if (result.error) return result;

    const std::string& varName = stmt.identifier;
    char suffix = varName.empty() ? '\0' : varName.back();

    VariableType expectedType;
    if (suffix == '$') expectedType = VariableType::String;
    else if (suffix == '%') expectedType = VariableType::Integer;
    else expectedType = VariableType::Float;

    VariableType actualType = eval.value->getType();
    if (actualType != expectedType) {
        if (!(expectedType == VariableType::Float && actualType == VariableType::Integer)) {
            result.error = true;
            result.output = "?TYPE MISMATCH IN LET";
            result.action = ExecutionAction::Error;
            return result;
        }
    }

    _variables[varName] = (eval.value)->clone();
    return result;
}

ExecutionResult Executor::handleIfThen(const ParsedStatement& stmt){
    ExecutionResult result;
    EvalResult condition = safeEvaluate(stmt.conditionTokens, result);
    if (result.error) return result;

    if (!condition.value->toBool()) return result;

    Parser parser(stmt.thenTokens);
    ParsedStatement thenStmt = parser.parse();
    return execute(thenStmt);
}

ExecutionResult Executor::handleGoto(const ParsedStatement& stmt){
    ExecutionResult result;
    EvalResult eval = safeEvaluate(stmt.expressionTokens, result);
    if (!result.error) {
        if (eval.value->getType() != VariableType::Integer){
            result.error = true;
            result.output = "?TYPE MISMATCH IN GOTO";
            return result;
        }
        result.action = ExecutionAction::Jump;
        result.jumpTarget = std::stoi(eval.value->toString());
    }
    return result;
}

ExecutionResult Executor::handleEnd(const ParsedStatement& stmt){
    ExecutionResult result;
    result.action = ExecutionAction::Halt;
    return result;
}

ExecutionResult Executor::handleInput(const ParsedStatement& stmt){
    ExecutionResult result;

    // Display prompt
    if (!stmt.printTokens.empty()){
        EvalResult promptEval = safeEvaluate(stmt.printTokens, result);
        if (result.error) return result;

        _io.print(promptEval.value->toString());
        _io.print(" ");
    }
    else _io.print("? ");

    // Read input line
    std::string input = _io.readLine();
    std::istringstream iss(input);

    // Parse input for each variable
    for (size_t i = 0; i < stmt.identifierTokens.size(); ++i) {
        const std::string& varName = stmt.identifierTokens[i].value;
        char suffix = varName.empty() ? '\0' : varName.back();

        if (i > 0) {
            char comma;
            if (!(iss >> comma) || comma != ',') {
                result.error = true;
                result.output = "?REDO FROM START";
                return result;
            }
        }

        iss >> std::ws;

        try {
            if (suffix == '$') {
                std::string strValue;
                std::getline(iss, strValue);
                strValue.erase(0, strValue.find_first_not_of(" \t"));
                strValue.erase(strValue.find_last_not_of(" \t") + 1);
                _variables[varName] = std::unique_ptr<Variable>(new StringVariable(strValue));
            } else if (suffix == '%') {
                int intValue;
                if (!(iss >> intValue)) throw std::runtime_error("invalid integer");
                _variables[varName] = std::unique_ptr<Variable>(new IntVariable(intValue));
            } else {
                double floatValue;
                if (!(iss >> floatValue)) throw std::runtime_error("invalid float");
                _variables[varName] = std::unique_ptr<Variable>(new FloatVariable(floatValue));
            }
        } catch (...) {
            result.error = true;
            result.output = "?REDO FROM START";
            return result;
        }
    }

    result.action = ExecutionAction::Wait;
    return result;
}