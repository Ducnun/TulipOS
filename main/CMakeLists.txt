# This tells CMake only to look at the P4-specific implementations
file(GLOB_RECURSE SOURCES 
    "*.cpp"
    "host/esp32p4/*.cpp" 
)
# Exclude the Arduino versions so they don't cause conflicts
list(FILTER SOURCES EXCLUDE REGEX "host/esp32/.*")

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS "../include"
)