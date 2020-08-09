function(clangtidy_addtarget target)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs "")
    cmake_parse_arguments(ADDTARGET "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN})
    get_target_property(TARGET_SOURCES ${target} SOURCES)
    get_target_property(TARGET_SOURCEDIR ${target} SOURCE_DIR)

    foreach(source ${TARGET_SOURCES})
            list(APPEND MYSOURCES "${source}")
    endforeach()

    find_program(RUN_CLANG_TIDY_BIN run-clang-tidy.py /usr/share/clang)
    find_program(CLANGTIDY_BIN clang-tidy clangtidy-11 clangtidy-10 clangtidy-9 clangtidy-8)

    if(RUN_CLANG_TIDY_BIN)
        add_custom_target(clangtidy_${target} WORKING_DIRECTORY ${TARGET_SOURCEDIR} COMMAND ${RUN_CLANG_TIDY_BIN} ${MYSOURCES} -header-filter='${TARGET_SOURCEDIR}/src/.*|${TARGET_SOURCEDIR}/test/.*|${TARGET_SOURCEDIR}/include/.*')
    elseif(CLANGTIDY_BIN)
        add_custom_target(clangtidy_${target} WORKING_DIRECTORY ${TARGET_SOURCEDIR} COMMAND ${CLANGTIDY_BIN} ${MYSOURCES} -header-filter='${TARGET_SOURCEDIR}/src/.*|${TARGET_SOURCEDIR}/test/.*|${TARGET_SOURCEDIR}/include/.*')
    else()
        message(WARNING "Failed to find binary for clang-tidy, skipping")
    endif()

    if(NOT TARGET clangtidy AND TARGET clangtidy_${target})
        add_custom_target(clangtidy DEPENDS clangtidy_${target})
        if (NOT TARGET lint)
            add_custom_target(lint DEPENDS clangtidy)
        else()
            add_dependencies(lint DEPENDS clangtidy)
        endif()
    else()
        add_dependencies(clangtidy DEPENDS clangtidy_${target})
    endif()


endfunction()
