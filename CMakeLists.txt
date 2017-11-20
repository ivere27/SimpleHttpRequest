# This file (c) 2017 AlertAvert.com.  All rights reserved.

project(SimpleHttpRequest)
cmake_minimum_required(VERSION 3.4)

# Version number
set(RELEASE_MAJOR 0)
set(RELEASE_MINOR 2)
set(RELEASE_PATCH 0)

# Use Conan for OpenSSL support.
include(.conan/conanbuildinfo.cmake)
conan_basic_setup()

set(RELEASE_STRING "${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_PATCH}-${BUILD_ID}")
message(STATUS "Building Release: ${RELEASE_STRING}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")

set(SRC ${CMAKE_SOURCE_DIR})

set(HTTP_PARSER ${SRC}/http-parser)
set(LIBUV ${SRC}/libuv)
set(OPENSSL ${SRC}/openssl)

include_directories(
        ${SRC}
        ${HTTP_PARSER}
        ${LIBUV}/include
        ${OPENSSL}/include
)

link_directories(${LIBUV}/.libs)
set(LIBS
        uv
        pthread
        crypto
)

#########
# Examples - demo program.
#########

add_executable(example
    ${SRC}/example.cpp
    ${HTTP_PARSER}/http_parser.c
)
target_link_libraries(example ${LIBS})
