cmake_minimum_required(VERSION 3.16)

# 1. Set variables that influence component discovery FIRST
set(EXTRA_COMPONENT_DIRS 
    "src" 
    "lib"
)

# 2. Then include the project logic
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# 3. Then define the project
project(TulipBasic_P4)

# 4. Then global compiler options
add_compile_options(
    -Wno-error=reorder
    -Wno-error=ignored-qualifiers
    -Wno-error=packed-not-aligned
    -fexceptions
)