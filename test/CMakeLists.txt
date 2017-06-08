# Part of Beast

add_executable (lib-tests
    ../extras/beast/unit_test/main.cpp
    config.cpp
    core.cpp
    http.cpp
    version.cpp
    websocket.cpp
    zlib.cpp
)

target_link_libraries(lib-tests Beast)

if (MINGW)
    set_target_properties(lib-tests PROPERTIES COMPILE_FLAGS "-Wa,-mbig-obj")
endif()
