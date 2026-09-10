#include "tokenizer/tokenizer.h"

// Hardcoded keywords to avoid std::unordered_map
struct Keyword {
    const char* name;
    TokenType type;
};

static const Keyword KEYWORDS[] = {
    {"PRINT", TokenType::Print}, {"LET", TokenType::Let}, {"IF", TokenType::If},
    {"THEN", TokenType::Then}, {"ELSE", TokenType::Else}, {"GOTO", TokenType::Goto},
    {"INPUT", TokenType::Input}, {"FOR", TokenType::For}, {"TO", TokenType::To},
    {"STEP", TokenType::Step}, {"NEXT", TokenType::Next}, {"RUN", TokenType::Run},
    {"LIST", TokenType::List}, {"NEW", TokenType::New}, {"RETURN", TokenType::Return},
    {"END", TokenType::End}, {"AND", TokenType::And}, {"OR", TokenType::Or},
    {"NOT", TokenType::Not}, {"MOD", TokenType::Mod}, {"CALL", TokenType::Call},
    {"TRUE", TokenType::True}, {"FALSE", TokenType::False},
};

static const uint32_t KEYWORD_COUNT = sizeof(KEYWORDS) / sizeof(Keyword);

Token Tokenizer::nextToken(){
    skipWhitespace();
    uint32_t start = _cursor;

    if (isAtEnd()) return {TokenType::Eof, {nullptr, 0}, _line};

    char c = advance();

    if (isDigit(c)) {
        while (isDigit(peek())) advance();

        TokenType type = TokenType::Integer;
        if (peek() == '.' && isDigit(peekNext())) {
            type = TokenType::Float;
            advance();
            while (isDigit(peek())) advance();
        }

        return {type, {_source.data + start, _cursor - start}, _line};
    }

    if (isAlpha(c)) {
        while (isAlphaNumeric(peek())) advance();
        util::StringView lexeme{_source.data + start, _cursor - start};
        
        for (size_t i = 0; i < KEYWORD_COUNT; ++i) {
            if (lexeme.equals(KEYWORDS[i].name)) {
                return {KEYWORDS[i].type, lexeme, _line};
            }
        }
        return {TokenType::Identifier, lexeme, _line};
    }

    if (c == '"') {
        while (peek() != '"' && !isAtEnd()) advance();
        if (isAtEnd()) return {TokenType::Error, {"Unterminated String", 19}, _line};
        advance(); // Consume closing quote
        // Return view without the quotes
        return Token(TokenType::String, util::StringView(_source.data + start + 1, _cursor - start - 2), _line);
    }

    switch (c){
        case '(': return {TokenType::LeftParen, {_source.data + start, 1}, _line}; break;
        case ')': return {TokenType::RightParen,{_source.data + start, 1}, _line}; break;
        case '+': return {TokenType::Plus,      {_source.data + start, 1}, _line}; break;
        case '-': return {TokenType::Minus,     {_source.data + start, 1}, _line}; break;
        case '*': return {TokenType::Star,      {_source.data + start, 1}, _line}; break;
        case '/': return {TokenType::Slash,     {_source.data + start, 1}, _line}; break;
        case ':': return {TokenType::Colon,     {_source.data + start, 1}, _line}; break;
        case ';': return {TokenType::Semicolon, {_source.data + start, 1}, _line}; break;
        case ',': return {TokenType::Comma,     {_source.data + start, 1}, _line}; break;
        case '^': return {TokenType::Caret,     {_source.data + start, 1}, _line}; break;
        case '.': return {TokenType::Dot,       {_source.data + start, 1}, _line}; break;
        case '{': return {TokenType::LeftBrace, {_source.data + start, 1}, _line}; break;
        case '}': return {TokenType::RightBrace,{_source.data + start, 1}, _line}; break;
        case '=': return {TokenType::Equal,     {_source.data + start, 1}, _line}; break;
        case '>': {
            if (match('=')) return {TokenType::GreaterEqual, {_source.data + start, 2}, _line};
            return {TokenType::Greater, {_source.data + start, 1}, _line};
        }
        case '<': {
            if (match('=')) return {TokenType::LessEqual, {_source.data + start, 2}, _line};
            if (match('>')) return {TokenType::NotEqual, {_source.data + start, 2}, _line};
            return {TokenType::Less, {_source.data + start, 1}, _line};
        } 
    }

    return {TokenType::Error, "Unsupported Token at line: ", _line};
}