cmake_minimum_required(VERSION 3.10)

if(NOT UART_RP_TEST_ROOT)
    get_filename_component(UART_RP_TEST_ROOT
        "${CMAKE_CURRENT_SOURCE_DIR}/../../uart-rp-test" ABSOLUTE)
endif()

if(NOT COMMAND set_common_compile_options)
    include(${UART_RP_TEST_ROOT}/cmake/CommonConfig.cmake)
endif()

add_etl_if_needed()

add_library(crsf
    crsf.cpp
    crsf_callbacks.cpp
    crc.cpp
)

target_link_libraries(crsf PUBLIC etl::etl packet_catcher)

target_include_directories(crsf
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_common_compile_options(crsf)

add_executable(test_crsf
    test_crsf.cpp
)

target_link_libraries(test_crsf crsf)
set_common_compile_options(test_crsf)

enable_testing()
add_test(NAME CRSFTest COMMAND test_crsf)
