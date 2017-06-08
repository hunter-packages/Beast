# Part of Beast

add_executable (http-tests
    message_fuzz.hpp
    test_parser.hpp
    ../../extras/beast/unit_test/main.cpp
    basic_parser.cpp
    buffer_body.cpp
    doc_http_samples.cpp
    doc_snippets.cpp
    dynamic_body.cpp
    empty_body.cpp
    error.cpp
    field.cpp
    fields.cpp
    message.cpp
    parser.cpp
    read.cpp
    rfc7230.cpp
    serializer.cpp
    status.cpp
    string_body.cpp
    type_traits.cpp
    verb.cpp
    write.cpp
)

target_include_directories(http-tests PRIVATE ../../examples)

target_link_libraries(http-tests Beast)

add_executable (http-bench
    nodejs_parser.hpp
    message_fuzz.hpp
    ../../extras/beast/unit_test/main.cpp
    nodejs_parser.cpp
    parser_bench.cpp
)

target_link_libraries(http-bench Beast)