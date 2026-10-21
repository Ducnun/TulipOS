#include "system/shell.h"

#include "storage/filesystems/vfs/dir_entry.h"
#include "utils/span.h"
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace {
bool isWhitespace(char character) {
    return character == ' ' || character == '\t' || character == '\r' ||
           character == '\n' || character == '\v' || character == '\f';
}

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

char uppercaseAscii(char character) {
    if (character >= 'a' && character <= 'z') {
        return static_cast<char>(character - ('a' - 'A'));
    }
    return character;
}

bool equalsIgnoreCase(util::StringView text, const char* expected) {
    const size_t expectedLength = std::strlen(expected);
    if (text.length != expectedLength) return false;

    for (size_t i = 0; i < text.length; ++i) {
        if (uppercaseAscii(text.data[i]) != expected[i]) return false;
    }
    return true;
}

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

void Shell::run() {
    _console.clear();
    _programIO.write("Tulip OS Shell\n");

    char buffer[256];
    while (true) {
        const char* cwd = _storage.getCurrentDirectory();
        _programIO.write(cwd ? cwd : "");
        _programIO.write("> ");

        if (!_programIO.readLine(buffer, sizeof(buffer))) continue;
        size_t lineLength = std::strlen(buffer);
        while (lineLength > 0 && isWhitespace(buffer[lineLength - 1])) {
            buffer[--lineLength] = '\0';
        }
        if (lineLength == 0) continue;

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

Shell::CommandResult Shell::executeCommand(util::StringView line) {
    const util::StringView command = trim(line);

    if (equalsIgnoreCase(command, "EXIT") || equalsIgnoreCase(command, "QUIT")) {
        return CommandResult::Exit;
    }
    if (equalsIgnoreCase(command, "RUN")) {
        const size_t start = _taskControl.millis();
        _engine.run();
        _programIO.write("Execution time: ");
        writeUnsigned(_programIO, _taskControl.millis() - start);
        _programIO.write(" ms\n");
    } else if (equalsIgnoreCase(command, "LIST")) {
        _engine.list();
    } else if (equalsIgnoreCase(command, "NEW")) {
        _engine.clear();
    } else if (equalsIgnoreCase(command, "DUMP")) {
        _engine.DumpProgram();
    } else if (equalsIgnoreCase(command, "CLS")) {
        _console.clear();
    } else if (equalsIgnoreCase(command, "PWD")) {
        const char* cwd = _storage.getCurrentDirectory();
        _programIO.write(cwd ? cwd : "");
        _programIO.write("\n");
    } else if (equalsIgnoreCase(command, "LS")) {
        listDirectory();
    } else if (equalsIgnoreCase(command, "HELP")) {
        _programIO.write("RUN LIST NEW DUMP LS CD <path> PWD CLS EXIT\n");
    } else if (command.length >= 2 &&
               uppercaseAscii(command.data[0]) == 'C' &&
               uppercaseAscii(command.data[1]) == 'D' &&
               (command.length == 2 || isWhitespace(command.data[2]))) {
        size_t pathStart = 2;
        while (pathStart < command.length &&
               isWhitespace(command.data[pathStart])) {
            ++pathStart;
        }
        if (!_storage.changeDirectory(command.data + pathStart)) {
            _programIO.write("? Unable to change directory\n");
        }
    } else {
        return CommandResult::NotHandled;
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
