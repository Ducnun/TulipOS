#include "kernel/output_service.h"

#include "sdkconfig.h"

#include <cstdio>

void OutputService::print(util::StringView txt){
#if defined(CONFIG_COMPILER_OPTIMIZATION_DEBUG) && CONFIG_COMPILER_OPTIMIZATION_DEBUG
    std::printf("%.*s", static_cast<int>(txt.length), txt.data);
    std::fflush(stdout);
#endif
    _terminal.print(txt);
}

void OutputService::print(char c){
    if (c == '\n' || c == '\r'){
#if defined(CONFIG_COMPILER_OPTIMIZATION_DEBUG) && CONFIG_COMPILER_OPTIMIZATION_DEBUG
        std::printf("\r\n");
        std::fflush(stdout);
#endif
        _terminal.newline();
    }
    else if (c == '\b'){
#if defined(CONFIG_COMPILER_OPTIMIZATION_DEBUG) && CONFIG_COMPILER_OPTIMIZATION_DEBUG
        std::printf("\b \b");
        std::fflush(stdout);
#endif
        _terminal.backspace();
    }
    else{
#if defined(CONFIG_COMPILER_OPTIMIZATION_DEBUG) && CONFIG_COMPILER_OPTIMIZATION_DEBUG
        std::printf("%c", c);
        std::fflush(stdout);
#endif
        _terminal.sendChar(c);
    }
}

void OutputService::println(util::StringView txt){
    print(txt);
    print('\n');
}

void OutputService::clear(){
#if defined(CONFIG_COMPILER_OPTIMIZATION_DEBUG) && CONFIG_COMPILER_OPTIMIZATION_DEBUG
    std::printf("\033[2J\033[H");
    std::fflush(stdout);
#endif
    _terminal.clearScreen();
}

void OutputService::setColor(uint8_t fg, uint8_t bg){
    _terminal.setColor(fg);
    _terminal.setBackground(bg);
}