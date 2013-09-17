cmake_minimum_required(VERSION 2.8)

project(msgpack-c-len C)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_C_FLAGS " -std=c99 -O3 -Wall -Wextra -Wimplicit")

OPTION(TESTS "enable_testing" ON)

add_library(msgpack-c-len STATIC msglen.c)
install(TARGETS msgpack-c-len DESTINATION lib)

if(TESTS)
    find_package(CUnit REQUIRED)
    include_directories(${CUNIT_INCLUDE_DIR})

    set(CASES_MPAC "${CMAKE_CURRENT_SOURCE_DIR}/cases.mpac")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.h.t ${CMAKE_CURRENT_BINARY_DIR}/test.h)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    add_executable(msglen_test test.c)
    add_dependencies(msglen_test msgpack-c-len)
    target_link_libraries(msglen_test
        msgpack-c-len
        ${CUNIT_LIBRARIES}
    )
    set(test_target msglen_test)
    add_custom_target(test)
    add_custom_target(${test_target}_runtest
        COMMAND ${test_target}
        DEPENDS ${test_target}
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    add_dependencies(test ${test_target}_runtest)
endif(TESTS)
