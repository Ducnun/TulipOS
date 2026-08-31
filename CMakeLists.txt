cmake_minimum_required(VERSION 3.16)

# Include the standard IDF project logic
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Name your project (must match the P4 goal)
project(TulipBasic_P4)

# Add existing code from platformio project
set(EXTRA_COMPONENT_DIRS 
    "src"
    "lib"
)
