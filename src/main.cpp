
#ifdef ESP32
  #include "Arduino.h"
  #include "esp32_io.h"
  #include "interpreter.h"

  PS2Keyboard keyboard(13,14);
  ESP32IO io(keyboard);
  BasicInterpreter interpreter(io);

  void setup(){
    Serial.begin(115200);
    keyboard.begin();
    io.begin();
    interpreter.runREPL();
  }

  void loop(){

  }

#else
  #include "interpreter.h"
  #include "console_io.h"
  ConsoleIO io;

  int main(){
    BasicInterpreter interpreter(io);
    interpreter.runREPL();
  }
#endif