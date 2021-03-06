# Part of Beast

GroupSources(extras/beast extras)
GroupSources(include/beast beast)
GroupSources(test/http "/")

add_executable (http-tests
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    message_fuzz.hpp
    fail_parser.hpp
    ../../extras/beast/unit_test/main.cpp
    basic_dynabuf_body.cpp
    basic_fields.cpp
    basic_parser_v1.cpp
    concepts.cpp
    empty_body.cpp
    fields.cpp
    header_parser_v1.cpp
    message.cpp
    parse.cpp
    parse_error.cpp
    parser_v1.cpp
    read.cpp
    reason.cpp
    rfc7230.cpp
    streambuf_body.cpp
    string_body.cpp
    write.cpp
    chunk_encode.cpp
)

target_link_libraries(http-tests Boost::boost Boost::program_options Boost::filesystem Boost::system 
    Boost::coroutine Boost::thread Boost::context)

if (NOT WIN32)
    target_link_libraries(http-tests Threads::Threads)
endif()

add_executable (bench-tests
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    nodejs_parser.hpp
    ../../extras/beast/unit_test/main.cpp
    nodejs_parser.cpp
    parser_bench.cpp
)

target_link_libraries(bench-tests Boost::boost Boost::program_options Boost::filesystem Boost::system)
if (NOT WIN32)
    target_link_libraries(bench-tests Threads::Threads)
endif()
