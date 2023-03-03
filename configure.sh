# export CMAKE_TOOLCHAIN_FILE=/mnt/d/ws_wsl/vcpkg/scripts/buildsystems/vcpkg.cmake 

rm -r build
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/mnt/d/ws_wsl/vcpkg/scripts/buildsystems/vcpkg.cmake
make
./depth_zncc
