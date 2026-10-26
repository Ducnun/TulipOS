#include "system/shell.h"

#include "storage/filesystems/vfs/dir_entry.h"
#include "utils/span.h"
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <string>

// Shell helpers
namespace {
    // Helper for whitepsace
    bool isWhitespace(char character) {
        return character == ' ' || character == '\t' || character == '\r' ||
            character == '\n' || character == '\v' || character == '\f';
    }

    // Trimming whitespace from both ends of a string
    util::StringView trim(util::StringView text) {
        while (text.length > 0 && isWhitespace(text.data[0])) {
            ++text.data;
            --text.length;
        }
        while (text.length > 0 && isWhitespace(text.data[text.length - 1])) {
            --text.length;
        }
        return text;
    }

    // Convert a lowercase ASCII character to uppercase; leave other characters unchanged
    char uppercaseAscii(char character) {
        if (character >= 'a' && character <= 'z') {
            return static_cast<char>(character - ('a' - 'A'));
        }
        return character;
    }

    // Case-insensitive comparison of a StringView with a C-string
    bool equalsIgnoreCase(util::StringView text, const char* expected) {
        const size_t expectedLength = std::strlen(expected);
        if (text.length != expectedLength) return false;

        for (size_t i = 0; i < text.length; ++i) {
            if (uppercaseAscii(text.data[i]) != expected[i]) return false;
        }
        return true;
    }

    // Parse a line number from the beginning of a string, returning the line number and the remaining source code
    bool parseLineNumber(const char* line, int& lineNumber, const char*& source) {
        if (line[0] < '0' || line[0] > '9') return false;

        int value = 0;
        const char* cursor = line;
        while (*cursor >= '0' && *cursor <= '9') {
            const int digit = *cursor - '0';
            if (value > (std::numeric_limits<int>::max() - digit) / 10) {
                return false;
            }
            value = value * 10 + digit;
            ++cursor;
        }

        while (*cursor == ' ' || *cursor == '\t') ++cursor;
        lineNumber = value;
        source = cursor;
        return true;
    }

    // Write an unsigned integer to the program IO as a string
    void writeUnsigned(IProgramIO& programIO, uint64_t value) {
        char digits[20];
        size_t position = sizeof(digits);
        do {
            digits[--position] = static_cast<char>('0' + value % 10);
            value /= 10;
        } while (value != 0);

        programIO.write(util::StringView(digits + position, sizeof(digits) - position));
    }
}

// Main shell loop
void Shell::run() {
    // Print welcome message
    _console.clear();
    _programIO.write("Tulip OS Shell\n");
    _programIO.write("Type HELP for a list of commands.\n");

    char buffer[256];
    while (true) {
        // Attempt to get CWD to print as prompt
        const char* cwd = _storage.getCurrentDirectory();
        _programIO.write(cwd ? cwd : "");
        _programIO.write("> ");

        // Read a line of input from the user
        if (!_programIO.readLine(buffer, sizeof(buffer))) continue;
        size_t lineLength = std::strlen(buffer);
        while (lineLength > 0 && isWhitespace(buffer[lineLength - 1])) {
            buffer[--lineLength] = '\0';
        }
        if (lineLength == 0) continue;

        // Process input, either as shell command or as a program line
        try {
            const CommandResult commandResult =
                executeCommand(util::StringView(buffer, lineLength));
            if (commandResult == CommandResult::Exit) return;
            if (commandResult == CommandResult::Handled) continue;

            int lineNumber;
            const char* source;
            if (parseLineNumber(buffer, lineNumber, source)) {
                if (*source == '\0') {
                    _engine.removeLine(lineNumber);
                } else {
                    _engine.storeLine(lineNumber, std::string(source));
                }
            } else {
                _engine.executeImmediate(std::string(buffer));
            }
        } catch (const std::exception& error) {
            _programIO.write("? ERROR: ");
            _programIO.write(error.what());
            _programIO.write("\n");
        }
    }
}

const Shell::Command* Shell::commandTable(size_t& count) {
    static const Command commands[] = {
        {"EXIT", "EXIT", "Leave the shell", &Shell::commandExit},
        {"QUIT", "QUIT", "Leave the shell", &Shell::commandExit},
        {"RUN", "RUN", "Run the current BASIC program", &Shell::commandRun},
        {"LIST", "LIST", "List the current BASIC program", &Shell::commandList},
        {"NEW", "NEW", "Clear the current BASIC program", &Shell::commandNew},
        {"DUMP", "DUMP", "Show program diagnostics", &Shell::commandDump},
        {"CLS", "CLS", "Clear the console", &Shell::commandCls},
        {"PWD", "PWD", "Show the current directory", &Shell::commandPwd},
        {"LS", "LS", "List directory contents", &Shell::commandLs},
        {"LOAD", "LOAD <path>", "Load a BASIC program from storage",
         &Shell::commandLoad},
        {"HELP", "HELP", "Show available shell commands", &Shell::commandHelp},
        {"CD", "CD <path>", "Change the current directory", &Shell::commandCd},
    };
    count = sizeof(commands) / sizeof(commands[0]);
    return commands;
}

