#include "executor.h"

ExecutionResult Executor::execute(const ParsedStatement& stmt){
    ExecutionResult execResult;

    switch (stmt.type){
        case StatementType::Print: {
            if (stmt.printTokens.empty()){
                _io.print("\n");
            }
            else{
                ExpressionEvaluator evaluator(_variables);
                VariableValue result = evaluator.evaluate(stmt.printTokens);
                _io.print(result.toString() + "\n");
            }
            break;
        }
        case StatementType::Assignment: {
            ExpressionEvaluator evaluator(_variables);
            VariableValue result = evaluator.evaluate(stmt.expressionTokens);
            _variables[stmt.identifier] = result;
            break;
        }
        case StatementType::IfThen: {
            ExpressionEvaluator evaluator(_variables);
            VariableValue conditionResult = evaluator.evaluate(stmt.conditionTokens);
            if (conditionResult.type != ValueType::Boolean){
                throw std::runtime_error("IF condition must be of boolean type");
            }
            
            if (conditionResult.boolValue){
                Parser parser(stmt.thenTokens);
                ParsedStatement thenStmt = parser.parse();
                execute(thenStmt);
            }
            break;
        }
        case StatementType::Goto: {
            ExpressionEvaluator evaluator(_variables);
            VariableValue result = evaluator.evaluate(stmt.expressionTokens);
            execResult.action = ExecutionAction::Jump;
            execResult.jumpTarget = result.intValue;
            break;
        }
        case StatementType::End: {
            execResult.action = ExecutionAction::Halt;
            break;
        }
        default:
            break;
    }
    return execResult;
}