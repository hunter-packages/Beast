# Part of Beast

add_executable (websocket-ssl-tests
    ../../../extras/beast/unit_test/main.cpp
    websocket_async_ssl_echo_server.hpp
    ssl_server.cpp
)

target_link_libraries(websocket-ssl-tests Beast OpenSSL::SSL)
