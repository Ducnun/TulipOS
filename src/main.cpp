// Compiled only if esp32 env is chosen
#ifdef ESP32
  #include "host/esp32_definitions.h"
  #include "host/esp32_host.h"
  #include "system/shell.h"
  #include "devices/sd_card_spi.h"
  #include "storage/mbr/mbr.h"
  #include "storage/block/volume.h"

  PS2Keyboard keyboard(KB_DATA, KB_CLK);
  UARTTerminal terminal(UARTTERMINAL_SPEED);
  SDCardSPI sdDriver(SD_MOSI, SD_MISO, SD_SCK, SD_CS);
  ESP32Host host(keyboard, terminal);
  MBR mbr;
  Shell shell(host);

  void setup(){
    // Start IO and shell
    Serial.begin(115200);
    keyboard.begin();

    sdDriver.begin();
    mbr.read(sdDriver);
    auto partition = mbr.partition(0);
    Serial.println(partition.startLBA());
    Volume volume(sdDriver, partition);
    uint8_t boot[512];
    volume.readBlock(0, boot);

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