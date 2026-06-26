#include "interpreter.h"

// Compiled only if esp32 env is chosen
#ifdef ESP32
  #include "esp32_io.h"

  PS2Keyboard keyboard(13,14);
  UARTTerminal terminal(1000000);
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
  #include "console_io.h"
  ConsoleIO io;

  // Start IO and Interpreter
  int main(){
    // BasicInterpreter interpreter(io);
    // interpreter.runREPL();
    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize("LET A = 5");
    for (const auto& token : tokens){
      io.print(TokenUtils::toString(token));
    }
  }
#endif