#include "tokenizer/tokenizer.h"

const std::unordered_map<std::string, TokenType> Tokenizer::_keywords = {
    {"PRINT", TokenType::Print},
    {"LET", TokenType::Let},
    {"IF", TokenType::If},
    {"THEN", TokenType::Then},
    {"ELSE", TokenType::Else},
    {"GOTO", TokenType::Goto},
    {"INPUT", TokenType::Input},
    {"FOR", TokenType::For},
    {"TO", TokenType::To},
    {"STEP", TokenType::Step},
    {"NEXT", TokenType::Next},
    {"RUN", TokenType::Run},
    {"LIST", TokenType::List},
    {"NEW", TokenType::New},
    {"RETURN", TokenType::Return},
    {"END", TokenType::End},
    {"AND", TokenType::And},
    {"OR", TokenType::Or},
    {"NOT", TokenType::Not},
    {"MOD", TokenType::Mod},
    {"CALL", TokenType::Call},
    {"TRUE", TokenType::True},
    {"FALSE", TokenType::False},
};

void Tokenizer::reset(){
    _current = 0;
    _start = 0;
    _line = 1;
    _tokens.clear();
}

std::vector<Token>& Tokenizer::tokenize(std::string source){
    reset();
    _source = source;
    while (!isAtEnd()){
        _start = _current;
        scanToken();
    }
    addToken(TokenType::Eof);

    return _tokens;
}

void Tokenizer::scanToken(void){
    char c = advance();
    // Dispatching switch case
    switch(c){
        case ' ': break;
        case '\r': break;
        case '\t': break;
        case '\n': _line++; break;
        case '(': addToken(TokenType::LeftParen); break;
        case ')': addToken(TokenType::RightParen); break;
        case ',': addToken(TokenType::Comma); break;
        case ';': addToken(TokenType::Semicolon); break;
        case '+': addToken(TokenType::Plus); break;
        case '-': addToken(TokenType::Minus); break;
        case '*': addToken(TokenType::Star); break;
        case '/': addToken(TokenType::Slash); break;
        case '^': addToken(TokenType::Caret); break;
        case '=': addToken(TokenType::Equal); break;
        case '{': addToken(TokenType::LeftBrace); break;
        case '}': addToken(TokenType::RightBrace); break;
        case '.': addToken(TokenType::Dot); break;
        case '>': addToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater); break;
        case '<': 
            if (match('=')) addToken(TokenType::LessEqual);
            else if (match('>')) addToken(TokenType::NotEqual);
            else addToken(TokenType::Less);
            break;
        case '"': string(); break;
        default:
        {
            if (std::isdigit(c)){
                number();
            }
            else if (isAlpha(c)){
                identifier();
            }
            else{
                addToken(TokenType::Error, "Unexpected Token");
                advance();
            }
            break;
        }
    }
}

void Tokenizer::string(){
    while (peek() != '"' && !isAtEnd()){
        if (peek() == '\n') _line++;
        advance();
    }

    if (isAtEnd()){
        addToken(TokenType::Error, "Unterminated String");
        return;
    }

    advance();
    std::string value = _source.substr(_start + 1, _current - _start - 2);
    addToken(TokenType::String, value);
}

void Tokenizer::number(){
    while (std::isdigit(peek())) advance();

    if (peek() == '.' && std::isdigit(peekNext())){
        advance();
        while (std::isdigit(peek())) advance();
    }
    addToken(TokenType::Float, _source.substr(_start, _current - _start));
}

void Tokenizer::identifier(){
    while (isAlphaNumeric(peek())) advance();

    std::string lexeme = _source.substr(_start, _current - _start);
    auto it = _keywords.find(lexeme);

    if (it != _keywords.end()){
        addToken(it->second);
    }
    else{
        addToken(TokenType::Identifier, lexeme);
    }
}