# Part of Beast

GroupSources(extras/beast extras)
GroupSources(include/beast beast)

GroupSources(examples "/")

add_executable (http-crawl
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    urls_large_data.hpp
    urls_large_data.cpp
    http_crawl.cpp
)

target_link_libraries(http-crawl Boost::boost Boost::system)

if (NOT WIN32)
    target_link_libraries(http-crawl Threads::Threads)
endif()

add_executable (http-server
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    file_body.hpp
    mime_type.hpp
    http_async_server.hpp
    http_sync_server.hpp
    http_server.cpp
)

target_link_libraries(http-server Boost::boost Boost::program_options Boost::system Boost::filesystem)

if (NOT WIN32)
    target_link_libraries(http-server Threads::Threads)
endif()


add_executable (http-example
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    http_example.cpp
)

target_link_libraries(http-example Boost::boost Boost::system)

if (NOT WIN32)
    target_link_libraries(http-example Threads::Threads)
endif()


add_executable (websocket-echo
    ${BEAST_INCLUDES}
    websocket_async_echo_server.hpp
    websocket_sync_echo_server.hpp
    websocket_echo.cpp
)

target_link_libraries(websocket-echo Boost::boost Boost::system)

if (NOT WIN32)
    target_link_libraries(websocket-echo Threads::Threads)
endif()


add_executable (websocket-example
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    websocket_example.cpp
)

target_link_libraries(websocket-example Boost::boost Boost::system)

if (NOT WIN32)
    target_link_libraries(websocket-example Threads::Threads)
endif()
