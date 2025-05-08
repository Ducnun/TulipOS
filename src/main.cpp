#ifdef ESP32
  #include "esp32_io.h"
  ESP32IO io;
#else
  #include "console_io.h"
  ConsoleIO io;
#endif

#include "interpreter.h"

int main(){
  BasicInterpreter interpreter(io);
  interpreter.runREPL();
}