cmake_minimum_required(VERSION 3.18.0)

option(CATCH_BUILD_TESTING OFF)
option(SPDLOG_ENABLE_PCH ON)
add_subdirectory(third_party/catch2)
add_subdirectory(third_party/spdlog)

project(keegmake)

include(cmake/ClangFormat.cmake)
include(cmake/ClangTidy.cmake)

option(ENABLE_COLOR "Enable colors in the output of all possible tools" ON)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")

    set(KEEGMAKE_CXX_FLAGS_WARNING "-Wall;-Wextra;-Werror;")
    set(KEEGMAKE_CXX_FLAGS_OPTIMIZATION "-Og;-g")
    set(KEEGMAKE_CXX_FLAGS_TEST "--coverage")
    set(KEEGMAKE_CXX_FLAGS_OTHERS "")
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        if(${ENABLE_COLOR})
            list(APPEND KEEGMAKE_CXX_FLAGS_OTHERS "-fdiagnostics-color=always")
        endif()
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
        if(${ENABLE_COLOR})
            list(APPEND KEEGMAKE_CXX_FLAGS_OTHERS "-fdiagnostics-color")
        endif()

    endif()

else()
    message(WARNING "Unsupported compiler detected! ${CMAKE_CXX_COMPILER_ID}")
endif()

# Always enable compile_commands.json generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_custom_target(gen_ast COMMAND ${CMAKE_SOURCE_DIR}/tools/astGen.py ${CMAKE_BINARY_DIR}/include BYPRODUCTS "${CMAKE_BINARY_DIR}/include/expr_ast.hpp")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include")
add_library(ast INTERFACE)
add_dependencies(ast gen_ast)
target_include_directories(ast INTERFACE "${CMAKE_BINARY_DIR}/include")

add_executable(main
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ast_visitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/expressions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interpreter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/literal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scanner.cpp
)
target_link_libraries(main spdlog::spdlog ast)
target_compile_features(main PRIVATE cxx_std_17)
target_compile_options(main PRIVATE ${KEEGMAKE_CXX_FLAGS_WARNING} ${KEEGMAKE_CXX_FLAGS_OPTIMIZATION} ${KEEGMAKE_CXX_FLAGS_OTHERS})
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/src")
clangtidy_addtarget(main)

clangformat_globfiles()
