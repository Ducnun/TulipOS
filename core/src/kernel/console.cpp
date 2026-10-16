#include "kernel/console.h"

void Console::run(){
    _output.clear();
    _output.println("Tulip OS Shell");
    while(true){
        _output.print("> ");    // Temp prompt

        std::string line;
        KeyEvent ev;
        while (true){
            _input.waitForKeyEvent(ev);
            if (ev.action != KeyAction::Char) continue;

            if (ev.character == '\n' || ev.character == '\r'){
                _output.print('\n');
                break;
            }

            if (ev.character == '\b'){
                if (!line.empty()){
                    line.pop_back();
                    _output.print('\b');
                }
                continue;
            }

            if (ev.character >= ' ' || ev.character == '\t'){
                line += ev.character;
                _output.print(ev.character);
            }
        }
    }
}