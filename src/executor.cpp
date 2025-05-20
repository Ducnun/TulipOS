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

ExecutionResult Executor::handleInput(const ParsedStatement& stmt){
    ExecutionResult execResult;

    // Display prompt
    if (!stmt.printTokens.empty()){
        EvalResult promptEval = safeEvaluate(stmt.printTokens, execResult);
        if (execResult.error) return execResult;

        _io.print(promptEval.value.toString());
        _io.print(" ");
    }
    else _io.print("? ");

    // Read input line
    std::string input = _io.readLine();
    std::istringstream iss(input);

    // Parse input for each variable
    for (size_t i = 0; i < stmt.identifierTokens.size(); i++){
        const Token& varToken = stmt.identifierTokens[i];
        std::string varName = varToken.value;

        // For string variables
        if (varName.back() == '$'){
            std::string value;
            if (i == 0){
                // First variable gets all input
                std::getline(iss, value);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t" + 1));
            }
            else{            // All subsequent variables need commas
                char comma;
                if (!(iss >> comma && comma == ',')){
                    execResult.error = true;
                    execResult.output = "?REDO FROM START";
                    return execResult;
                }
                iss >> std::ws;
                std::getline(iss, value);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
            }
            _variables[varName] = VariableValue::fromString(value);
        }
    }

    execResult.action = ExecutionAction::Wait;
    return execResult;
}