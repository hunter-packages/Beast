# Part of Beast

cmake_minimum_required (VERSION 3.2)

include(cmake/HunterGate.cmake)

HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.18.36.tar.gz"
    SHA1 "50eaca2a8083ac266bdec6655c64f352c380b784"
)

project(Beast VERSION 1.0.0)

option(Beast_BUILD_EXAMPLES "Build examples" OFF)
option(Beast_BUILD_TESTS "Build tests" OFF)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    # /wd4244 /wd4127
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100 /wd4244 /wd4251 /MP /W4 /bigobj /D BOOST_COROUTINES_NO_DEPRECATION_WARNING=1 /D _WIN32_WINNT=0x0601 /D _SCL_SECURE_NO_WARNINGS=1 /D _CRT_SECURE_NO_WARNINGS=1")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ob2 /Oi /Ot /GL /MT")
    set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Oi /Ot /MT")

    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")

    # for RelWithDebInfo builds, disable incremental linking
    # since CMake sets it ON by default for that build type and it
    # causes warnings
    #
    string (REPLACE "/INCREMENTAL" "/INCREMENTAL:NO" replacement_flags
        ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO})
    set (CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO ${replacement_flags})

else()
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads)

    set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic -Wno-unused-parameter -DBOOST_COROUTINES_NO_DEPRECATION_WARNING=1")
endif()

set(Boost_USE_MULTITHREADED ON)
set(BOOST_COMPONENTS system)
if (Beast_BUILD_EXAMPLES OR Beast_BUILD_TESTS)
    list(APPEND BOOST_COMPONENTS coroutine context thread filesystem program_options)
endif()
hunter_add_package(Boost COMPONENTS ${BOOST_COMPONENTS})
find_package(Boost CONFIG REQUIRED ${BOOST_COMPONENTS})

add_definitions ("-DBOOST_COROUTINES_NO_DEPRECATION_WARNING")

if(Beast_BUILD_EXAMPLES)
    hunter_add_package(OpenSSL)
    find_package(OpenSSL)
endif(Beast_BUILD_EXAMPLES)

if (MINGW)
    link_libraries(ws2_32 mswsock)
endif()

if ("${VARIANT}" STREQUAL "coverage")
    set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov")
elseif ("${VARIANT}" STREQUAL "asan")
    set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
elseif ("${VARIANT}" STREQUAL "usan")
    set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-omit-frame-pointer")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
    set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
elseif ("${VARIANT}" STREQUAL "debug")
    set(CMAKE_BUILD_TYPE DEBUG)
elseif ("${VARIANT}" STREQUAL "release")
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

function(DoGroupSources curdir rootdir folder)
    file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
            DoGroupSources(${curdir}/${child} ${rootdir} ${folder})
        elseif(${child} STREQUAL "CMakeLists.txt")
            source_group("" FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        else()
            string(REGEX REPLACE ^${rootdir} ${folder} groupname ${curdir})
            string(REPLACE "/" "\\" groupname ${groupname})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        endif()
    endforeach()
endfunction()

function(GroupSources curdir folder)
    DoGroupSources(${curdir} ${curdir} ${folder})
endfunction()

if(Beast_BUILD_TESTS)
    include_directories (extras)
endif(Beast_BUILD_TESTS)

include_directories (include)

if(Beast_BUILD_TESTS)
    set(ZLIB_SOURCES
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/crc32.h
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/deflate.h
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/inffast.h
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/inffixed.h
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/inflate.h
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/inftrees.h
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/trees.h
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/zlib.h
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/zutil.h
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/adler32.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/compress.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/crc32.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/deflate.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/infback.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/inffast.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/inflate.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/inftrees.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/trees.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/uncompr.c
        ${PROJECT_SOURCE_DIR}/test/zlib/zlib-1.2.8/zutil.c
    )
endif(Beast_BUILD_TESTS)

file(GLOB_RECURSE BEAST_INCLUDES
    ${PROJECT_SOURCE_DIR}/include/beast/*.hpp
    ${PROJECT_SOURCE_DIR}/include/beast/*.ipp
)

file(GLOB_RECURSE EXTRAS_INCLUDES
    ${PROJECT_SOURCE_DIR}/extras/beast/*.hpp
    ${PROJECT_SOURCE_DIR}/extras/beast/*.ipp
)

if(Beast_BUILD_EXAMPLES)
    add_subdirectory (examples)
    if (NOT OPENSSL_FOUND)
        message("OpenSSL not found. Not building examples/ssl")
    else()
        add_subdirectory (examples/ssl)
    endif()
endif(Beast_BUILD_EXAMPLES)

if(Beast_BUILD_TESTS)
    add_subdirectory (test)
    add_subdirectory (test/core)
    add_subdirectory (test/http)
    add_subdirectory (test/websocket)
    add_subdirectory (test/zlib)
endif(Beast_BUILD_TESTS)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE Boost::system)

### Install ###
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Note: use 'targets_export_name'
configure_file("cmake/Config.cmake.in" "${project_config}" @ONLY)

install(
    TARGETS ${PROJECT_NAME}
    EXPORT "${targets_export_name}"
    INCLUDES DESTINATION "${include_install_dir}"
)

install(
    DIRECTORY "include/"
    DESTINATION "${include_install_dir}"
)

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
