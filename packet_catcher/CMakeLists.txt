cmake_minimum_required(VERSION 3.10)

if(NOT UART_RP_TEST_ROOT)
    get_filename_component(UART_RP_TEST_ROOT
        "${CMAKE_CURRENT_SOURCE_DIR}/../../uart-rp-test" ABSOLUTE)
endif()

if(NOT COMMAND set_common_compile_options)
    include(${UART_RP_TEST_ROOT}/cmake/CommonConfig.cmake)
endif()

add_etl_if_needed()

add_library(packet_catcher
    packet_catcher.cpp
    instrumentation.cpp
)

target_link_libraries(packet_catcher PUBLIC etl::etl monitoring_domain)

target_include_directories(packet_catcher
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../crsf
        ${CMAKE_CURRENT_SOURCE_DIR}/../msp
        ${UART_RP_TEST_ROOT}/domain
)

set_common_compile_options(packet_catcher)
