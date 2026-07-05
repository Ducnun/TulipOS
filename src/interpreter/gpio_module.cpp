#include "runtime/modules/gpio_module.h"

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
    return Value::Bool(false); 
}

Value gpio::write(const std::vector<Value>& args){
    #ifdef ESP32
    // Safety check: ensure we have 2 arguments
    if (args.size() < 2) return Value::Bool(false);

    if (args[0].type != ValueType::Number || args[1].type != ValueType::Number) return Value::Bool(false);
    int pin = (int)args[0].number;
    int val = (int)args[1].number;
    
    pinMode(pin, OUTPUT); // Good practice to set mode
    digitalWrite(pin, val);

    #endif
    return Value::Bool(true); // Return success
}

Value gpio::read(const std::vector<Value>& args) {
    #ifdef ESP32
    if (args.empty()) return Value::Float(0.0f);

    if (args[0].type != ValueType::Number) return Value::Bool(false);
    int pin = (int)args[0].number;
    pinMode(pin, INPUT);
    
    return Value::Float((float)digitalRead(pin));
    #else
    return Value::Bool(true);
    #endif
}