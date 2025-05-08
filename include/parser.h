#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "tokens.h"
#include "io_interface.h"

class Parser {
public: 
    Parser(std::vector<Token> tokens, IOInterface& io, std::unordered_map<std::string, int>& variables) : _tokens(tokens), _io(io), _variables(variables){};
    ~Parser() = default;

    void parse(void);

private:
    std::vector<Token> _tokens;
    IOInterface& _io;
    std::unordered_map<std::string, int>& _variables;
};