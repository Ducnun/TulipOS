#include "parser/parser.h"

StmtPtr Parser::parseLine(Tokenizer& tokenizer, Arena& arena) {
    _tokenizer = &tokenizer;
    _arena = &arena;
    _error = {};

    _previous = {};
    _current = _tokenizer->nextToken();
    _next = _tokenizer->nextToken();

    if (checkTokenizerError()) return nullptr;
    if (isAtEnd()) {
        fail(ParseErrorCode::UnexpectedToken, "Expected statement");
        return nullptr;
    }

    StmtPtr result = statement();
    if (hasError() || !result) return nullptr;

    if (checkTokenizerError()) return nullptr;

    // If one parseLine() corresponds to exactly one statement, require EOF.
    if (!isAtEnd()) {
        fail(ParseErrorCode::UnexpectedToken, "Unexpected token after statement");
        return nullptr;
    }

    return result;
}

StmtPtr Parser::statement() {
    if (hasError()) {
        return nullptr;
    }

    switch (_current.type) {
        case TokenType::Print:      return printStatement();
        case TokenType::Let:        return letStatement();
        case TokenType::Input:      return inputStatement();
        case TokenType::Goto:       return gotoStatement();
        case TokenType::If:         return ifStatement();
        case TokenType::For:        return forStatement();
        case TokenType::Next:       return nextStatement();
        case TokenType::LeftBrace:  return blockStatement();
        case TokenType::Call:
            advance();
            return callStatement();

        case TokenType::Identifier:
            if (checkNext(TokenType::Equal)) {
                return assignmentStatement();
            }

            if (checkNext(TokenType::LeftParen) ||
                checkNext(TokenType::Dot)) {
                return callStatement();
            }

            fail(
                ParseErrorCode::UnexpectedToken,
                "Expected assignment or function call"
            );
            return nullptr;

        default:
            fail(ParseErrorCode::UnexpectedToken, "Expected statement");
            return nullptr;
    }
}

StmtPtr Parser::blockStatement() {
    if (!consume(TokenType::LeftBrace, "Expected '{'")) return nullptr;
    
    // Use a fixed-size stack buffer to collect statements
    StmtPtr tempBuffer[32]; 
    uint8_t count = 0;

    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (count >= 32) throw std::runtime_error("Block too large (max 32 stmts)");
        tempBuffer[count++] = statement();
    }
    if (!consume(TokenType::RightBrace, "Expected '}' after body")) return nullptr;

    if (hasError()) return nullptr;

    // Copy from stack to Code Arena
    StmtPtr* arenaStmts = (StmtPtr*)_arena->alloc(sizeof(StmtPtr) * count);
    memcpy(arenaStmts, tempBuffer, sizeof(StmtPtr) * count);

    return allocate<BlockStmt>(arenaStmts, count);
}

StmtPtr Parser::printStatement() {
    if (!consume(TokenType::Print, "Expected PRINT")) return nullptr;
    
    constexpr uint8_t MaxItems = 16;
    PrintItem temporary[MaxItems];
    uint8_t count = 0;

    ExprPtr first = expression();
    if (!first) return nullptr;

    temporary[count++] = {first, PrintSeparator::None};

    while (matchAny({TokenType::Comma, TokenType::Semicolon})) {
        TokenType separatorToken = _previous.type;

        if (count >= MaxItems) {
            fail(ParseErrorCode::TooManyItems, "Too many PRINT items");
            return nullptr;
        }

        ExprPtr item = expression();
        if (!item) return nullptr;

        PrintSeparator separator = (separatorToken == TokenType::Comma) ? PrintSeparator::Comma : PrintSeparator::Semicolon;
        temporary[count++] = {item, separator};
    }

    PrintItem* items = copyArray(temporary, count);

    if (!items) {
        return nullptr;
    }

    return allocate<PrintStmt>(items, count);
}

StmtPtr Parser::letStatement(){
    if (!consume(TokenType::Let, "Expected LET (Parser misaligned)")) return nullptr;
    if (!consume(TokenType::Identifier, "Expected identifier after LET")) return nullptr;
    util::StringView varName = _previous.value;
    if (!consume(TokenType::Equal, "Expected '=' after variable")) return nullptr;
    ExprPtr value = expression();
    Symbol varId = intern(varName);
    if (hasError()) return nullptr;

    return allocate<LetStmt>(varId, value);
}

