#include "kernel/console_service.h"

std::string ConsoleService::readLine(const std::string& prompt){
    print(prompt);
    std::string line;
    _historyIndex = -1;

    while (true){
        _host.service();    // Let host run service tasks

        if (_keyboard->available()){
            KeyEvent ev = _keyboard->readEvent();

            switch (ev.action){
                // Typing
                case KeyAction::Char:{                    
                    switch (ev.character){
                        // End of line
                        case '\n': {
                            _history.push_back(line);
                            _terminal->newline();
                            return line;
                        }
                        // Backspace
                        case '\b': {
                            if (!line.empty()){
                                line.pop_back();
                                _terminal->backspace();
                            }
                            break;
                        }
                        default:{
                            line += ev.character;
                            _terminal->sendChar(ev.character);
                            break;
                        }
                    }    
                    break;
                }
                case KeyAction::ArrowUp:{
                    historyNav(line, true);
                    break;
                }
                case KeyAction::ArrowDown:{
                    historyNav(line, false);
                    break;
                }
                case KeyAction::Pause:{
                    _breakRequested = true;
                    break;
                }
                default:{
                    break;
                }
            }
        }
        else{
            _host.delay(1);     // No need to poll over 1 kHz
        }
    }
    return line;
}

void ConsoleService::print(const std::string& text){
    _terminal->sendString(text);
}

void ConsoleService::println(const std::string& text){
    _terminal->sendString(text);
    _terminal->newline();
}

void ConsoleService::clearScreen(){
    _terminal->clearScreen();
}

void ConsoleService::setCursor(int x, int y){
    _terminal->setCursor(x, y);
}

void ConsoleService::setColor(uint8_t color){
    _terminal->setColor(color);
}

void ConsoleService::setBackgroundColor(uint8_t color){
    _terminal->setBackground(color);
}

bool ConsoleService::checkAndResetBreak() {
    if (_breakRequested) {
        _breakRequested = false;
        return true;
    }
    return false;
}

/**
 * Helper functions
 */
// Updated currentline directly, handles indexing and redrawing
void ConsoleService::historyNav(std::string& currentLine, bool up) {
    if (_history.empty()) return;

    // 1. Clear current line visually
    for (size_t i = 0; i < currentLine.length(); i++) _terminal->backspace();

    // 2. Update Index
    if (up) {
        if (_historyIndex < (int)_history.size() - 1) _historyIndex++;
    } else {
        if (_historyIndex > -1) _historyIndex--;
    }

    // 3. Update Buffer
    if (_historyIndex == -1) currentLine = "";
    else currentLine = _history[_history.size() - 1 - _historyIndex];

    // 4. Print new line visually
    print(currentLine);
}