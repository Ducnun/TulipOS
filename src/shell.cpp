#include "shell.h"

Shell::Shell(IOInterface &io) : _io(io), _engine(_io) {}

void Shell::runREPL() {
    printWelcome();

    while (true) {
        _io.print("> ");
        std::string line = _io.readLine();
        if (line.empty()) continue;

        // 1. Handle Built-in Shell Commands
        std::string upper = toUpper(line);
        if (handleREPLDirective(upper)) continue;

        // 2. Handle Program Storage or Immediate Execution
        try {
            if (std::isdigit(line[0])) {
                // Parse line number
                std::istringstream iss(line);
                int num;
                iss >> num;
                
                std::string rest;
                std::getline(iss, rest);
                
                if (rest.find_first_not_of(" \t\r\n") == std::string::npos) {
                    _engine.removeLine(num);
                } else {
                    _engine.storeLine(num, rest);
                }
            } else {
                _engine.executeImmediate(line);
            }
        } catch (const std::exception& e) {
            _io.print("? ERROR: ");
            _io.print(e.what());
            _io.print("\n");
        }
    }
}

void Shell::printWelcome(){
    _io.reset();
    _io.clearScreen();
    _io.print("**** TULIP BASIC V1 ****\n");
    
    #ifdef ESP32
        size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        size_t used = 296636 - freeHeap;
        std::string memLine = std::to_string(freeHeap / 1000) + "K BYTES FREE (" +
                            std::to_string(used) + "B USED)\n";      
        _io.print(memLine);
    #endif
    _io.print("READY.\n");
}

std::string Shell::toUpper(const std::string& line){
    std::string result;
    int charIndex = 0;
    
    while (charIndex < line.length()){
        if (!(line[charIndex] == '\n' || line[charIndex] == '\r')) {
            result += std::toupper(line[charIndex]);
        }
        charIndex++; 
    }
    return result;
}

bool Shell::handleREPLDirective(const std::string& line){    
    if (line == "RUN") {
        uint32_t start = millis();
        _engine.run();
        uint32_t duration = millis() - start;
        Serial.printf("\nExecution time: %u ms\n", duration);
        
        _io.print("READY.\n");
        return true;
    }
    if (line == "LIST") {
        _engine.list();
        return true;
    }
    if (line == "NEW") {
        _engine.clear();
        return true;
    }
    if (line == "BENCH"){
        std::string benchScript = 
            "10 PRINT \"PI APPROXIMATION\"\n"
            "20 LET PI = 0\n"
            "30 LET SG = 1\n"
            "40 FOR I = 0 TO 4999\n"
            "50 LET D = 2 * I + 1\n"
            "60 LET PI = PI + SG * (1.0 / D)\n"
            "70 LET SG = SG * -1\n"   // Note: Fixed the '=' here
            "80 NEXT I\n"
            "90 LET PI = PI * 4\n"
            "100 PRINT PI\n";
        _engine.loadProgram(benchScript);

        return true;
    }

    if (line == "EXIT" || line == "QUIT"){
        _io.print("OK");
        return true;
    }

    if (line == "CLS"){
        _io.clearScreen();
        return true;
    }

    return false;
}
