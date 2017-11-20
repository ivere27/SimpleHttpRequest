
project(SimpleHttpRequest)
cmake_minimum_required(VERSION 3.4)

# Version number
set(RELEASE_MAJOR 0)
set(RELEASE_MINOR 1)
set(RELEASE_PATCH 0)

# Uncomment the following lines to use Conan for OpenSSL support.
#
#  include(.conan/conanbuildinfo.cmake)
#  conan_basic_setup()

set(RELEASE_STRING "${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_PATCH}-${BUILD_ID}")
message(STATUS "Building Release: ${RELEASE_STRING}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")

set(SRC ${CMAKE_SOURCE_DIR})

set(HTTP_PARSER ${SRC}/http-parser)
set(LIBUV ${SRC}/libuv)

include_directories(
        ${SRC}
        ${HTTP_PARSER}
        ${LIBUV}/include
)

link_directories(
        ${LIBUV}/.libs
        ${HTTP_PARSER}
)
set(LIBS
        crypto
        http_parser
        pthread
        uv
)

#########
# Examples - demo program.
#########

add_executable(example
    ${SRC}/example.cpp
)
target_link_libraries(example ${LIBS})
