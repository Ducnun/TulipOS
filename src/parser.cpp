#include "parser/parser.h"

std::vector<StmtPtr> Parser::parse(const std::vector<Token>& tokens){
    _tokens = std::move(tokens);
    _current = 0;
    std::vector<StmtPtr> _statements;

    while (!isAtEnd()){
        _statements.push_back(statement());

        while (match({TokenType::Colon})); 
    }

    return _statements;
}

StmtPtr Parser::statement(){
    switch (peek().type){
        case TokenType::Print: return printStatement();
        case TokenType::Let: return letStatement();
        case TokenType::Input: return inputStatement();
        case TokenType::Goto: return gotoStatement();
        case TokenType::If: return ifStatement();
        case TokenType::For: return forStatement();
        case TokenType::Next: return nextStatement();
        case TokenType::LeftBrace: return blockStatement();
        case TokenType::Call: advance(); return callStatement();
        case TokenType::Identifier:
            if (checkNext(TokenType::Equal)) return assignmentStatement();
            if (checkNext(TokenType::LeftParen) || checkNext(TokenType::Dot)) return callStatement();
            throw std::runtime_error("Unexpected identifier: " + peek().value);
        default:
            throw std::runtime_error("Unknown Statement: " + TokenUtils::typeToString(_tokens[_current].type));
    }
}

StmtPtr Parser::blockStatement(){
    consume(TokenType::LeftBrace, "Expected '{' (Parser misaligned)");
    std::vector<StmtPtr> statements; 

    while (!check(TokenType::RightBrace) && !isAtEnd()){
        statements.push_back(statement());
    }
    consume(TokenType::RightBrace, "Expected '}' after body (Unterminated '{')");;
    return allocate<BlockStmt>(std::move(statements));
}

StmtPtr Parser::printStatement(){
    std::vector<PrintItem> items;
    consume(TokenType::Print, "Expected PRINT (Parser misaligned)");

    ExprPtr expr = expression();
    items.emplace_back(expr, PrintSeparator::None);

    while (match({TokenType::Comma, TokenType::Semicolon})) {
        TokenType sep = previous().type;
        PrintSeparator separator = sep == TokenType::Comma ? PrintSeparator::Comma : PrintSeparator::Semicolon;

        ExprPtr expr = expression();
        items.emplace_back(expr, separator);
    }

    return allocate<PrintStmt>(std::move(items));
}

StmtPtr Parser::letStatement(){
    consume(TokenType::Let, "Expected LET (Parser misaligned)");
    Token var = consume(TokenType::Identifier, "Expected variable after LET");
    consume(TokenType::Equal, "Expected '=' after variable");
    ExprPtr value = expression();
    Symbol varId = _registry.getOrCreate(var.value);
    return allocate<LetStmt>(varId, value);
}

StmtPtr Parser::inputStatement(){
    std::string prompt;
    bool hasPrompt = false;
    std::vector<Symbol> varIds;
    consume(TokenType::Input, "Expected INPUT (Parser misaligned)");
    
    if (match({TokenType::String})) {
        prompt = previous().value;
        hasPrompt = true;
        consume(TokenType::Semicolon, "Expected ';' after prompt");
    }

    do{
        varIds.push_back(_registry.getOrCreate(consume(TokenType::Identifier, "Expected variable after ','").value));

    }while (match({TokenType::Comma}));

    Symbol promptId = _registry.getOrCreate(prompt);
    return allocate<InputStmt>(promptId, hasPrompt, varIds);
}

StmtPtr Parser::gotoStatement(){
    consume(TokenType::Goto, "Expected GOTO (Parser misaligned)");
    ExprPtr target = expression();
    return allocate<GotoStmt>(target);
}

StmtPtr Parser::ifStatement(){
    // Header
    consume(TokenType::If, "Expected IF (Parser misaligned)");
    ExprPtr condition = expression();
    consume(TokenType::Then, "Expected THEN after expression");
    StmtPtr thenBranch = statement();
    
    // If ELSE token available, consume and create else branch statement(s)
    StmtPtr elseBranch = nullptr;
    if (match({TokenType::Else})) elseBranch = statement();

    return allocate<IfStmt>(condition, thenBranch, elseBranch);
}

StmtPtr Parser::forStatement(){
    // Header
    consume(TokenType::For, "Expected FOR (Parser misaligned)");
    std::string variable = consume(TokenType::Identifier, "Expected variable after FOR").value;
    consume(TokenType::Equal, "Expected '=' after variable");
    ExprPtr start = expression();
    consume(TokenType::To, "Expected TO after expression");
    ExprPtr end = expression();

    // Optional Step expression
    ExprPtr step;
    if (match({TokenType::Step})) step = expression();
    else step = allocate<LiteralExpr>(Value::Float(1.0f));

    Symbol varId = _registry.getOrCreate(variable);
    return allocate<ForStmt>(varId, start, end, step);
}