// Shell command dispatcher
Shell::CommandResult Shell::executeCommand(util::StringView line) {
    const util::StringView input = trim(line);
    size_t nameLength = 0;
    while (nameLength < input.length && !isWhitespace(input.data[nameLength])) {
        ++nameLength;
    }

    const util::StringView name(input.data, nameLength);
    const util::StringView arguments =
        nameLength < input.length
            ? trim(util::StringView(input.data + nameLength,
                                    input.length - nameLength))
            : util::StringView("", 0);

    size_t commandCount = 0;
    const Command* commands = commandTable(commandCount);
    for (size_t i = 0; i < commandCount; ++i) {
        if (equalsIgnoreCase(name, commands[i].name)) {
            return (this->*commands[i].handler)(arguments);
        }
    }

    return CommandResult::NotHandled;
}

Shell::CommandResult Shell::commandExit(util::StringView) {
    return CommandResult::Exit;
}

Shell::CommandResult Shell::commandRun(util::StringView) {
    const size_t start = _taskControl.millis();
    _engine.run();
    _programIO.write("Execution time: ");
    writeUnsigned(_programIO, _taskControl.millis() - start);
    _programIO.write(" ms\n");
    return CommandResult::Handled;
}

Shell::CommandResult Shell::commandList(util::StringView) {
    _engine.list();
    return CommandResult::Handled;
}

Shell::CommandResult Shell::commandNew(util::StringView) {
    _engine.clear();
    return CommandResult::Handled;
}

Shell::CommandResult Shell::commandDump(util::StringView) {
    _engine.DumpProgram();
    return CommandResult::Handled;
}

Shell::CommandResult Shell::commandCls(util::StringView) {
    _console.clear();
    return CommandResult::Handled;
}

Shell::CommandResult Shell::commandPwd(util::StringView) {
    const char* cwd = _storage.getCurrentDirectory();
    _programIO.write(cwd ? cwd : "");
    _programIO.write("\n");
    return CommandResult::Handled;
}

Shell::CommandResult Shell::commandLs(util::StringView) {
    listDirectory();
    return CommandResult::Handled;
}

Shell::CommandResult Shell::commandLoad(util::StringView arguments) {
    loadProgram(arguments);
    return CommandResult::Handled;
}

Shell::CommandResult Shell::commandHelp(util::StringView) {
    size_t commandCount = 0;
    const Command* commands = commandTable(commandCount);
    _programIO.write("Commands:\n");
    for (size_t i = 0; i < commandCount; ++i) {
        if (equalsIgnoreCase(util::StringView(commands[i].name), "QUIT")) {
            continue;
        }
        _programIO.write(commands[i].usage);
        _programIO.write(" - ");
        _programIO.write(commands[i].description);
        _programIO.write("\n");
    }
    return CommandResult::Handled;
}

Shell::CommandResult Shell::commandCd(util::StringView arguments) {
    if (arguments.length == 0) {
        _programIO.write("? Usage: CD <path>\n");
        return CommandResult::Handled;
    }

    const std::string path(arguments.data, arguments.length);
    if (!_storage.changeDirectory(path.c_str())) {
        _programIO.write("? Unable to change directory\n");
    }
    return CommandResult::Handled;
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
            writeUnsigned(_programIO, entries[i].size);
        }
        _programIO.write("\n");
    }
}

void Shell::loadProgram(util::StringView path) {
    if (path.length == 0) {
        _programIO.write("? Usage: LOAD <path>\n");
        return;
    }

    const std::string filename(path.data, path.length);
    uint8_t* programBuffer = nullptr;
    size_t programSize = 0;
    if (!_storage.loadProgram(filename.c_str(), &programBuffer, &programSize)) {
        _programIO.write("? Unable to load program\n");
        return;
    }

    auto freeBuffer = [this, programSize](uint8_t* buffer) {
        _storage.freeProgram(buffer, programSize);
    };
    std::unique_ptr<uint8_t, decltype(freeBuffer)> bufferGuard(
        programBuffer, freeBuffer);

    try {
        const std::string script(
            reinterpret_cast<const char*>(programBuffer), programSize);
        _engine.loadProgram(script);
        _programIO.write("Program loaded successfully\n");
    } catch (const std::exception& error) {
        _programIO.write("? ERROR: ");
        _programIO.write(error.what());
        _programIO.write("\n");
    }
}