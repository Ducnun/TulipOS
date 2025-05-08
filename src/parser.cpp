#include "parser.h"

void Parser::parse(void){
    size_t i = 0;
    size_t tokenSize = tokens.size();

    while(i < tokenSize){
        Token& token = tokens[i];

        if (token.type == TokenType::Keyword && token.value == "PRINT"){
            i++;
            if (i < tokenSize && tokens[i].type == TokenType::String){
                _io.print(tokens[i].value);
                _io.print("\n");
            }
        }

        i++;
    }
}