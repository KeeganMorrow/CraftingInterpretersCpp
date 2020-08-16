# Based off of this blog post
# https://crascit.com/2016/04/09/using-ccache-with-cmake/
function(ccache_setup)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    endif()
endfunction()
