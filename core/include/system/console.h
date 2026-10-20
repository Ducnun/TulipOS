#pragma once

#include "api/iconsole.h"
#include "kernel/api/iinput_service.h"
#include "kernel/api/ioutput_service.h"
#include <string>
#include <vector>

class Console : public IConsole {
public:
    Console(IInputService& input, IOutputService& output) : _input(input), _output(output) {}

    bool readLine(util::StringView prompt, char* buffer, size_t capacity) override;
    void clear() override { _output.clear(); }

private: 
    IInputService& _input;
    IOutputService& _output;
    std::vector<std::string> _history;
    static constexpr size_t HISTORY_LIMIT = 16;

    void replaceLine(std::string& line, const std::string& replacement);
};