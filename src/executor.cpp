#include "executor.h"

void Executor::execute(const ParsedStatement& stmt){
    switch (stmt.type){
        case StatementType::Print: {
            ExpressionEvaluator evaluator(_variables);
            VariableValue result = evaluator.evaluate(stmt.printTokens);
            _io.print(result.toString() + "\n");
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
        default:
            break;
    }
}