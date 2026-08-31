# 1. Get all CPP files recursively
file(GLOB_RECURSE ALL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")

set(SOURCES "")

foreach(file ${ALL_SOURCES})
    # Always exclude the old Arduino main
    if(file STREQUAL "main.cpp")
        continue()
    endif()

    # Target-specific logic
    if(CONFIG_IDF_TARGET_ESP32P4)
        # If we are on P4, EXCLUDE the old esp32 folder
        if(file MATCHES "^host/esp32/")
            continue()
        endif()
    elseif(CONFIG_IDF_TARGET_ESP32)
        # If we are on standard ESP32, EXCLUDE the p4 folder
        if(file MATCHES "^host/esp32p4/")
            continue()
        endif()
    endif()

    # If it passed the filters, add it to the build
    list(APPEND SOURCES ${file})
endforeach()

# Debugging: This will print during 'idf.py build' so you can see if the files are included
message(STATUS "Final Source List: ${SOURCES}")

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "../include"
    REQUIRES 
        freertos 
        esp_timer 
        esp_system
        esp_driver_gpio
        esp_driver_uart
        esp_driver_spi
)