cmake_minimum_required(VERSION 3.10)
project(ESPBASIC_Native)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # This is vital

include_directories(core/include)
file(GLOB_RECURSE CORE_SOURCES "core/src/*.cpp")

add_executable(core_test ${CORE_SOURCES} platforms/native/console_host.h)