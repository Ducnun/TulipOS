// Compiled only if esp32 env is chosen
#ifdef ESP32
  #include "Arduino.h"
  #include "esp32_io.h"
  #include "interpreter.h"

  PS2Keyboard keyboard(13,14);
  UARTTerminal terminal(115200);
  ESP32IO io(keyboard, terminal);
  BasicInterpreter interpreter(io);

  void setup(){
    // Start IO and interpreter
    Serial.begin(115200);
    keyboard.begin();
    io.begin();
    interpreter.runREPL();
  }

  void loop(){
    // Loop is handled by REPL
  }

// Compiled only when native env is chosen
#else
  #include "interpreter.h"
  #include "console_io.h"
  ConsoleIO io;

  // Start IO and Interpreter
  int main(){
    BasicInterpreter interpreter(io);
    interpreter.runREPL();
  }
#endif