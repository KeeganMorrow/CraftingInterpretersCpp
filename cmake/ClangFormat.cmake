
function(clangformat_addfiles)
    set(options "")
    set(oneValueArgs "FORMATTARGET")
    set(multiValueArgs "FILES")
    cmake_parse_arguments(ADDFILES "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})

    if(NOT ADDFILES_WORKING_DIRECTORY)
        set(ADDFILES_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    if(NOT ADDFILES_FORMATTARGET)
        set(ADDFILES_FORMATTARGET "clangformat")
    endif()

    find_program(CLANGFORMAT_BIN clang-format clangformat-11 clangformat-10 clangformat-9 clangformat-8)
    if(CLANGFORMAT_BIN)
        add_custom_target(${ADDFILES_FORMATTARGET} WORKING_DIRECTORY ${ADDFILES_WORKING_DIRECTORY} COMMAND ${CLANGFORMAT_BIN} -i ${ADDFILES_FILES})

        if (NOT TARGET format)
            add_custom_target(format DEPENDS ${ADDFILES_FORMATTARGET})
        else()
            add_dependencies(format DEPENDS ${ADDFILES_FORMATTARGET})
        endif()
    else()
        message(WARNING "Failed to find binary for clang-format, skipping")
    endif()
endfunction()

function(clangformat_globfiles)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs EXTENSIONS DIRECTORIES)
    cmake_parse_arguments(GLOBFILES "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})

    if(NOT GLOBFILES_EXTENSIONS)
        set(GLOBFILES_EXTENSIONS .hpp .cpp .h .c .cc)
    endif()
    if(NOT GLOBFILES_DIRECTORIES)
        set(GLOBFILES_DIRECTORIES ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/test ${CMAKE_SOURCE_DIR}/include)
    endif()


    foreach(extension ${GLOBFILES_EXTENSIONS})
        foreach(directory ${GLOBFILES_DIRECTORIES})
            list(APPEND globexprs "${directory}/*${extension}")
        endforeach()
    endforeach()

    file(GLOB FILES ${globexprs})
    clangformat_addfiles(FILES ${FILES})
endfunction()