StmtPtr Parser::nextStatement(){
    consume(TokenType::Next, "Expected NEXT (Parser misaligned)");
    std::string var = "";
    if (check(TokenType::Identifier)) {
        var = advance().value;
    }
    Symbol varId = _registry.getOrCreate(var);
    return allocate<NextStmt>(varId);
}

StmtPtr Parser::assignmentStatement() {
    // Identifier = Expression
    Token var = consume(TokenType::Identifier, "Expected variable name");
    consume(TokenType::Equal, "Expected '=' after variable");
    ExprPtr value = expression();
    Symbol varId = _registry.getOrCreate(var.value);
    return allocate<AssignmentStmt>(varId, value);
}

StmtPtr Parser::callStatement() {
    std::string moduleName = "";
    std::string functionName = consume(TokenType::Identifier, "Expected function name").value;

    // If there's a dot, the first identifier was the module
    if (match({TokenType::Dot})) {
        moduleName = functionName;
        functionName = consume(TokenType::Identifier, "Expected function name after '.'").value;
    }

    // Arguments
    consume(TokenType::LeftParen, "Expected '(' after function name");
    std::vector<ExprPtr> args;
    if (!check(TokenType::RightParen)) {
        do args.push_back(expression());
        while (match({TokenType::Comma}));
    }
    consume(TokenType::RightParen, "Expected ')' after arguments");

    Symbol moduleId = _registry.getOrCreate(moduleName);
    Symbol functionId = _registry.getOrCreate(functionName);
    return allocate<CallStmt>(moduleId, functionId, std::move(args));
}

ExprPtr Parser::expression() {
    return orExpr();
}

ExprPtr Parser::orExpr() {
    ExprPtr expr = andExpr();
    while (match({TokenType::Or})) {
        TokenType op = previous().type;
        ExprPtr right = andExpr();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::andExpr() {
    ExprPtr expr = equality();
    while (match({TokenType::And})) {
        TokenType op = previous().type;
        ExprPtr right = equality();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::equality(){
    ExprPtr expr = comparison();
    while (match({TokenType::Equal, TokenType::NotEqual})){
        TokenType op = previous().type;
        ExprPtr right = comparison();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::comparison(){
    ExprPtr expr = term();
    while (match({TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual})){
        TokenType op = previous().type;
        ExprPtr right = term();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::term(){
    ExprPtr expr = factor();
    while (match({TokenType::Plus, TokenType::Minus})){
        TokenType op = previous().type;
        ExprPtr right = factor();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::factor(){
    ExprPtr expr = unary();
    while (match({TokenType::Star, TokenType::Slash})){
        TokenType op = previous().type;
        ExprPtr right = unary();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::unary(){
    if (match({TokenType::Minus, TokenType::Not})){
        TokenType op = previous().type;
        ExprPtr val = unary();
        return allocate<UnaryExpr>(op, val);
    }
    return primary();
}

ExprPtr Parser::primary(){
    if (match({TokenType::True}))    return allocate<LiteralExpr>(Value::Bool(true));
    if (match({TokenType::False}))   return allocate<LiteralExpr>(Value::Bool(false));

    if (match({TokenType::Integer, TokenType::Float})) {
        // match() calls advance() for us, so previous() is now the number token
        return allocate<LiteralExpr>(Value::Float(std::stof(previous().value)));
    }

    if (match({TokenType::String})) {
        Symbol id = _registry.getOrCreate(previous().value);
        return allocate<LiteralExpr>(Value::PermanentStr(id, previous().value.length()));
    }

    if (match({TokenType::Identifier})) {
        Token nameToken = previous();

        // Check for a function call (module or otherwise) as an identifier (e.g. IF GPIO.read(2) THEN PRINT "PRESSED")
        if (check(TokenType::Dot) || check(TokenType::LeftParen)) {
            std::string module = "";
            std::string function = nameToken.value;

            if (match({TokenType::Dot})) {
                module = function;
                function = consume(TokenType::Identifier, "Expected function name after '.'").value;
            }

            consume(TokenType::LeftParen, "Expected '(' after function name");
            std::vector<ExprPtr> args;
            if (!check(TokenType::RightParen)) {
                do { args.push_back(expression()); } while (match({TokenType::Comma}));
            }
            consume(TokenType::RightParen, "Expected ')' after arguments");

            Symbol moduleId = _registry.getOrCreate(module);
            Symbol functionId = _registry.getOrCreate(function);
            return allocate<CallExpr>(moduleId, functionId, std::move(args));
        }

        return allocate<VariableExpr>(_registry.getOrCreate(previous().value)); 
    }

    if (match({TokenType::LeftParen})) {
        ExprPtr expr = expression();
        consume(TokenType::RightParen, "Expected ')' after expression.");
        return allocate<GroupingExpr>(expr);
    }

    throw std::runtime_error("Expected expression, found: " + peek().value);
}