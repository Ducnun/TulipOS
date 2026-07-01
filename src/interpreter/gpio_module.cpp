#include "interpreter/gpio_module.h"

Value gpio::call(const std::string& func, const std::vector<Value>& args) {
    #ifndef ESP32
        throw std::runtime_error("GPIO Module not supported on native interpreter");
    #endif

    if (func == "WRITE") {
        return gpio::write(args);
    } 
    else if (func == "READ") {
        return gpio::read(args);
    }

    // ALWAYS return something, even for unknown functions
    return Value(false); 
}

Value gpio::write(const std::vector<Value>& args){
    #ifdef ESP32
    // Safety check: ensure we have 2 arguments
    if (args.size() < 2) return Value(false);

    int pin = (int)args[0].asNumber();
    int val = (int)args[1].asNumber();
    
    pinMode(pin, OUTPUT); // Good practice to set mode
    digitalWrite(pin, val);

    #endif
    return Value(true); // Return success
}

Value gpio::read(const std::vector<Value>& args) {
    #ifdef ESP32
    if (args.empty()) return Value(0.0f);

    int pin = (int)args[0].asNumber();
    pinMode(pin, INPUT);
    
    return Value((float)digitalRead(pin));
    #else
    return Value(true);
    #endif
}