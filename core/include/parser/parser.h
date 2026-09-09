#pragma once

#include <stdexcept>

#include "ast/expression.h"
#include "ast/statement.h"
#include "runtime/core/symbol_table.h"
#include "tokenizer/tokenizer.h"
#include "parser/parser_error.h"

class Parser{
public:
    Parser(SymbolTable& symTable) 
        : _symTable(symTable) {}

    ~Parser() = default;

    StmtPtr parseLine(Tokenizer& tokenizer, Arena& arena);

    bool hasError() const {
        return static_cast<bool>(_error);
    }

    const ParseError& error() const {
        return _error;
    }


private:
    // References to external state
    SymbolTable& _symTable;
    
    // State set per call to parseLine
    Tokenizer* _tokenizer = nullptr;
    Arena* _arena = nullptr;
    
    // The "Lookahead": In the pull model, we store the current and previous tokens
    Token _previous;
    Token _current;
    Token _next;

    // First parser error encountered.
    ParseError _error;

    struct ParsedCall {
        Symbol module = INVALID_SYMBOL;
        Symbol function = INVALID_SYMBOL;
        ExprPtr* arguments = nullptr;
        uint8_t argumentCount = 0;
    };

    void advance() {
        _previous = _current;
        _current = _next;

        // Do not keep requesting tokens after EOF.
        if (_current.type == TokenType::Eof) {
            _next = _current;
        } else {
            _next = _tokenizer->nextToken();
        }

        if (_current.type == TokenType::Error){
            fail(ParseErrorCode::UnexpectedToken, _current.value.data ? _current.value.data : "Tokenizer error");
        }
    }

    // Helper functions
    bool check(TokenType expected) const {
        return _current.type == expected;
    }

    bool checkNext(TokenType expected) const {
        return _next.type == expected;
    }

    bool isAtEnd() const {
        return _current.type == TokenType::Eof;
    }

    bool matchAny(std::initializer_list<TokenType> types) {
        if (hasError()) return false;
        for (auto type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    bool match(TokenType expected){
        if (!check(expected) || hasError()) return false;

        advance();
        return true;
    }

    bool consume(TokenType type, const char* message) {
        if (hasError()) {
            return false;
        }

        if (!check(type)) {
            fail(ParseErrorCode::UnexpectedToken, message);
            return false;
        }

        advance();
        return true;
    }

    void fail(ParseErrorCode code, const char* message) {
        // Preserve the first error. Subsequent errors are usually consequences
        // of the parser already being out of alignment.
        if (hasError()) {
            return;
        }

        _error.code = code;
        _error.token = _current;
        _error.message = message;
    }

    Symbol intern(util::StringView value) {
        Symbol symbol = _symTable.getOrCreate(value);

        if (symbol == INVALID_SYMBOL) {
            fail(
                ParseErrorCode::SymbolTableFull,
                "Symbol table or symbol arena is full"
            );
        }

        return symbol;
    }

    bool checkTokenizerError() {
        if (_current.type != TokenType::Error) return false;
        fail(ParseErrorCode::UnexpectedToken, _current.value.data ? _current.value.data : "Tokenizer error");
        return true;
    }

    bool parseCallTail(util::StringView firstName, ParsedCall& result);

    // statements
    StmtPtr statement();
    StmtPtr printStatement();
    StmtPtr letStatement();
    StmtPtr assignmentStatement();
    StmtPtr ifStatement();
    StmtPtr forStatement();
    StmtPtr nextStatement();
    StmtPtr gotoStatement();
    StmtPtr inputStatement();
    StmtPtr returnStatement();
    StmtPtr callStatement();
    StmtPtr blockStatement();

    // expressions
    ExprPtr expression();
    ExprPtr orExpr();
    ExprPtr andExpr();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();

    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        if (hasError()) {
            return nullptr;
        }

        // Arena::alloc must return memory suitably aligned for T.
        void* mem = _arena->alloc(sizeof(T));

        if (!mem) {
            fail(ParseErrorCode::OutOfMemory, "AST arena is full");
            return nullptr;
        }

        return new (mem) T(std::forward<Args>(args)...);
    }

    template<typename T>
    T* copyArray(const T* source, size_t count) {
        if (hasError() || count == 0) {
            return nullptr;
        }

        void* mem = _arena->alloc(sizeof(T) * count);

        if (!mem) {
            fail(ParseErrorCode::OutOfMemory, "AST arena is full");
            return nullptr;
        }

        // Only use this with trivially copyable types such as pointers, Symbols, and simple aggregate records.
        std::memcpy(mem, source, sizeof(T) * count);
        return static_cast<T*>(mem);
    }
};