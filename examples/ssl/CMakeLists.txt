# Part of Beast

GroupSources(extras/beast extras)
GroupSources(include/beast beast)

GroupSources(examples/ssl "/")

add_executable (http-ssl-example
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    http_ssl_example.cpp
)

target_link_libraries(http-ssl-example Boost::boost Boost::system OpenSSL::SSL)

if (NOT WIN32)
    target_link_libraries(http-ssl-example Threads::Threads)
endif()

add_executable (websocket-ssl-example
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    websocket_ssl_example.cpp
)

target_link_libraries(websocket-ssl-example Boost::boost Boost::system OpenSSL::SSL)

if (NOT WIN32)
    target_link_libraries(websocket-ssl-example Threads::Threads)
endif()
