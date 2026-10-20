#pragma once

#include "engine/iprogramio.h"
#include "kernel/api/ioutput_service.h"
#include "system/api/iconsole.h"

class ConsoleProgramIO : public IProgramIO {
public:
    ConsoleProgramIO(IConsole& console, IOutputService& output)
        : _console(console), _output(output) {}

    void write(util::StringView text) override {
        _output.print(text);
    }

    bool readLine(char* buffer, size_t capacity) override {
        return _console.readLine("", buffer, capacity);
    }

private:
    IConsole& _console;
    IOutputService& _output;
};