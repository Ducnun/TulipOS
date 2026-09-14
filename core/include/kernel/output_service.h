#pragma once

#include "kernel/api/ioutput_service.h"
#include "hal/iterminal.h"

class OutputService : public IOutputService{
public:
    OutputService(ITerminal& terminal) : _terminal(terminal) {}

    void print(util::StringView txt) override;
    void print(char c) override;
    void println(util::StringView txt) override;
    void clear() override;
    void setColor(uint8_t fg, uint8_t bg) override;

private:
    ITerminal& _terminal;
};