StmtPtr Parser::inputStatement() {
    if (!consume(TokenType::Input, "Expected INPUT")) return nullptr;
    
    Symbol promptId = INVALID_SYMBOL;
    if (match(TokenType::String)) {
        promptId = intern(_previous.value);
        if (!consume(TokenType::Semicolon, "Expected ';' after prompt")) return nullptr;
    }

    Symbol tempIds[16];
    uint8_t count = 0;
    do {
        if (!consume(TokenType::Identifier, "Expected variable")) return nullptr;
        tempIds[count++] = intern(_previous.value);
    } while (match(TokenType::Comma) && count < 16);

    if (hasError()) return nullptr;

    // Allocate the ID array in the Symbol Arena (since it's small/permanent metadata)
    // or the Code Arena (since it's part of the AST). Let's use Code Arena.
    Symbol* arenaIds = (Symbol*)_arena->alloc(sizeof(Symbol) * count);
    memcpy(arenaIds, tempIds, sizeof(Symbol) * count);

    return allocate<InputStmt>(promptId, count > 0, arenaIds, count);
}

StmtPtr Parser::gotoStatement(){
    if (!consume(TokenType::Goto, "Expected GOTO (Parser misaligned)")) return nullptr;
    ExprPtr target = expression();

    if (hasError()) return nullptr;
    return allocate<GotoStmt>(target);
}

StmtPtr Parser::ifStatement(){
    // Header
    if (!consume(TokenType::If, "Expected IF (Parser misaligned)")) return nullptr;
    ExprPtr condition = expression();
    if (!consume(TokenType::Then, "Expected THEN after expression")) return nullptr;
    StmtPtr thenBranch = statement();
    
    // If ELSE token available, consume and create else branch statement(s)
    StmtPtr elseBranch = nullptr;
    if (match({TokenType::Else})) elseBranch = statement();

    if (hasError()) return nullptr;

    return allocate<IfStmt>(condition, thenBranch, elseBranch);
}

StmtPtr Parser::forStatement(){
    // Header
    if (!consume(TokenType::For, "Expected FOR (Parser misaligned)")) return nullptr;
    if (!consume(TokenType::Identifier, "Expected variable after FOR")) return nullptr;
    util::StringView variableName = _previous.value;
    if (!consume(TokenType::Equal, "Expected '=' after variable")) return nullptr;
    ExprPtr start = expression();
    if (!consume(TokenType::To, "Expected TO after expression")) return nullptr;
    ExprPtr end = expression();

    // Optional Step expression
    ExprPtr step;
    if (match({TokenType::Step})) step = expression();
    else step = allocate<LiteralExpr>(Value::Float(1.0f));

    Symbol variable = intern(variableName);
    if (hasError()) return nullptr;

    return allocate<ForStmt>(variable, start, end, step);
}

StmtPtr Parser::nextStatement(){
    consume(TokenType::Next, "Expected NEXT (Parser misaligned)");

    Symbol varId = INVALID_SYMBOL; // Default to no var
    if (check(TokenType::Identifier)) {
        advance();
        util::StringView varName = _previous.value;
        varId = intern(varName);
    }
    return allocate<NextStmt>(varId);
}

StmtPtr Parser::assignmentStatement() {
    // Identifier = Expression
    consume(TokenType::Identifier, "Expected variable name");
    util::StringView var = _previous.value;
    consume(TokenType::Equal, "Expected '=' after variable");
    ExprPtr value = expression();
    Symbol varId = intern(var);
    return allocate<AssignmentStmt>(varId, value);
}

StmtPtr Parser::callStatement() {
    if (!consume(TokenType::Identifier, "Expected function or module name")) return nullptr;

    util::StringView firstName = _previous.value;
    ParsedCall call;

    if (!parseCallTail(firstName, call)) return nullptr;

    return allocate<CallStmt>(call.module, call.function, call.arguments, call.argumentCount);
}

ExprPtr Parser::expression() {
    return orExpr();
}

