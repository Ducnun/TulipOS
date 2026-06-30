#include "interpreter/basic_engine.h"

// Adds a line to the program
void BasicEngine::storeLine(int lineNumber, const std::string& source) {
    auto tokens = _tokenizer.tokenize(source);
    auto statements = _parser.parse(std::move(tokens));

    Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("Largest free block: %u bytes\n", ESP.getMaxAllocHeap());

    if (!statements.empty()){
        // We store the first statement and the source string
        _program.addLine(lineNumber, std::move(statements[0]), source);
    }
}

// Removes a line
void BasicEngine::removeLine(int lineNumber) {
    _program.removeLine(lineNumber);
}

// Executes a single line immediately
void BasicEngine::executeImmediate(const std::string& source) {
    auto tokens = _tokenizer.tokenize(source);
    auto statements = _parser.parse(std::move(tokens));

    for (auto& stmt : statements) {
        stmt->accept(_interpreter);
    }
}

// Runs the stored program
void BasicEngine::run() {
    if (_program.isEmpty()) return;
    _ctx.reset();

    auto pc = _program.begin();
    while (pc != _program.end() && !_ctx.halted) {
        _ctx.currentLine = pc->first;
        try {
            if (pc->second.statement) {
                pc->second.statement->accept(_interpreter);
            }
            pc++;
        } catch (const GotoSignal& g) {
            pc = _program.lower_bound(g.targetLine);
            if (pc == _program.end()) throw std::runtime_error("Line not found");
        } catch (const EndSignal& e) {
            break;
        }
    }
}

void BasicEngine::list() {
    for (auto const& line : _program) {
        _io.print(std::to_string(line.first) + " " + line.second.source + "\n");
    }
}

void BasicEngine::clear() {
    _program.clear();
    _ctx.reset();
}

void BasicEngine::loadProgram(const std::string& script) {
    _program.clear(); // Optional: clear existing program first
    
    std::stringstream ss(script);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (line.empty()) continue;

        // Find the first space to separate line number from code
        size_t firstSpace = line.find(' ');
        if (firstSpace != std::string::npos) {
            try {
                int lineNum = std::stoi(line.substr(0, firstSpace));
                std::string content = line.substr(firstSpace + 1);
                storeLine(lineNum, content);
            } catch (...) {
                // Skip lines that don't start with a valid number
                continue;
            }
        }
    }
}