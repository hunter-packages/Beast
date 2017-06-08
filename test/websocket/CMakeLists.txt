# Part of Beast

add_executable (websocket-tests
    ../../extras/beast/unit_test/main.cpp
    websocket_async_echo_server.hpp
    websocket_sync_echo_server.hpp
    error.cpp
    option.cpp
    rfc6455.cpp
    stream.cpp
    teardown.cpp
    frame.cpp
    mask.cpp
    utf8_checker.cpp
)

target_link_libraries(websocket-tests Beast)

if (MINGW)
    set_target_properties(websocket-tests PROPERTIES COMPILE_FLAGS "-Wa,-mbig-obj -Og")
endif()
