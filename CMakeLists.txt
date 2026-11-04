cmake_minimum_required(VERSION 3.10)

# Packet parser plus the CRSF and MSP protocols.
# Consumed from uart-rp-test:
#   add_subdirectory(${CRSF_ROOT} ${CMAKE_BINARY_DIR}/crsf_project)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(librcparse CXX)
    set(CRSF_IS_TOP_LEVEL TRUE)
else()
    set(CRSF_IS_TOP_LEVEL FALSE)
endif()

if(NOT UART_RP_TEST_ROOT)
    get_filename_component(UART_RP_TEST_ROOT
        "${CMAKE_CURRENT_SOURCE_DIR}/../uart-rp-test" ABSOLUTE)
endif()

if(CRSF_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    if(NOT EXISTS "${UART_RP_TEST_ROOT}/modules/etl/CMakeLists.txt")
        message(FATAL_ERROR
            "ETL not found. Set UART_RP_TEST_ROOT to a uart-rp-test checkout: ${UART_RP_TEST_ROOT}")
    endif()

    function(set_common_compile_options target_name)
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Werror)
    endfunction()

    function(add_etl_if_needed)
        if(NOT TARGET etl::etl)
            add_subdirectory(${UART_RP_TEST_ROOT}/modules/etl etl)
        endif()
    endfunction()
endif()

add_subdirectory(packet_catcher)
add_subdirectory(crsf)
add_subdirectory(msp)

# OpenTelemetry export is optional and only built when the parent already
# provides monitoring_domain. The packet catcher itself does not use it.
if(TARGET monitoring_domain)
    add_subdirectory(monitoring)
endif()

if(CRSF_IS_TOP_LEVEL)
    add_subdirectory(crsf-stream-viewer)
endif()
