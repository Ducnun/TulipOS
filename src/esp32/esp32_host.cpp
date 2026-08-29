#include "host/esp32_host.h"

void ESP32Host::begin(){
    Serial.begin(115200);
    _terminal.begin();
    _keyboard.begin();
    _sdCard.begin();
} 

void ESP32Host::service(){
    while (_keyboard.available()){
        auto event = _keyboard.readEvent();
        if (event.action == KeyAction::Pause) _breakRequested = true;
    }
}

void ESP32Host::print(const std::string& txt){
    Serial.print(txt.c_str());
    _terminal.sendString(txt.c_str());
}

std::string ESP32Host::readLine(){
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
void ESP32Host::redrawLine(std::string& line){
    Serial.print("\r");
    Serial.print("                                                  ");
    Serial.print("\r> ");
    Serial.print(line.c_str());
    _terminal.redrawLine("> " + line);
}

void ESP32Host::clearScreen(){
    Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    _terminal.clearScreen();
}

void ESP32Host::reset(){
    _terminal.resync();
}

void ESP32Host::setCursor(int x, int y){
    _terminal.setCursor(x, y);
}

bool ESP32Host::checkBreakRequested(){
    if (_breakRequested){
        _breakRequested = false;
        return true;
    }
    return false;
}

std::vector<StorageDescriptor> ESP32Host::getAvailableStorage(void){
    std::vector<StorageDescriptor> devices;
    // The host knows that on ESP32, we have an SD Card.
    devices.push_back({"SD", &_sdCard});
    return devices;
}

void ESP32Host::delay(uint32_t ms){
    uint32_t start = millis();

    while (::millis() - start < ms){
        service();
        if (_breakRequested) break;
    }
}

uint32_t ESP32Host::millis(){
    return ::millis();
}