ExprPtr Parser::orExpr() {
    ExprPtr expr = andExpr();
    while (match({TokenType::Or})) {
        TokenType op = _previous.type;
        ExprPtr right = andExpr();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::andExpr() {
    ExprPtr expr = equality();
    while (match({TokenType::And})) {
        TokenType op = _previous.type;
        ExprPtr right = equality();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::equality(){
    ExprPtr expr = comparison();
    while (matchAny({TokenType::Equal, TokenType::NotEqual})){
        TokenType op = _previous.type;
        ExprPtr right = comparison();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::comparison(){
    ExprPtr expr = term();
    while (matchAny({TokenType::Less, TokenType::LessEqual, TokenType::Greater, TokenType::GreaterEqual})){
        TokenType op = _previous.type;
        ExprPtr right = term();
        expr = allocate<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::term(){
    ExprPtr expr = factor();
    while (matchAny({TokenType::Plus, TokenType::Minus})){
        TokenType op = _previous.type;
        ExprPtr right = factor();
        if (!right) return nullptr;

        expr = allocate<BinaryExpr>(expr, op, right);
        if (!expr) return nullptr;
    }
    return expr;
}

ExprPtr Parser::factor(){
    ExprPtr expr = unary();
    while (matchAny({TokenType::Star, TokenType::Slash, TokenType::Mod})){
        TokenType op = _previous.type;
        ExprPtr right = unary();
        if (!right) return nullptr;

        expr = allocate<BinaryExpr>(expr, op, right);
        if (!expr) return nullptr;
    }
    return expr;
}

ExprPtr Parser::unary(){
    if (matchAny({TokenType::Minus, TokenType::Not})){
        TokenType op = _previous.type;
        ExprPtr val = unary();
        if (!val) return nullptr;
        return allocate<UnaryExpr>(op, val);
    }
    return primary();
}

ExprPtr Parser::primary() {
    if (match(TokenType::True)) return allocate<LiteralExpr>(Value::Bool(true));
    if (match(TokenType::False)) return allocate<LiteralExpr>(Value::Bool(false));

    if (matchAny({TokenType::Integer, TokenType::Float})) {
        // This is only safe if the token text is null-terminated.
        float value = strtof(_previous.value.data, nullptr);
        return allocate<LiteralExpr>(Value::Float(value));
    }

    if (match(TokenType::String)) {
        const uint32_t length = _previous.value.length;
        Symbol id = intern(_previous.value);

        if (hasError()) return nullptr;

        return allocate<LiteralExpr>(Value::PermanentStr(id, length));
    }

    if (match(TokenType::Identifier)) {
        util::StringView firstName = _previous.value;

        if (check(TokenType::LeftParen) || check(TokenType::Dot)) {
            ParsedCall call;
            if (!parseCallTail(firstName, call)) return nullptr;
            return allocate<CallExpr>(call.module, call.function, call.arguments, call.argumentCount);
        }

        Symbol variable = intern(firstName);

        if (hasError()) return nullptr;
        return allocate<VariableExpr>(variable);
    }

    if (match(TokenType::LeftParen)) {
        ExprPtr inner = expression();

        if (!inner) return nullptr;
        if (!consume(TokenType::RightParen, "Expected ')' after expression")) return nullptr;

        return allocate<GroupingExpr>(inner);
    }

    fail(ParseErrorCode::ExpectedExpression, "Expected expression");
    return nullptr;
}

bool Parser::parseCallTail(util::StringView firstName, ParsedCall& result) {
    util::StringView moduleName;
    util::StringView functionName = firstName;

    if (match(TokenType::Dot)) {
        moduleName = firstName;
        if (!consume(TokenType::Identifier, "Expected function name after '.'")) return false;
        functionName = _previous.value;
    }

    if (!consume(TokenType::LeftParen, "Expected '(' after function name")) return false;

    constexpr uint8_t MaxArguments = 8;
    ExprPtr temporaryArguments[MaxArguments];
    uint8_t argumentCount = 0;

    if (!check(TokenType::RightParen)) {
        while (true) {
            if (argumentCount >= MaxArguments) {
                fail(ParseErrorCode::TooManyItems, "Too many function arguments");
                return false;
            }

            ExprPtr argument = expression();
            if (!argument)  return false;
            temporaryArguments[argumentCount++] = argument;
            
            if (!match(TokenType::Comma)) break;
        }
    }

    if (!consume(TokenType::RightParen, "Expected ')' after arguments")) return false;

    // Do not intern the empty module name.
    if (moduleName.length != 0) {
        result.module = intern(moduleName);
        if (hasError()) return false;
    } else {
        result.module = INVALID_SYMBOL;
    }

    result.function = intern(functionName);

    if (hasError()) return false;

    result.argumentCount = argumentCount;
    result.arguments = copyArray(temporaryArguments, argumentCount);

    if (argumentCount != 0 && !result.arguments) return false;

    return true;
}