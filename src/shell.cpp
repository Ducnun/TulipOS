#include "system/shell.h"

Shell::Shell(IHost& host, StorageService& storage) : _host(host), _storage(storage), _engine(_host) {}

void Shell::runREPL() {
    printWelcome();

    while (true) {
        _host.print("> ");
        std::string line = _host.readLine();
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
            _host.print("? ERROR: ");
            _host.print(e.what());
            _host.print("\n");
        }
    }
}

void Shell::printWelcome(){
    _host.reset();
    _host.clearScreen();
    _host.print("**** TULIP BASIC V2 ****\n");
    
    #ifdef ESP32
        size_t freeHeap = PROGRAM_SIZE;
        size_t used = _engine.reportProgramSize();
        std::string memLine = std::to_string(freeHeap / 1024) + "K BYTES FREE (" +
                            std::to_string(used) + "B USED)\n";      
        _host.print(memLine);
    #endif
    _host.print("READY.\n");
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
        uint32_t start = _host.millis();
        _engine.run();
        uint32_t duration = _host.millis() - start;
        _host.print("\nExecution time: ");
        _host.print(std::to_string(duration));
        _host.print(" ms\n");
        
        _host.print("READY.\n");
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
    if (line == "DUMP"){
        _engine.DumpProgram();
        return true;
    }
    if (line == "LS"){
        _storage.ls(nullptr);
        return true;
    }
    if (line == "BENCHPI"){
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
    if (line == "BENCHPRIME") {
        std::string script = 
            "10 PRINT \"SEARCHING FOR PRIMES...\"\n"
            "20 LET C = 0\n"
            "30 FOR N = 2 TO 300\n"
            "40 LET P = 1\n"
            "50 FOR D = 2 TO N - 1\n"
            "60 IF MATH.INT(N / D) * D = N THEN LET P = 0\n" // Divisibility check
            "70 NEXT D\n"
            "80 IF P = 1 THEN LET C = C + 1\n"
            "90 NEXT N\n"
            "100 PRINT \"FOUND \": PRINT C: PRINT \" PRIMES\"\n";
        _engine.loadProgram(script);
        return true;
    }
    if (line == "BENCHSTR") {
        std::string script = 
            "10 LET A = \"*\"\n"
            "20 FOR I = 1 TO 1000\n"
            "30 LET A = A + \".\"\n"
            "40 NEXT I\n"
            "50 PRINT \"STRING LENGTH: \": PRINT 1000\n"
            "60 PRINT \"LAST CHAR: \": PRINT \".\"\n";
        _engine.loadProgram(script);
        return true;
    }
    if (line == "BENCHNEST") {
        std::string script = 
            "10 LET K = 0\n"
            "20 FOR I = 1 TO 20\n"
            "30 FOR J = 1 TO 20\n"
            "40 FOR L = 1 TO 20\n"
            "50 LET K = K + 1\n"
            "60 NEXT L\n"
            "70 NEXT J\n"
            "80 NEXT I\n"
            "90 PRINT \"TOTAL ITERATIONS: \": PRINT K\n";
        _engine.loadProgram(script);
        return true;
    }

    if (line == "EXIT" || line == "QUIT"){
        _host.print("OK");
        return true;
    }

    if (line == "CLS"){
        _host.clearScreen();
        return true;
    }

    return false;
}
