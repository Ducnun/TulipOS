#pragma once

#include <vector>
#include <string>
#include "tokens.h"
#include "io_interface.h"

class Parser {
public: 
    Parser(std::vector<Token> tokens, IOInterface& io) : tokens(tokens), _io(io){};
    ~Parser() = default;

    void parse(void);

private:
    std::vector<Token> tokens;
    IOInterface& _io;
};