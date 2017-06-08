# Part of Beast

add_executable (http-ssl-example
    http_ssl_example.cpp
)

target_link_libraries(http-ssl-example Beast OpenSSL::SSL)

add_executable (websocket-ssl-example
    websocket_ssl_example.cpp
)

target_link_libraries(websocket-ssl-example Beast OpenSSL::SSSL)
