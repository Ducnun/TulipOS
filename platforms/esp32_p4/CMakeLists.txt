cmake_minimum_required(VERSION 3.16)

# We point to the symlink folders directly. 
# ESP-IDF will look for a CMakeLists.txt inside each.
set(EXTRA_COMPONENT_DIRS 
    "core" 
    "hardware_drivers"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_p4_app)