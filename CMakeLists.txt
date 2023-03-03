cmake_minimum_required(VERSION 3.7)
project(depth_zncc VERSION 0.1.0)

include(CTest)
enable_testing()

set(SOURCES src/main.cpp src/utils/clchecks.cpp src/utils/datatools.cpp src/zncc/zncc.cpp)
add_executable(depth_zncc src/main.cpp ${SOURCES})

include("D:/ws/vcpkg/scripts/buildsystems/vcpkg.cmake")

# OpenCL
find_package(OpenCL REQUIRED)
target_link_libraries(depth_zncc OpenCL::OpenCL)

# LodePNG C++ API
find_package(lodepng REQUIRED)
target_link_libraries(depth_zncc lodepng)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")

set(CMAKE_GENERATOR_PLATFORM x64)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


add_definitions("-DCMAKE_TOOLCHAIN_FILE=D:/ws/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_GENERATOR_PLATFORM=x64")

add_custom_command(TARGET depth_zncc POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/src/kernels/ $<TARGET_FILE_DIR:depth_zncc>/kernels/)

add_custom_command(TARGET depth_zncc POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                           ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:depth_zncc>/data/)