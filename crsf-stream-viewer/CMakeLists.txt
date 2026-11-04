cmake_minimum_required(VERSION 3.10)

add_executable(crsf-stream-viewer main.cpp)
target_link_libraries(crsf-stream-viewer PRIVATE crsf)
set_common_compile_options(crsf-stream-viewer)
