#include "system/shell.h"

#include "storage/filesystems/vfs/dir_entry.h"
#include "utils/span.h"
#include <cctype>
#include <sstream>

namespace {
std::string uppercase(const std::string& text) {
    std::string result = text;
    for (char& character : result) {
        character = static_cast<char>(
            std::toupper(static_cast<unsigned char>(character)));
    }
    return result;
}
}

void Shell::run() {
    _console.clear();
    _programIO.write("Tulip OS Shell\n");

    char buffer[256];
    while (true) {
        const char* cwd = _storage.getCurrentDirectory();
        _programIO.write(cwd ? cwd : "");
        _programIO.write("> ");

        if (!_programIO.readLine(buffer, sizeof(buffer))) continue;
        const std::string line(buffer);
        if (line.empty()) continue;

        try {
            if (executeCommand(line)) return;

            std::istringstream input(line);
            int lineNumber;
            if ((line[0] >= '0' && line[0] <= '9') && (input >> lineNumber)) {
                std::string source;
                std::getline(input, source);
                const size_t firstCodeCharacter = source.find_first_not_of(" \t");
                if (firstCodeCharacter == std::string::npos) {
                    _engine.removeLine(lineNumber);
                } else {
                    _engine.storeLine(lineNumber, source.substr(firstCodeCharacter));
                }
            } else {
                _engine.executeImmediate(line);
            }
        } catch (const std::exception& error) {
            _programIO.write("? ERROR: ");
            _programIO.write(error.what());
            _programIO.write("\n");
        }
    }
}

bool Shell::executeCommand(const std::string& line) {
    const std::string command = uppercase(line);

    if (command == "EXIT" || command == "QUIT") return true;
    if (command == "RUN") {
        const size_t start = _taskControl.millis();
        _engine.run();
        _programIO.write("Execution time: ");
        _programIO.write(std::to_string(_taskControl.millis() - start));
        _programIO.write(" ms\n");
    } else if (command == "LIST") {
        _engine.list();
    } else if (command == "NEW") {
        _engine.clear();
    } else if (command == "DUMP") {
        _engine.DumpProgram();
    } else if (command == "CLS") {
        _console.clear();
    } else if (command == "PWD") {
        const char* cwd = _storage.getCurrentDirectory();
        _programIO.write(cwd ? cwd : "");
        _programIO.write("\n");
    } else if (command == "LS") {
        listDirectory();
    } else if (command == "HELP") {
        _programIO.write("RUN LIST NEW DUMP LS CD <path> PWD CLS EXIT\n");
    } else if (command.rfind("CD ", 0) == 0) {
        const std::string path = line.substr(3);
        if (!_storage.changeDirectory(path.c_str())) {
            _programIO.write("? Unable to change directory\n");
        }
    } else {
        return false;
    }

    return false;
}

void Shell::listDirectory() {
    DirectoryEntry entries[32];
    const size_t count = _storage.listDirectory(nullptr, util::span<DirectoryEntry>(entries));
    for (size_t i = 0; i < count; ++i) {
        _programIO.write(entries[i].name);
        if (entries[i].isDirectory) {
            _programIO.write("/");
        } else {
            _programIO.write("  ");
            _programIO.write(std::to_string(entries[i].size));
        }
        _programIO.write("\n");
    }
}
