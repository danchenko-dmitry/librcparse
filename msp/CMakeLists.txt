cmake_minimum_required(VERSION 3.10)

if(NOT UART_RP_TEST_ROOT)
    get_filename_component(UART_RP_TEST_ROOT
        "${CMAKE_CURRENT_SOURCE_DIR}/../../uart-rp-test" ABSOLUTE)
endif()

if(NOT COMMAND set_common_compile_options)
    include(${UART_RP_TEST_ROOT}/cmake/CommonConfig.cmake)
endif()

add_etl_if_needed()

add_library(msp
    msp.cpp
    msp_callbacks.cpp
)

target_link_libraries(msp PUBLIC etl::etl msp_domain)

target_include_directories(msp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${UART_RP_TEST_ROOT}/domain/include
)

set_common_compile_options(msp)

add_executable(test_msp
    test_msp.cpp
)

target_link_libraries(test_msp msp)
set_common_compile_options(test_msp)

enable_testing()
add_test(NAME MSPTest COMMAND test_msp)
