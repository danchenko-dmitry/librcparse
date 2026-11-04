cmake_minimum_required(VERSION 3.10)

add_library(packet_catcher_monitoring
    instrumentation.cpp
)

target_link_libraries(packet_catcher_monitoring PUBLIC packet_catcher monitoring_domain)

target_include_directories(packet_catcher_monitoring
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_common_compile_options(packet_catcher_monitoring)
