cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS On)

project(
    depth_zncc
    VERSION 0.1.0
    DESCRIPTION ""
    HOMEPAGE_URL "https://github.com/husmen/DepthZNCC/"
    LANGUAGES CXX C CUDA)

file(GLOB SOURCES src/utils/*.cpp src/zncc/*.cpp)
add_executable(depth_zncc src/main.cpp ${SOURCES})

set_property(TARGET depth_zncc PROPERTY CXX_STANDARD 17)

# set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -ccbin C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/")
# target_compile_options(depth_zncc PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:
#                         --ccbin="C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/"
#                         >)

# if(MSVC)
    # target_compile_options(depth_zncc PRIVATE /W4 /WX)
# else()
#     target_compile_options(depth_zncc PRIVATE -Wall -Wextra -pedantic -Werror)
# endif()

if(WIN32)
    include("D:/ws/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp:experimental /std:c++17")
    add_definitions("-DCMAKE_TOOLCHAIN_FILE=D:/ws/vcpkg/scripts/buildsystems/vcpkg.cmake")
    option(USE_OCL "Use OpenCL" ON)
    option(USE_SIMD "Use SIMD" ON)
endif()

if(UNIX)
    include("/mnt/d/ws/vcpkg_wsl/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -std=c++17")
    add_definitions("-DCMAKE_TOOLCHAIN_FILE=/mnt/d/ws/vcpkg_wsl/scripts/buildsystems/vcpkg.cmake")
    # add_definitions("-DCMAKE_TOOLCHAIN_FILE=/mnt/d/ws/vcpkg_wsl/scripts/buildsystems/vcpkg.cmake -DOpenCL_FOUND=True -DOpenCL_LIBRARY=/usr/local/cuda-11.7/lib64/libOpenCL.so")
    # add_definitions("-DCMAKE_TOOLCHAIN_FILE=/mnt/d/ws/vcpkg_wsl/scripts/buildsystems/vcpkg.cmake -DOpenCL_FOUND=True -DOpenCL_LIBRARY=/opt/intel/oclcpuexp_2023.15.3.0.20_rel/x64/libOpenCL.so")
    option(USE_SIMD "Use SIMD" ON)
endif()

# OpenCL
if(USE_OCL)
    find_package(OpenCL REQUIRED)
    target_link_libraries(depth_zncc OpenCL::OpenCL)
    add_compile_definitions(USE_OCL)
endif()

# OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(depth_zncc OpenMP::OpenMP_CXX)
endif()

if(USE_SIMD)
    add_compile_definitions(USE_SIMD)
endif()

# LodePNG C++ API
find_package(lodepng REQUIRED)
target_link_libraries(depth_zncc lodepng)

# Other includes (tqdm, )
# target_include_directories(depth_zncc PRIVATE "include/")


# Testing
# include(CTest)
# enable_testing()


# CPack
include(CPack)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

add_custom_command(TARGET depth_zncc POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/src/kernels/ $<TARGET_FILE_DIR:depth_zncc>/kernels/)

add_custom_command(TARGET depth_zncc POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_directory
                           ${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:depth_zncc>/data/)
