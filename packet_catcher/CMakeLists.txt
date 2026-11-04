cmake_minimum_required(VERSION 3.10)

if(NOT UART_RP_TEST_ROOT)
    get_filename_component(UART_RP_TEST_ROOT
        "${CMAKE_CURRENT_SOURCE_DIR}/../../uart-rp-test" ABSOLUTE)
endif()

if(NOT COMMAND set_common_compile_options)
    function(set_common_compile_options target_name)
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Werror)
    endfunction()
endif()

if(NOT COMMAND add_etl_if_needed)
    function(add_etl_if_needed)
        if(NOT TARGET etl::etl)
            add_subdirectory(${UART_RP_TEST_ROOT}/modules/etl etl)
        endif()
    endfunction()
endif()

add_etl_if_needed()

add_library(packet_catcher
    packet_catcher.cpp
)

target_link_libraries(packet_catcher PUBLIC etl::etl)

target_include_directories(packet_catcher
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../crsf
        ${CMAKE_CURRENT_SOURCE_DIR}/../msp
)

set_common_compile_options(packet_catcher)
