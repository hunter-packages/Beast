# Part of Beast

cmake_minimum_required (VERSION 3.5.2)

include(cmake/HunterGate.cmake)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.37.tar.gz"
    SHA1 "113baeaf553bc805279db2dfa001950d74557aba"
)

project (Beast VERSION 84)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)
option (Beast_BUILD_EXAMPLES "Build examples" OFF)
option (Beast_BUILD_TESTS "Build tests" OFF)

if (MSVC)
    set (CMAKE_VERBOSE_MAKEFILE FALSE)

    add_definitions (-D_WIN32_WINNT=0x0601)
    add_definitions (-D_SCL_SECURE_NO_WARNINGS=1)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS=1)

    set (Boost_USE_STATIC_LIBS ON)
    set (Boost_USE_STATIC_RUNTIME ON)

    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4 /bigobj /permissive-")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
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
    set (THREADS_PREFER_PTHREAD_FLAG ON)
    find_package (Threads)

    set( CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic -Wno-unused-parameter")

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wrange-loop-analysis")
    endif ()
endif()

#-------------------------------------------------------------------------------
#
# Boost
#

option (Boost_USE_STATIC_LIBS "Use static libraries for boost" ON)

set(BOOST_COMPONENTS system)
if (Beast_BUILD_EXAMPLES OR Beast_BUILD_TESTS)
    list(APPEND BOOST_COMPONENTS coroutine context filesystem program_options thread)
endif()
hunter_add_package(Boost COMPONENTS ${BOOST_COMPONENTS})
find_package (Boost CONFIG REQUIRED ${BOOST_COMPONENTS})

link_directories(${Boost_LIBRARY_DIRS})

if (MINGW)
    link_libraries(ws2_32 mswsock)
endif()

#-------------------------------------------------------------------------------
#
# OpenSSL
#

# Only used for examples and tests
if(Beast_BUILD_EXAMPLES OR Beast_BUILD_TESTS)
    hunter_add_package(OpenSSL)
    find_package(OpenSSL REQUIRED)
    add_definitions (-DBEAST_USE_OPENSSL=1)
else()
    add_definitions (-DBEAST_USE_OPENSSL=0)
endif()

#
#-------------------------------------------------------------------------------

function(DoGroupSources curdir rootdir folder)
    file (GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
    foreach (child ${children})
        if (IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
            DoGroupSources(${curdir}/${child} ${rootdir} ${folder})
        elseif (${child} STREQUAL "CMakeLists.txt")
            source_group("" FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        else()
            string(REGEX REPLACE ^${rootdir} ${folder} groupname ${curdir})
            string(REPLACE "/" "\\" groupname ${groupname})
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        endif()
    endforeach()
endfunction()

function(GroupSources curdir folder)
    DoGroupSources (${curdir} ${curdir} ${folder})
endfunction()

#-------------------------------------------------------------------------------

if ("${VARIANT}" STREQUAL "coverage")
    if (MSVC)
    else()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2 -fprofile-arcs -ftest-coverage")
        set (CMAKE_BUILD_TYPE RELWITHDEBINFO)
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov")
    endif()

elseif ("${VARIANT}" STREQUAL "ubasan")
    if (MSVC)
    else()
        set (CMAKE_CXX_FLAGS
          "${CMAKE_CXX_FLAGS} -DBEAST_NO_SLOW_TESTS=1 -msse4.2 -funsigned-char -fno-omit-frame-pointer -fsanitize=address,undefined -fsanitize-blacklist=${PROJECT_SOURCE_DIR}/scripts/blacklist.supp")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
        set (CMAKE_BUILD_TYPE RELWITHDEBINFO)
    endif()

elseif ("${VARIANT}" STREQUAL "debug")
    set (CMAKE_BUILD_TYPE DEBUG)

elseif ("${VARIANT}" STREQUAL "release")
    set (CMAKE_BUILD_TYPE RELEASE)

endif()

#-------------------------------------------------------------------------------
#
# Library interface
#

add_library (${PROJECT_NAME} INTERFACE)
target_link_libraries (${PROJECT_NAME} INTERFACE Boost::system)
if (NOT MSVC)
    target_link_libraries (${PROJECT_NAME} INTERFACE Threads::Threads)
endif()
target_compile_definitions (${PROJECT_NAME} INTERFACE BOOST_COROUTINES_NO_DEPRECATION_WARNING=1)

# Install

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

#-------------------------------------------------------------------------------
#
# Tests and examples
#

include_directories (.)
include_directories (extras)
include_directories (include)

if (OPENSSL_FOUND)
    include_directories (${OPENSSL_INCLUDE_DIR})
endif()

file(GLOB_RECURSE BEAST_INCLUDES
    ${PROJECT_SOURCE_DIR}/include/beast/*.hpp
    ${PROJECT_SOURCE_DIR}/include/beast/*.ipp
)

file(GLOB_RECURSE COMMON_INCLUDES
    ${PROJECT_SOURCE_DIR}/example/common/*.hpp
  )

file(GLOB_RECURSE EXAMPLE_INCLUDES
    ${PROJECT_SOURCE_DIR}/example/*.hpp
  )

file(GLOB_RECURSE EXTRAS_INCLUDES
    ${PROJECT_SOURCE_DIR}/extras/beast/*.hpp
    ${PROJECT_SOURCE_DIR}/extras/beast/*.ipp
)

if (Beast_BUILD_TESTS)
    add_subdirectory (test)
endif()

if (Beast_BUILD_EXAMPLES AND
    (NOT "${VARIANT}" STREQUAL "coverage") AND
    (NOT "${VARIANT}" STREQUAL "ubasan"))
    add_subdirectory (example)
endif()
