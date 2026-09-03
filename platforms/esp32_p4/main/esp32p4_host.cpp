#include "esp32p4_host.h"
#include "esp_timer.h"
#include <unistd.h>

void ESP32P4Host::begin(){
    _kb.begin();
}

void ESP32P4Host::service() {
    static uint32_t count = 0;
    // Only yield every 1000 calls (tweak this number as needed)
    if (++count > 3000) {
        vTaskDelay(0);
        count = 0;
    }
}

void ESP32P4Host::print(const std::string& txt){
    printf(txt.c_str());
    fsync(STDOUT_FILENO);
}

std::string ESP32P4Host::readLine(){
    std::string line;
    while (true) {
        vTaskDelay(1);  // Yield when not actively reading
        if (_kb.available()) {
            KeyEvent key = _kb.readEvent();

            if (key.action == KeyAction::None) continue;

            if (key.action == KeyAction::Char && key.character == '\n') {
                _history.push_back(line);
                printf("\n");   
                _terminal.newline();
                break;
            }

            if (key.action == KeyAction::Char && key.character == '\b') {
                if (!line.empty()) {
                    line.pop_back();
                    write(STDOUT_FILENO, "\b \b", 3);
                    fsync(STDOUT_FILENO);
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
                write(STDOUT_FILENO, &key.character, 1);        // Low level write to VFS uart file
                fsync(STDOUT_FILENO);
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

void ESP32P4Host::redrawLine(std::string& line){

}

void ESP32P4Host::clearScreen(){

}

void ESP32P4Host::reset(){

}

void ESP32P4Host::setCursor(int x, int y){

}

bool ESP32P4Host::checkBreakRequested(){
    if (_breakRequested){
        _breakRequested = false;
        return true;
    }
    return false;
}

std::vector<StorageDescriptor> ESP32P4Host::getAvailableStorage(void){
    std::vector<StorageDescriptor> devices;
    // The host knows that on ESP32, we have an SD Card.
    devices.push_back({"SD", &_sdCard});
    return devices;
}

void ESP32P4Host::delay(uint32_t ms){
    vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t ESP32P4Host::millis(){
    return (uint32_t)(esp_timer_get_time() / 1000);
}