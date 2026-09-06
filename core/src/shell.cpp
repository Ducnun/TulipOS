#include "system/shell.h"

void Shell::bootstrapEngine(){
    // 1. Ask the memory service for the raw blocks based on logical type
    void* progMem = _kernel.memory().alloc(PROGRAM_SIZE, MemoryType::Program);
    void* tranMem = _kernel.memory().alloc(TRANSIENT_SIZE, MemoryType::Transient);
    void* persMem = _kernel.memory().alloc(PERSISTANT_SIZE, MemoryType::Program); 

    // 2. Wrap them in Arenas
    // These Arenas now "live" in the correct physical banks automatically
    RuntimeResources res {
        .programArena = Arena(progMem, PROGRAM_SIZE),
        .transientArena = Arena(tranMem, TRANSIENT_SIZE),
        .persistentArena = ManagedArena(persMem, PERSISTANT_SIZE)
    };

    // 3. Start the engine
    _engine = new BasicEngine(_kernel, res);
}

void Shell::runREPL() {
    printWelcome();
    
    while (true) {
        std::string line = _kernel.console().readLine("> ");

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
            _kernel.console().print("? ERROR: ");
            _kernel.console().println(e.what());
        }
    }
}

void Shell::printWelcome(){
    _kernel.console().clearScreen();
    _kernel.console().println("**** TULIP OS V2 ****");
    _kernel.console().println("READY.");
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
        uint32_t start = _kernel.system().millis();
        _engine.run();
        uint32_t duration = _kernel.system().millis() - start;
        _kernel.console().print("\nExecution time: ");
        _kernel.console().print(std::to_string(duration));
        _kernel.console().print(" ms\n");
        
        _kernel.console().print("READY.\n");
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
        _kernel.storage().listDirectory();
        return true;
    }
    if (line.substr(0, 3) == "CD "){
        _kernel.storage().cd(line.substr(3).c_str());
        return true;
    }
    if (line.substr(0, 5) == "LOAD ") {
        handleLoad(line.substr(5));
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
        _kernel.console().print("OK");
        return true;
    }

    if (line == "CLS"){
        _kernel.console().clearScreen();
        return true;
    }

    return false;
}

void Shell::handleLoad(const std::string& path) {
    IFile* file = _kernel.storage().open(path.c_str());
    if (!file) return;

    // 1. Prepare Engine (Equivalent to 'NEW')
    _engine.clear(); 
    _kernel.console().print("\nLoading " + path + "...\n");

    // 2. Streaming Read
    std::string lineBuffer = "";
    uint8_t readBuf[256]; // Small chunks

    while (!file->eof()) {
        _kernel.system().yield(); // Crucial: Keep system alive
        
        if (_kernel.console().checkAndResetBreak()) {
            _kernel.console().println("\nLoad aborted by user.");
            break;
        }

        size_t n = file->read(readBuf, sizeof(readBuf));
        
        for (size_t i = 0; i < n; i++) {
            char c = (char)readBuf[i];
            
            if (c == '\n' || c == '\r') {
                if (!lineBuffer.empty()) {
                    // Send one line to the engine immediately
                    _engine.processRawLine(lineBuffer);
                    lineBuffer.clear();
                }
            } else {
                lineBuffer += c;
            }
        }
    }

    // Handle the last line if the file doesn't end with a newline
    if (!lineBuffer.empty()) {
        _engine.processRawLine(lineBuffer);
    }

    file->close();
    delete file;
    _kernel.console().print("READY.\n");
}