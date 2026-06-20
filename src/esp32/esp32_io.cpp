#include "esp32_io.h"

void ESP32IO::begin(){
    Serial.begin(115200);
    _terminal.begin();
    _keyboard.begin();
} 

void ESP32IO::print(const std::string& txt){
    Serial.print(txt.c_str());
    _terminal.sendString(txt);
}

std::string ESP32IO::readLine(){
    std::string line;
    while (true) {
        if (_keyboard.available()) {
            KeyEvent key = _keyboard.readEvent();

            if (key.action == KeyAction::None) continue;

            if (key.action == KeyAction::Char && key.character == '\n') {
                _history.push_back(line);
                Serial.println("");   
                _terminal.newline();
                break;
            }

            if (key.action == KeyAction::Char && key.character == '\b') {
                if (!line.empty()) {
                    line.pop_back();
                    Serial.print("\b \b");  // Erase last char
                    _terminal.backspace();
                }
                continue;
            }

            if (key.action == KeyAction::Pause){
                _breakRequested = true;
                return "";
            }

            if (key.action == KeyAction::Char){
                line += key.character;
                Serial.print(key.character);
                _terminal.sendChar(key.character);
            }

            if (key.action == KeyAction::ArrowUp){
                if (_history.empty()) continue;
                if (_historyIndex == -1){
                    _historyIndex = _history.size() - 1;
                    _currentLine = line;
                }
                else if (_historyIndex > 0){
                    _historyIndex--;
                }
                line = _history[_historyIndex];
                redrawLine(line);
                continue;
            }

            if (key.action == KeyAction::ArrowDown){
                if (_historyIndex == -1) continue;
                if (_historyIndex < _history.size() - 1){
                    _historyIndex++;
                    line = _history[_historyIndex];
                }
                else{
                    line = _currentLine;
                    _historyIndex = -1;
                }
                redrawLine(line);
                continue;
            }
        }
    }
    return line;
}

// Dumb implementation for now
void ESP32IO::redrawLine(std::string& line){
    Serial.print("\r");
    Serial.print("                                                  ");
    Serial.print("\r> ");
    Serial.print(line.c_str());
    _terminal.redrawLine("> " + line);
}

void ESP32IO::clearScreen(){
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    _terminal.clearScreen();
}

void ESP32IO::reset(){
    _terminal.resync();
}

void ESP32IO::setCursor(int x, int y){
    _terminal.setCursor(x, y);
}

bool ESP32IO::checkBreakRequested(){
    if (_breakRequested){
        _breakRequested = false;
        return true;
    }
    return false;
}

void ESP32IO::pollKeyboard(){
    if (_keyboard.available()){
        KeyEvent key = _keyboard.readEvent();
        if (key.action == KeyAction::Pause){
            _breakRequested = true;
        }
    }
}
