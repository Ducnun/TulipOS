#include "kernel/console.h"

void Console::run(){
    _output.println("Tulip OS Shell");
    while(true){
        _output.print("> ");    // Temp prompt

        std::string line;
        std::string escapeSeq;
        KeyEvent ev;
        while (true){
            _input.waitForKeyEvent(ev);
            if (ev.action == KeyAction::Char){
                line += ev.character;
                _output.print(ev.character);
            }
        }
    }
}