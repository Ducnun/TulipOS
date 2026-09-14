// #include "engine/basic_engine.h"

// void BasicEngine::storeLine(int lineNumber, const std::string& source){
//     auto tokens = _tokenizer.tokenize(source);
//     auto statements = _parser.parse(std::move(tokens));
//     if (statements.empty()) return;

//     _program.addLine(lineNumber, wrapStatements(std::move(statements)), source);
// }

// void BasicEngine::processRawLine(const std::string& line) {
//     if (line.empty()) return;

//     // Find the first space to separate line number from code
//     size_t firstSpace = line.find(' ');
//     if (firstSpace != std::string::npos) {
//         try {
//             int lineNum = std::stoi(line.substr(0, firstSpace));
//             std::string content = line.substr(firstSpace + 1);
//             storeLine(lineNum, content);
//         } catch (...) {
//             // Log error: Line doesn't start with a valid number
//         }
//     }
// }

// // Removes a line
// void BasicEngine::removeLine(int lineNumber) {
//     _program.removeLine(lineNumber);
// }

// void BasicEngine::executeImmediate(const std::string& source){
//     auto tokens = _tokenizer.tokenize(source);
//     auto statements = _parser.parse(std::move(tokens));
//     if (statements.empty()) return;

//     // Treat immediate execution as a temporary "Line 0"
//     // This allows loops/jumps to point back to "0" and work!
//     _program.addLine(0, wrapStatements(std::move(statements)), source);
    
//     // Run starting at 0, but do NOT reset variables (keep REPL state)
//     runInternal(0, false, true);

//     // Cleanup so it doesn't show up in LIST
//     _program.removeLine(0);
// }

// void BasicEngine::run() {
//     // Normal program execution starts from the beginning and resets state
//     // DumpProgram();
//     runInternal(_program.begin() != _program.end() ? _program.begin()->first : 1, true, false);
// }

// // Unified core execution loop
// void BasicEngine::runInternal(int startLine, bool shouldReset, bool limitToOneLine){
//     if (_program.isEmpty()) return;
//     if (shouldReset) _ctx.reset();

//     _ctx.halted = false;
//     _ctx.jumpRequested = false;
    
//     auto pc = _program.lower_bound(startLine);
    
//     _host.service();
//     while (pc != _program.end() && !_ctx.halted){
//         _ctx.currentLine = pc->first;

//         // Handle break requests
//         if (_host.checkBreakRequested()){
//             _host.print("! BREAK TRIG'D ON ");
//             _host.print(std::to_string(_ctx.currentLine));
//             _host.print("\n");
//             return;
//         }
//         _host.service();
        
//         if (pc->second.statement){
//             try{
//                 pc->second.statement->accept(_interpreter);
//             } catch (const std::exception& e){
//                 _host.print("? RUNTIME ERROR: " + std::string(e.what()) + "\n");
//                 break;
//             }
//         }

//         // Clear all temporary results per line
//         _ctx.transientArena.reset();

//         // Handle Control Flow
//         if (_ctx.jumpRequested){
//             _ctx.jumpRequested = false;
//             pc = _program.lower_bound(_ctx.jumpTarget);
//             // We don't pc++ here, we start the next iteration at the jump target
//         } else {
//             pc++;
//             if (limitToOneLine) break;
//         }
//     }
// }

// void BasicEngine::list(){
//     for (auto const& line : _program) {
//         _host.print(std::to_string(line.first) + " " + line.second.source + "\n");
//     }
// }

// void BasicEngine::clear(){
//     _program.clear();
//     _ctx.reset();
//     _symTable.clear();
//     _staticArena.reset();
// }

// void BasicEngine::loadProgram(const std::string& script){
//     _program.clear(); // Optional: clear existing program first
//     _symTable.clear();
//     _staticArena.reset();
    
//     std::stringstream ss(script);
//     std::string line;
    
//     while (std::getline(ss, line)){
//         if (line.empty()) continue;

//         // Find the first space to separate line number from code
//         size_t firstSpace = line.find(' ');
//         if (firstSpace != std::string::npos){
//             try{
//                 int lineNum = std::stoi(line.substr(0, firstSpace));
//                 std::string content = line.substr(firstSpace + 1);
//                 storeLine(lineNum, content);
//             } catch (...){
//                 // Skip lines that don't start with a valid number
//                 continue;
//             }
//         }
//     }
// }

// // Helper to wrap multiple statements into one if needed
// StmtPtr BasicEngine::wrapStatements(std::vector<StmtPtr>&& statements){
//     if (statements.size() == 1) {
//         return std::move(statements[0]);
//     }
//     void* mem = _staticArena.alloc(sizeof(BlockStmt));
//     return new (mem) BlockStmt(std::move(statements));
// }

// void BasicEngine::DumpProgram() {
//     _host.print(_symTable.DumpTable().c_str());
//     _host.print("--- PROGRAM AST DUMP ---\n");

//     PrettyPrinter printer(_symTable);

//     // Iterating through the map of program lines
//     for (auto const& [lineNum, lineData] : _program) {
//         _host.print("[");
//         _host.print(std::to_string(lineNum));
//         _host.print("] ");
        
//         // This triggers the visitor pattern
//         std::string output = printer.print(lineData.statement);
//         _host.print(output);
//     }
//     _host.print("------------------------\n");

//     _host.print("Size: ");
//     _host.print(std::to_string(reportProgramSize()));
//     _host.print("\nTotal space: ");
//     _host.print(std::to_string(PROGRAM_SIZE));
//     _host.print("\n");
// }