# Part of Beast

GroupSources(extras/beast extras)
GroupSources(include/beast beast)
GroupSources(test "/")

add_executable (lib-tests
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    ../extras/beast/unit_test/main.cpp
    core.cpp
    http.cpp
    version.cpp
    websocket.cpp
    zlib.cpp
)

target_link_libraries(lib-tests Boost::boost Boost::program_options Boost::system)

if (NOT WIN32)
    target_link_libraries(lib-tests Threads::Threads)
endif()

if (MINGW)
    set_target_properties(lib-tests PROPERTIES COMPILE_FLAGS "-Wa,-mbig-obj")
endif()
