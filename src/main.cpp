// Compiled only if esp32 env is chosen
#ifdef ESP32
  #include "esp32_io.h"
  #include "shell.h"

  PS2Keyboard keyboard(13,14);
  UARTTerminal terminal(1000000);
  ESP32IO io(keyboard, terminal);
  Shell shell(io);

  void setup(){
    // Start IO and shell
    Serial.begin(115200);
    keyboard.begin();
    io.begin();
    shell.runREPL();
  }

  void loop(){
    // Loop is handled by REPL
  }

// Compiled only when native env is chosen
#else
  #include "console_io.h"
  #include "shell.h"
  ConsoleIO io;

  // Start IO and shell
  int main(){
    Shell shell(io);
    shell.runREPL();
  }
#endif