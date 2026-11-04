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

add_library(msp
    msp.cpp
    msp_callbacks.cpp
)

target_include_directories(msp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${UART_RP_TEST_ROOT}/domain
        ${UART_RP_TEST_ROOT}/domain/include
)

if(TARGET msp_domain)
    target_link_libraries(msp PUBLIC etl::etl msp_domain)
else()
    target_sources(msp PRIVATE ${UART_RP_TEST_ROOT}/domain/msp_errors.cpp)
    target_link_libraries(msp PUBLIC etl::etl)
endif()

set_common_compile_options(msp)

add_executable(test_msp
    test_msp.cpp
)

target_link_libraries(test_msp msp)
set_common_compile_options(test_msp)

enable_testing()
add_test(NAME MSPTest COMMAND test_msp)
