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
    consume(TokenType::RightBrace, "Expected '}' after body (Unterminated '{')");

    return util::make_unique<BlockStmt>(std::move(statements));
}

StmtPtr Parser::printStatement(){
    std::vector<PrintItem> items;
    consume(TokenType::Print, "Expected PRINT (Parser misaligned)");

    ExprPtr expr = expression();
    items.emplace_back(std::move(expr), PrintSeparator::None);

    while (match({TokenType::Comma, TokenType::Semicolon})) {
        TokenType sep = previous().type;
        PrintSeparator separator = sep == TokenType::Comma ? PrintSeparator::Comma : PrintSeparator::Semicolon;

        ExprPtr expr = expression();
        items.emplace_back(std::move(expr), separator);
    }

    return util::make_unique<PrintStmt>(std::move(items));
}

StmtPtr Parser::letStatement(){
    consume(TokenType::Let, "Expected LET (Parser misaligned)");
    Token var = consume(TokenType::Identifier, "Expected variable after LET");
    consume(TokenType::Equal, "Expected '=' after variable");
    ExprPtr value = expression();
    return util::make_unique<LetStmt>(var.value, std::move(value));
}

StmtPtr Parser::inputStatement(){
    std::string prompt;
    bool hasPrompt = false;
    std::vector<std::string> variables;
    consume(TokenType::Input, "Expected INPUT (Parser misaligned)");
    
    if (match({TokenType::String})) {
        prompt = previous().value;
        hasPrompt = true;
        consume(TokenType::Semicolon, "Expected ';' after prompt");
    }

    do{
        variables.push_back(consume(TokenType::Identifier, "Expected variable after ','").value);
    }while (match({TokenType::Comma}));

    return util::make_unique<InputStmt>(std::move(prompt), hasPrompt, std::move(variables));
}

StmtPtr Parser::gotoStatement(){
    consume(TokenType::Goto, "Expected GOTO (Parser misaligned)");
    ExprPtr target = expression();
    return util::make_unique<GotoStmt>(std::move(target));
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

    return util::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
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

    return util::make_unique<ForStmt>(std::move(variable), std::move(start), std::move(end), std::move(step));
}

StmtPtr Parser::nextStatement(){
    consume(TokenType::Next, "Expected NEXT (Parser misaligned)");
    std::string var = "";
    if (check(TokenType::Identifier)) {
        var = advance().value;
    }
    return util::make_unique<NextStmt>(std::move(var));
}

StmtPtr Parser::assignmentStatement() {
    // Identifier = Expression
    Token var = consume(TokenType::Identifier, "Expected variable name");
    consume(TokenType::Equal, "Expected '=' after variable");
    ExprPtr value = expression();
    return util::make_unique<AssignmentStmt>(var.value, std::move(value));
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

    return util::make_unique<CallStmt>(moduleName, functionName, std::move(args));
}

ExprPtr Parser::expression() {
    return orExpr();
}

ExprPtr Parser::orExpr() {
    ExprPtr expr = andExpr();
    while (match({TokenType::Or})) {
        Token op = previous();
        ExprPtr right = andExpr();
        expr = util::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::andExpr() {
    ExprPtr expr = equality();
    while (match({TokenType::And})) {
        Token op = previous();
        ExprPtr right = equality();
        expr = util::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::equality(){
    ExprPtr expr = comparison();
    while (match({TokenType::Equal, TokenType::NotEqual})){
        Token op = previous();
        ExprPtr right = comparison();
        expr = util::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

ExprPtr Parser::comparison(){
    ExprPtr expr = term();
    while (match({TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual})){
        Token op = previous();
        ExprPtr right = term();
        expr = util::make_unique<BinaryExpr>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
}

ExprPtr Parser::term(){
    ExprPtr expr = factor();
    while (match({TokenType::Plus, TokenType::Minus})){
        Token op = previous();
        ExprPtr right = factor();
        expr = util::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::factor(){
    ExprPtr expr = unary();
    while (match({TokenType::Star, TokenType::Slash})){
        Token op = previous();
        ExprPtr right = unary();
        expr = util::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::unary(){
    if (match({TokenType::Minus, TokenType::Not})){
        Token op = previous();
        ExprPtr val = unary();
        return util::make_unique<UnaryExpr>(op, std::move(val));
    }
    return primary();
}

ExprPtr Parser::primary(){
    if (match({TokenType::True}))    return util::make_unique<LiteralExpr>(Value(true));
    if (match({TokenType::False}))   return util::make_unique<LiteralExpr>(Value(false));

    if (match({TokenType::Integer, TokenType::Float})) {
        // match() calls advance() for us, so previous() is now the number token
        return util::make_unique<LiteralExpr>(Value(std::stof(previous().value)));
    }

    if (match({TokenType::String})) {
        return util::make_unique<LiteralExpr>(Value(previous().value));
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

            return util::make_unique<CallExpr>(module, function, std::move(args));
        }

        return util::make_unique<VariableExpr>(previous()); 
    }

    if (match({TokenType::LeftParen})) {
        ExprPtr expr = expression();
        consume(TokenType::RightParen, "Expected ')' after expression.");
        return util::make_unique<GroupingExpr>(std::move(expr));
    }

    throw std::runtime_error("Expected expression, found: " + peek().value);
}