# Part of Beast

add_executable (echo-op
    echo_op.cpp
)

target_link_libraries(echo-op Beast)

add_executable (http-crawl
    urls_large_data.hpp
    urls_large_data.cpp
    http_crawl.cpp
)

target_link_libraries(http-crawl Beast)

add_executable (http-server
    file_body.hpp
    mime_type.hpp
    http_async_server.hpp
    http_sync_server.hpp
    http_server.cpp
)

target_link_libraries(http-server Beast Boost::program_options Boost::filesystem)


add_executable (http-example
    http_example.cpp
)

target_link_libraries(http-example Beast)


add_executable (websocket-echo
    websocket_async_echo_server.hpp
    websocket_sync_echo_server.hpp
    websocket_echo.cpp
)

target_link_libraries(websocket-echo Beast)


add_executable (websocket-example
    websocket_example.cpp
)

target_link_libraries(websocket-example Beast)
