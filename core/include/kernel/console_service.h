#pragma once

#include "hal/ikeyboard.h"
#include "hal/iterminal.h"
#include "hal/ihost.h"
#include "iconsole.h"
#include <vector>

class ConsoleService : public IConsole {
public:
    ConsoleService(ITerminal* terminal, IKeyboard* keyboard, IHost& host) : _terminal(terminal), _keyboard(keyboard), _host(host) {}

    std::string readLine(const std::string& prompt);
    void print(const std::string& text);
    void println(const std::string& text);
    void clearScreen();
    void setCursor(int x, int y);
    void setColor(uint8_t color);
    void setBackgroundColor(uint8_t color);
    bool checkAndResetBreak();

private:
    ITerminal* _terminal;
    IKeyboard* _keyboard;
    IHost& _host;

    std::string _currentBuffer;
    std::vector<std::string> _history;
    int _historyIndex;
    bool _breakRequested;

    void historyNav(std::string& currentLine, bool up);
};