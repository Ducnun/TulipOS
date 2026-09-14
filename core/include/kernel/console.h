#pragma once

#include "api/iconsole.h"
#include "api/iinput_service.h"
#include "api/ioutput_service.h"

class Console : public IConsole {
public:
    Console(IInputService& input, IOutputService& output) : _input(input), _output(output) {}

    void run() override;

private: 
    IInputService& _input;
    IOutputService& _output;
};