// Compiled only if esp32 env is chosen
#ifdef ESP32
  #include "host/esp32_host.h"
  #include "system/shell.h"

  PS2Keyboard keyboard(13,14);
  UARTTerminal terminal(1000000);
  ESP32Host host(keyboard, terminal);
  Shell shell(host);

  void setup(){
    // Start IO and shell
    Serial.begin(115200);
    keyboard.begin();
    host.begin();
    shell.runREPL();
  }

  void loop(){
    // Loop is handled by REPL
  }

// Compiled only when native env is chosen
#else
  #include "host/console_host.h"
  #include "system/shell.h"
  ConsoleHost host;

  // Start IO and shell
  int main(){
    Shell shell(host);
    shell.runREPL();
  }
#endif