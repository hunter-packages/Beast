# Part of Beast

GroupSources(extras/beast extras)
GroupSources(include/beast beast)
GroupSources(test/core "/")

add_executable (core-tests
    ${BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    ../../extras/beast/unit_test/main.cpp
    buffer_test.hpp
    async_completion.cpp
    bind_handler.cpp
    buffer_cat.cpp
    buffer_concepts.cpp
    buffers_adapter.cpp
    clamp.cpp
    consuming_buffers.cpp
    dynabuf_readstream.cpp
    error.cpp
    handler_alloc.cpp
    handler_concepts.cpp
    handler_ptr.cpp
    placeholders.cpp
    prepare_buffer.cpp
    prepare_buffers.cpp
    static_streambuf.cpp
    static_string.cpp
    stream_concepts.cpp
    streambuf.cpp
    to_string.cpp
    write_dynabuf.cpp
    base64.cpp
    empty_base_optimization.cpp
    get_lowest_layer.cpp
    is_call_possible.cpp
    sha1.cpp
)

target_link_libraries(core-tests Boost::boost Boost::program_options Boost::filesystem Boost::system 
        Boost::coroutine Boost::thread Boost::context)

if (NOT WIN32)
    target_link_libraries(core-tests Threads::Threads)
endif()
