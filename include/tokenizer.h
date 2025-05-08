#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include "tokens.h"

class Tokenizer {
public: 
    ~Tokenizer() = default;

    std::vector<Token> tokenize(const std::string& input);

};