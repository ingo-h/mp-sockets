# Copyright (C) 2021 GPL 3 and higher by Ingo Höft,  <Ingo@Hoeft-online.de>
# Redistribution only with this Copyright remark. Last modified: 2022-01-06

# Configure and build with:
# cmake -S . -B build -D CMAKE_BUILD_TYPE=MinSizeRel
# cmake --build build --config MinSizeRel

cmake_minimum_required(VERSION 3.18)
# Version 3.18 is the version from the Debian 11 (bullseye) repository.

# This is a guard that you do not build direct in the source tree.
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    message ("In-source builds are not supported.")
    message ("Use a separate folder for building, for example:\n")
    message ("    cmake -S . -B build/")
    message ("    cmake --build build/\n")
    message ("Before that, remove the files already created:\n")
    message ("    rm -rf CMakeCache.txt CMakeFiles\n")
    message (FATAL_ERROR "")
endif()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set the project name and version
project(MP_SOCKETS VERSION 0001
        DESCRIPTION "Proof of multiplatform gtests for TCP socket connections."
        HOMEPAGE_URL "https://github.com/ingo-h/mp-sockets")


#################################
# Googletest                    #
#################################
# Download, configure and build
include(FetchContent)

message(CHECK_START "Download and configuring GoogleTest")

FetchContent_Declare(
    googletest
    GIT_REPOSITORY    https://github.com/google/googletest.git
    GIT_TAG           origin/main
    GIT_SHALLOW       ON
)
# This caches googletest_SOURCE_DIR, googletest_BINARY_DIR and googletest_POPULATED.
FetchContent_MakeAvailable(googletest)


#################################
# Build the Socket Server       #
#################################
add_executable(socket_server
    ${PROJECT_SOURCE_DIR}/socket_server.cpp)


#################################
# Build the Unit Tests          #
#################################
enable_testing()

include_directories(
    ${googletest_SOURCE_DIR}/googletest/include/
    ${googletest_SOURCE_DIR}/googlemock/include/
)
link_libraries(${PROJECT_BINARY_DIR}/lib/libgtest.so
               ${PROJECT_BINARY_DIR}/lib/libgmock.so
)

add_executable(test_sock
    ${PROJECT_SOURCE_DIR}/test_sock.cpp)
add_test(NAME ctest_sock COMMAND test_sock)
