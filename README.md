# TulipOS
This project aims to develop an OS inspired by 80s home computers for cheap microcontrollers. The name comes from an old Dutch computer manufacturer, whose keyboard was the sole piece of inspiration to start this project. This all started as a way to learn systems engineering coming from an embedded background and has spiralled out of control since. It is by no means perfect or intended for serious use.

## Features
- **Shell:** Command execution, line editing and program management
- **BASIC Interpreter:** language syntax, variables, program storage and execution
- **Hardware Abstraction:** Interfaces that abstract away hardware interaction from higher level functionality
- **Display:** Support for a VGA terminal through an additional project (found [here](https://github.com/Ducnun/PicoVGATerm))
- **Input:** PS/2 Keyboard support
- **Storage:** SD card support using a (currently read only) FAT32 filesystem implementation

## Platform support
| Component     | Implementation                 |
|---------------|--------------------------------|
| Host Platform | ESP32 / ESP32-P4               |
| Keyboard      | PS/2                           |
| Display       | RP2350 VGA Terminal            |
| Storage       | SPI SD card driver (read only) |

## Build instructions
For classic ESP32 targets, PlatformIO and the Arduino framework were used. This is currently not in a fully buildable state as the host class and some configs are missing.

For the ESP32-P4, ESP-IDF is used to get full support for the newer P4 board. To build, simply build and flash using ESP-IDF tools to build the project in `platforms/esp32_p4`. This build expects a PS/2 Keyboard, an SPI SD card module and the RP2350 based display driver in the default build. To host it more barebones, the host file in `main/*_host.h` can be altered to use custom implementations of the interfaces found in `core/hal`. These interfaces should live in the `drivers` directory.

Custom platforms could be added by creating symlinks of the `core` and `drivers` directory for the right platform (e.g. Arduino or ESP-IDF), then creating a custom main file and host object that represent the exact hardware configuration, tying together the required drivers and the kernels services.

