file(GLOB SOURCES "*.cpp")

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "."
    REQUIRES 
        core
        freertos 
        esp_timer 
        esp_system
        esp_driver_gpio
        esp_driver_uart
        esp_driver_spi
        esp_heap_caps
)