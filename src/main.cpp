#ifdef ESP32
  #include "esp32_io.h"
  // ESP32IO io;
#else
  #include "console_io.h"
  ConsoleIO io;
  #include "interpreter.h"
  
  int main(){
    BasicInterpreter interpreter(io);
    interpreter.runREPL();
  }
#endif


#ifdef ESP32

  #include "Arduino.h"

  // Pins (change to your actual GPIOs)
  const int ps2ClkPin = 14;
  const int ps2DataPin = 13;

  volatile bool dataReady = false;
  volatile uint8_t incomingByte = 0;
  volatile int bitCount = 0;

  void IRAM_ATTR onClockFall() {
    static uint8_t data = 0;
    static int count = 0;

    bool dataBit = digitalRead(ps2DataPin);

    if (count >= 1 && count <= 8) {
      data >>= 1;
      if (dataBit)
        data |= 0x80;
    }

    count++;

    if (count == 11) {  // Full frame: 1 start, 8 data, 1 parity, 1 stop
      incomingByte = data;
      dataReady = true;
      count = 0;
      data = 0;
    }
  }

  void setup() {
    Serial.begin(115200);
    delay(1000);

    pinMode(ps2ClkPin, INPUT_PULLUP);
    pinMode(ps2DataPin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ps2ClkPin), onClockFall, FALLING);

    Serial.println("PS/2 keyboard test started");
  }

  void loop() {
    if (dataReady) {
      dataReady = false;
      Serial.print("Scan code: 0x");
      Serial.println(incomingByte, HEX);

      // Example: 0x12 = Left Shift Make, 0xF0 = Break prefix
      if (incomingByte == 0x5A) Serial.println("Enter key pressed");
    }
  }

#endif