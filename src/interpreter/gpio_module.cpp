#include "interpreter/gpio_module.h"

Value gpio::call(const CallStmt& stmt, const std::vector<Value>& args) {
    #ifndef ESP32
        throw std::runtime_error("GPIO Module not supported on native interpreter");
    #endif

    if (stmt.function == "WRITE") {
        return gpio::write(args);
    } 
    else if (stmt.function == "READ") {
        return gpio::read(args);
    }

    // ALWAYS return something, even for unknown functions
    return Value(false); 
}

Value gpio::write(const std::vector<Value>& args) {
    // Safety check: ensure we have 2 arguments
    if (args.size() < 2) return Value(false);

    int pin = (int)args[0].asNumber();
    int val = (int)args[1].asNumber();
    
    pinMode(pin, OUTPUT); // Good practice to set mode
    digitalWrite(pin, val);

    return Value(true); // Return success
}

Value gpio::read(const std::vector<Value>& args) {
    if (args.empty()) return Value(0.0);

    int pin = (int)args[0].asNumber();
    pinMode(pin, INPUT);
    
    return Value((double)digitalRead(pin));
}