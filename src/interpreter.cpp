#include "interpreter.h"

BasicInterpreter::BasicInterpreter(IOInterface &io) : _io(io) {}

void BasicInterpreter::runREPL(){
    // Setup
    _io.clearScreen();
    _io.print("**** COMMODORE 64 BASIC V2 ****\n");
    _io.print(" 64K RAM SYSTEM 38911 BASIC BYTES FREE\n");
    _io.print("READY.\n");

    while (true){
        // Get new line
        _io.print("");
        _io.print("> ");
        std::string line = _io.readLine();
        
        // Clean line, if empty, skip
        std::string cleaned = preprocess(line);
        if (cleaned.empty()) continue;

        // If input is a REPL directive, skip rest
        if (handleREPLDirective(cleaned)) continue;

        // Store as program line if line starts with digits
        if (isdigit(cleaned[0])) {
            storeProgramLine(cleaned);
            continue;
        }

        // Run stored program when called
        if (cleaned == "RUN"){
            runProgram();
            continue;
        }

        // List stored program lines
        if (cleaned == "LIST"){
            listProgram();
            continue;
        }

        // Clean program when command NEW is entered in the terminal
        if (cleaned == "NEW"){
            clearProgram();
            continue;
        }

        // Call tokenizer and parser to process input
        evaluateImmediateCommand(cleaned);
    }
}

std::string BasicInterpreter::preprocess(const std::string& line){
    std::string result;
    for (char c : line){
        if (c == '\n' || c == '\r') continue;
        result += std::toupper(c);
    }
    return result;
}

bool BasicInterpreter::handleREPLDirective(const std::string& line){
    if (line == "EXIT" || line == "QUIT"){
        _io.print("OK");
        return true;
    }

    if (line == "CLS"){
        _io.clearScreen();
        return true;
    }

    if (line == "CLEAR"){
        clearVariables();
        return true;
    }

    return false;
}

void BasicInterpreter::storeProgramLine(const std::string& line){
    std::istringstream iss(line);
    int lineNumber;
    iss >> lineNumber;
    
    std::string rest;
    std::getline(iss, rest);
    std::string cleanedRest = preprocess(rest);
    
    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(cleanedRest);
    Parser parser(tokens);
    ParsedStatement stmt = parser.parse();

    stmt.lineNumber = lineNumber;
    _programLines[lineNumber] = ProgramLine{rest, stmt};
}

void BasicInterpreter::evaluateImmediateCommand(const std::string& line){
    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(line);
    Parser parser(tokens);
    ParsedStatement stmt = parser.parse();
    evaluateStatement(stmt);
}

void BasicInterpreter::runProgram(){
    for (const auto& pair : _programLines){
        const ParsedStatement& stmt = pair.second.statement;
        evaluateStatement(stmt);
    }
}

void BasicInterpreter::listProgram(){
    for (const auto& entry : _programLines){
        _io.print(std::to_string(entry.first) + " " + entry.second.source + "\n");
    }
}

void BasicInterpreter::evaluateStatement(const ParsedStatement& stmt){
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
                evaluateStatement(thenStmt);
            }
            break;
        }
        default:
            break;
    }
}

void BasicInterpreter::clearProgram(){
    _programLines.clear();
}

void BasicInterpreter::clearVariables(){
    _variables.clear();
}