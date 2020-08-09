cmake_minimum_required(VERSION 3.18.0)

option(CATCH_BUILD_TESTING OFF)
option(SPDLOG_ENABLE_PCH ON)
add_subdirectory(third_party/catch2)
add_subdirectory(third_party/spdlog)

project(keegmake)

include(cmake/ClangFormat.cmake)
include(cmake/ClangTidy.cmake)

# Always enable compile_commands.json generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(main src/main.cpp src/application.cpp src/interpreter.cpp src/scanner.cpp src/exception.cpp)
target_link_libraries(main spdlog::spdlog)
target_compile_features(main PRIVATE cxx_std_17)
clangtidy_addtarget(main)

clangformat_globfiles()
