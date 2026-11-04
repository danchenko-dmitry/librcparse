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

if(NOT EXISTS "${UART_RP_TEST_ROOT}/cmake/CommonConfig.cmake")
    message(FATAL_ERROR
        "UART_RP_TEST_ROOT does not look like uart-rp-test: ${UART_RP_TEST_ROOT}")
endif()

if(CRSF_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Use static libraries" FORCE)
    include(${UART_RP_TEST_ROOT}/cmake/CommonConfig.cmake)

    function(add_etl_if_needed)
        if(NOT TARGET etl::etl)
            add_subdirectory(${UART_RP_TEST_ROOT}/modules/etl etl)
        endif()
    endfunction()

    function(add_opentelemetry_if_needed)
        if(NOT TARGET opentelemetry_metrics)
            add_subdirectory(${UART_RP_TEST_ROOT}/modules/opentelemetry-cpp opentelemetry-cpp)
            add_custom_target(opentelemetry_proto_ready
                DEPENDS opentelemetry_proto opentelemetry_proto_grpc
                COMMENT "OpenTelemetry proto files generated and ready"
            )
        endif()
    endfunction()

    add_etl_if_needed()
    add_opentelemetry_if_needed()
    add_subdirectory(${UART_RP_TEST_ROOT}/domain domain)
endif()

add_subdirectory(packet_catcher)
add_subdirectory(crsf)
add_subdirectory(msp)
