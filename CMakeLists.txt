# Part of Beast

cmake_minimum_required (VERSION 3.5.2)

include(cmake/HunterGate.cmake)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.18.36.tar.gz"
    SHA1 "50eaca2a8083ac266bdec6655c64f352c380b784"
)

project(Beast VERSION 1.0.0)

option(Beast_BUILD_EXAMPLES "Build examples" OFF)
option(Beast_BUILD_TESTS "Build tests" OFF)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
    set (CMAKE_VERBOSE_MAKEFILE FALSE)

    add_definitions (-D_WIN32_WINNT=0x0601)
    add_definitions (-D_SCL_SECURE_NO_WARNINGS=1)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS=1)

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100 /wd4244 /MP /W4 /bigobj")
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ob2 /Oi /Ot /GL")
    set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Oi /Ot")

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

    set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic -Wno-unused-parameter")
endif()

#-------------------------------------------------------------------------------
#
# Boost
#

set(Boost_USE_MULTITHREADED ON)
add_definitions (-DBOOST_COROUTINES_NO_DEPRECATION_WARNING=1) # for asio
set(BOOST_COMPONENTS system)
if(Beast_BUILD_EXAMPLES OR Beast_BUILD_TESTS)
    list(APPEND BOOST_COMPONENTS coroutine context thread filesystem program_options)
endif()
hunter_add_package(Boost COMPONENTS ${BOOST_COMPONENTS})
find_package(Boost CONFIG REQUIRED ${BOOST_COMPONENTS})

#-------------------------------------------------------------------------------
#
# OpenSSL
#

if(Beast_BUILD_EXAMPLES OR Beast_BUILD_TESTS)
    hunter_add_package(OpenSSL)
    find_package(OpenSSL)
endif()

#
#-------------------------------------------------------------------------------

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
endif()

### Library ###
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

include_directories(.)
include_directories(include)
include_directories(extras)

if(Beast_BUILD_EXAMPLES)
    add_subdirectory (examples)
    add_subdirectory (examples/ssl)
endif()

if(Beast_BUILD_TESTS)
    add_subdirectory (test)
    add_subdirectory (test/core)
    add_subdirectory (test/http)
    add_subdirectory (test/websocket)
    add_subdirectory (test/websocket/ssl)
    add_subdirectory (test/zlib)
endif()
