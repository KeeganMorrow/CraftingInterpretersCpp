cmake_minimum_required(VERSION 3.18.0)
project(keegmake)

# Always enable compile_commands.json generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
