#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include "tokens.h"

class Tokenizer {
public: 
    /**
     * @brief Destroy the Tokenizer object
     */
    ~Tokenizer() = default;

    /**
     * @brief tokenizes a cleaned input line
     * @param input Cleaned input
     * @return std::vector<Token> Vector of tokens in input order 
     */
    std::vector<Token> tokenize(const std::string& input);
};  