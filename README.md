# DepthZNCC
OpenCL implementation of Zero-mean Normalized Cross Correlation (ZNCC) - Project for 521288S Multiprocessor Programming, Spring 2023, @UniOulu

## Notes
- [Middlebury Stereo Datasets](https://vision.middlebury.edu/stereo/data/)
- Project structure and configuration mixes guidlines from [Canonical Project Structure](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html), [pitchfork](https://github.com/vector-of-bool/pitchfork) and [cmake_template](https://github.com/cpp-best-practices/cmake_template)

## How to

## Requirements
- C++17
- LodePNG
- OpenMP
- OpenCL
- CUDA

## Benchmarks:

- resizeFactor: 1
- winSize: 9
- maxDisp: 32
- ccThresh: 32
- occThresh = 16

| Method ⬇️\Resolution ➡️ | 2940x2016 | 1470x1008 | 735x504 |
|-----|-----|-----|-----|
| Single-Threaded | 348 | - | - |
| Multi-Threaded | 50.7 | - | - |
| OpenMP | 51.8 | - | - |
| SIMD | 15.4 | - | - |
| OpenCL (GPU) | 4.1 | - | - |
| OpenCL (APU) | 4.0 | - | - |
| OpenCL (CPU) | 11.6 | - | - |
| OpenCL Optimized (GPU) | 2.2 | - | - |
| OpenCL Optimized (APU) | 1.8 | - | - |
| OpenCL Optimized (CPU) | DNR | - | - |
| CUDA | - | - | - |
[Benchmark results as runtime in seconds]

## OpenCL Platform/Device Info

```yaml
### AMD APP
Platform summary:
Name: AMD Accelerated Parallel Processing
Vendor: Advanced Micro Devices, Inc.
Version: OpenCL 2.1 AMD-APP (3516.0)

Device summary:
Name: gfx90c
Max compute units: 8
Global memory size (bytes): 12980584448
Max work group size: 256
Kernel work group size: 14757395258967641292

### NVIDIA CUDA
Platform summary:
Name: NVIDIA CUDA
Vendor: NVIDIA Corporation
Version: OpenCL 3.0 CUDA 12.0.94

Device summary:
Name: NVIDIA GeForce RTX 3050 Laptop GPU
Max compute units: 16
Global memory size (bytes): 4294443008
Max work group size: 1024
Kernel work group size: 14757395258967641292

### Intel OpenCL
Platform summary:
Name: Intel(R) OpenCL
Vendor: Intel(R) Corporation
Version: OpenCL 3.0 WINDOWS

Device summary:
Name: AMD Ryzen 7 5800HS with Radeon Graphics
Max compute units: 16
Global memory size (bytes): 33721454592
Max work group size: 8192
Kernel work group size: 14757395258967641292
```

## TODO
- [x] ZNCC single-threaded.
- [X] ZNCC multi-threaded.
- [X] ZNCC OpenMP.
- [X] ZNCC SIMD.
- [X] ZNCC OpenCL.
- [X] ZNCC OpenCL optimization.
- [X] Benchmarking all implementations.
- [ ] Advanced profiling (Orbit?)
- [ ] ZNCC CUDA (optional).
- [ ] Unit tests (optional).
- [ ] Automatic data downloader (optional).

## Development diary
- **Day 1**: Wasted a whole day trying to run OpenCL (CUDA version) on WSL, turns out, that's not supported.
- **Day 2**: Setup OpenCL environment correctly on Windows, including
    - NVIDIA GPU support.
    - `vcpkg` package manager.
    - ~~`conan` package manager.~~
    - `cmake` build system.
    - ~~`meson` build system.~~
    - Hello world for OpenCL.
- **Day 3**: 
    - First OpenCL kernel.
    - PNG image loading with `LodePNG`.
    - Improved the project's structure.
- **Day 4**: Initial ZNCC implementation, naive and single threaded on CPU.
- **Day 5**: C++ Multithreading and OpenMP implementations.
- **Day 6**: Learning more about OpenCL.
- **Day 7**: Initial OpenCL implementation of ZNCC, mostly a copy paste of the C++ code with minor editing and a kernel wrapper.
- **Day 8**: Researching the topic of profiling and tracing OpenCL programs, the ecosystem looks like a mess!
- **Day 9**: Some refactoring and learning more about SIMD.
- **Day 10**: SIMD implementation, with better loop structures to take data locality into account.
- **Day 11**: Reworking CMake configuration to solve OpenCL issues on WSL, SIMD issues on Windows.
- **Day 12**: Improved and Optimized OpenCL implementation, based on ideas from the SIMD implementation.
- **Day 13**:
- **Day 14**:

## References

### OpenMP
- [Intro to GPU Programming with the OpenMP API](https://www.openmp.org/wp-content/uploads/2021-10-20-Webinar-OpenMP-Offload-Programming-Introduction.pdf)
- [OpenMP for GPU Offloading](https://enccs.github.io/openmp-gpu/)
- [OpenMP in Visual C++](https://learn.microsoft.com/en-us/cpp/parallel/openmp/openmp-in-visual-cpp?view=msvc-170)

### OpenCL
- [OpenCL](https://www.khronos.org/opencl/)
- [Hands On OpenCL](https://handsonopencl.github.io/)
- [The OpenCL Memory Hierarchy](https://comp.anu.edu.au/courses/acceleratorsHPC/slides/OpenCLMemory.pdf)
- [Parallel Musings](https://bashbaug.github.io/)
- [OpenCL - LinuxReviews](https://linuxreviews.org/OpenCL)

### CUDA
- [CUDA Refresher](https://developer.nvidia.com/blog/tag/cuda-refresher/)
- [CUDA C++ Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)
- [CUDA C++ Best Practices Guide](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/)
- [NVIDIA Ampere Architecture In-Depth](https://developer.nvidia.com/blog/nvidia-ampere-architecture-in-depth/)
- [GPU Performance Background User's Guide](https://docs.nvidia.com/deeplearning/performance/dl-performance-gpu-background/index.html)

### OpenCL 2.0/3.0 Implementations
- [Khronos OpenCL-SDK](https://github.com/KhronosGroup/OpenCL-SDK): generic, requires vendor ICD
- [NVIDIA CUDA](https://developer.nvidia.com/cuda-toolkit)
- [AMD ROCm](https://github.com/RadeonOpenCompute/ROCm)
    - [AMD Software: Adrenaline Edition](https://www.amd.com/en/technologies/software): AMD GPUs & APUs
    - [AMD OpenCL SDK](https://github.com/GPUOpen-LibrariesAndSDKs/OCL-SDK): unmaintained
- [Intel OpenCL Runtimes](https://www.intel.com/content/www/us/en/developer/articles/tool/opencl-drivers.html)
    - Intel OpenCL CPU Runtime [Github](https://github.com/intel/llvm/releases/tag/2023-WW13) | [Intel](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-cpu-runtime-for-opencl-applications-with-sycl-support.html): oneAPI DPC++, x64 CPUs including AMD
    - [Intel OpenCL Runtime](https://github.com/intel/compute-runtime): iGPU
    - [Beignet](https://github.com/intel/beignet): unmaintained, old Intel CPUs
- [POCL](http://portablecl.org/): generic

### OpenCL Profiling
- [clinfo](https://github.com/Oblomov/clinfo)
- [Oclgrind](https://github.com/jrprice/Oclgrind): An OpenCL device simulator and debugger
- [OCL Intercept](https://github.com/intel/opencl-intercept-layer): Intercept Layer for Debugging and Analyzing OpenCL Applications
- [GPUPerfAPI](https://gpuopen.com/gpuperfapi/): The GPU Performance API 
- [Radeon Developer Tool Suit](https://gpuopen.com/tools/): Radeon GPU Profiler, Analyzer & Memory Visualizer

```bash
> clinfo.exe
Number of platforms:                             3
  Platform Profile:                              FULL_PROFILE
  Platform Version:                              OpenCL 2.1 AMD-APP (3516.0)
  Platform Name:                                 AMD Accelerated Parallel Processing
  Platform Vendor:                               Advanced Micro Devices, Inc.
  Platform Extensions:                           cl_khr_icd cl_khr_d3d10_sharing cl_khr_d3d11_sharing cl_khr_dx9_media_sharing cl_amd_event_callback cl_amd_offline_devices
  Platform Profile:                              FULL_PROFILE
  Platform Version:                              OpenCL 3.0 CUDA 12.0.94
  Platform Name:                                 NVIDIA CUDA
  Platform Vendor:                               NVIDIA Corporation
  Platform Extensions:                           cl_khr_global_int32_base_atomics cl_khr_global_int32_extended_atomics cl_khr_local_int32_base_atomics cl_khr_local_int32_extended_atomics cl_khr_fp64 cl_khr_3d_image_writes cl_khr_byte_addressable_store cl_khr_icd cl_khr_gl_sharing cl_nv_compiler_options cl_nv_device_attribute_query cl_nv_pragma_unroll cl_nv_d3d10_sharing cl_khr_d3d10_sharing cl_nv_d3d11_sharing cl_nv_copy_opts cl_nv_create_buffer cl_khr_int64_base_atomics cl_khr_int64_extended_atomics cl_khr_device_uuid cl_khr_pci_bus_info cl_khr_external_semaphore cl_khr_external_memory cl_khr_external_semaphore_win32 cl_khr_external_memory_win32
  Platform Profile:                              FULL_PROFILE
  Platform Version:                              OpenCL 3.0 WINDOWS
  Platform Name:                                 Intel(R) OpenCL
  Platform Vendor:                               Intel(R) Corporation
  Platform Extensions:                           cl_khr_spirv_linkonce_odr cl_khr_fp64 cl_khr_icd cl_khr_global_int32_base_atomics cl_khr_global_int32_extended_atomics cl_khr_local_int32_base_atomics cl_khr_local_int32_extended_atomics cl_khr_int64_base_atomics cl_khr_int64_extended_atomics cl_khr_byte_addressable_store cl_khr_depth_images cl_khr_3d_image_writes cl_khr_il_program cl_intel_unified_shared_memory_preview cl_intel_device_attribute_query cl_intel_subgroups cl_intel_subgroups_char cl_intel_subgroups_short cl_intel_subgroups_long cl_intel_spirv_subgroups cl_intel_required_subgroup_size cl_intel_exec_by_local_thread cl_intel_vec_len_hint cl_khr_spir cl_khr_image2d_from_buffer


  Platform Name:                                 AMD Accelerated Parallel Processing
Number of devices:                               1
  Device Type:                                   CL_DEVICE_TYPE_GPU
  Vendor ID:                                     1002h
  Board name:                                    AMD Radeon(TM) Graphics
  Device Topology:                               PCI[ B#4, D#0, F#0 ]
  Max compute units:                             8
  Max work items dimensions:                     3
    Max work items[0]:                           1024
    Max work items[1]:                           1024
    Max work items[2]:                           1024
  Max work group size:                           256
  Preferred vector width char:                   4
  Preferred vector width short:                  2
  Preferred vector width int:                    1
  Preferred vector width long:                   1
  Preferred vector width float:                  1
  Preferred vector width double:                 1
  Native vector width char:                      4
  Native vector width short:                     2
  Native vector width int:                       1
  Native vector width long:                      1
  Native vector width float:                     1
  Native vector width double:                    1
  Max clock frequency:                           2000Mhz
  Address bits:                                  64
  Max memory allocation:                         10577156505
  Image support:                                 Yes
  Max number of images read arguments:           128
  Max number of images write arguments:          64
  Max image 2D width:                            16384
  Max image 2D height:                           16384
  Max image 3D width:                            2048
  Max image 3D height:                           2048
  Max image 3D depth:                            2048
  Max samplers within kernel:                    16
  Max size of kernel argument:                   1024
  Alignment (bits) of base address:              2048
  Minimum alignment (bytes) for any datatype:    128
  Single precision floating point capability
    Denorms:                                     No
    Quiet NaNs:                                  Yes
    Round to nearest even:                       Yes
    Round to zero:                               Yes
    Round to +ve and infinity:                   Yes
    IEEE754-2008 fused multiply-add:             Yes
  Cache type:                                    Read/Write
  Cache line size:                               64
  Cache size:                                    16384
  Global memory size:                            12980584448
  Constant buffer size:                          10577156505
  Max number of constant args:                   8
  Local memory type:                             Scratchpad
  Local memory size:                             32768
  Max pipe arguments:                            16
  Max pipe active reservations:                  16
  Max pipe packet size:                          1987221913
  Max global variable size:                      9519440640
  Max global variable preferred total size:      12980584448
  Max read/write image args:                     64
  Max on device events:                          1024
  Queue on device max size:                      8388608
  Max on device queues:                          1
  Queue on device preferred size:                262144
  SVM capabilities:
    Coarse grain buffer:                         Yes
    Fine grain buffer:                           Yes
    Fine grain system:                           No
    Atomics:                                     No
  Preferred platform atomic alignment:           0
  Preferred global atomic alignment:             0
  Preferred local atomic alignment:              0
  Kernel Preferred work group size multiple:     64
  Error correction support:                      0
  Unified memory for Host and Device:            1
  Profiling timer resolution:                    1
  Device endianess:                              Little
  Available:                                     Yes
  Compiler available:                            Yes
  Execution capabilities:
    Execute OpenCL kernels:                      Yes
    Execute native function:                     No
  Queue on Host properties:
    Out-of-Order:                                No
    Profiling :                                  Yes
  Queue on Device properties:
    Out-of-Order:                                Yes
    Profiling :                                  Yes
  Platform ID:                                   00007FFAF7332000
  Name:                                          gfx90c
  Vendor:                                        Advanced Micro Devices, Inc.
  Device OpenCL C version:                       OpenCL C 2.0
  Driver version:                                3516.0 (PAL,HSAIL)
  Profile:                                       FULL_PROFILE
  Version:                                       OpenCL 2.0 AMD-APP (3516.0)
  Extensions:                                    cl_khr_fp64 cl_amd_fp64 cl_khr_global_int32_base_atomics cl_khr_global_int32_extended_atomics cl_khr_local_int32_base_atomics cl_khr_local_int32_extended_atomics cl_khr_int64_base_atomics cl_khr_int64_extended_atomics cl_khr_3d_image_writes cl_khr_byte_addressable_store cl_khr_fp16 cl_khr_gl_sharing cl_amd_device_attribute_query cl_amd_vec3 cl_amd_printf cl_amd_media_ops cl_amd_media_ops2 cl_amd_popcnt cl_khr_d3d10_sharing cl_khr_d3d11_sharing cl_khr_dx9_media_sharing cl_khr_image2d_from_buffer cl_khr_subgroups cl_khr_gl_event cl_khr_depth_images cl_khr_mipmap_image cl_khr_mipmap_image_writes cl_amd_liquid_flash cl_amd_copy_buffer_p2p cl_amd_planar_yuv


  Platform Name:                                 NVIDIA CUDA
Number of devices:                               1
  Device Type:                                   CL_DEVICE_TYPE_GPU
  Vendor ID:                                     10deh
  Max compute units:                             16
  Max work items dimensions:                     3
    Max work items[0]:                           1024
    Max work items[1]:                           1024
    Max work items[2]:                           64
  Max work group size:                           1024
  Preferred vector width char:                   1
  Preferred vector width short:                  1
  Preferred vector width int:                    1
  Preferred vector width long:                   1
  Preferred vector width float:                  1
  Preferred vector width double:                 1
  Native vector width char:                      1
  Native vector width short:                     1
  Native vector width int:                       1
  Native vector width long:                      1
  Native vector width float:                     1
  Native vector width double:                    1
  Max clock frequency:                           1500Mhz
  Address bits:                                  64
  Max memory allocation:                         1073610752
  Image support:                                 Yes
  Max number of images read arguments:           256
  Max number of images write arguments:          32
  Max image 2D width:                            32768
  Max image 2D height:                           32768
  Max image 3D width:                            16384
  Max image 3D height:                           16384
  Max image 3D depth:                            16384
  Max samplers within kernel:                    32
  Max size of kernel argument:                   4352
  Alignment (bits) of base address:              4096
  Minimum alignment (bytes) for any datatype:    128
  Single precision floating point capability
    Denorms:                                     Yes
    Quiet NaNs:                                  Yes
    Round to nearest even:                       Yes
    Round to zero:                               Yes
    Round to +ve and infinity:                   Yes
    IEEE754-2008 fused multiply-add:             Yes
  Cache type:                                    Read/Write
  Cache line size:                               128
  Cache size:                                    458752
  Global memory size:                            4294443008
  Constant buffer size:                          65536
  Max number of constant args:                   9
  Local memory type:                             Scratchpad
  Local memory size:                             49152
  Max pipe arguments:                            0
  Max pipe active reservations:                  0
  Max pipe packet size:                          0
  Max global variable size:                      0
  Max global variable preferred total size:      0
  Max read/write image args:                     0
  Max on device events:                          0
  Queue on device max size:                      0
  Max on device queues:                          0
  Queue on device preferred size:                0
  SVM capabilities:
    Coarse grain buffer:                         Yes
    Fine grain buffer:                           No
    Fine grain system:                           No
    Atomics:                                     No
  Preferred platform atomic alignment:           0
  Preferred global atomic alignment:             0
  Preferred local atomic alignment:              0
  Kernel Preferred work group size multiple:     32
  Error correction support:                      0
  Unified memory for Host and Device:            0
  Profiling timer resolution:                    1000
  Device endianess:                              Little
  Available:                                     Yes
  Compiler available:                            Yes
  Execution capabilities:
    Execute OpenCL kernels:                      Yes
    Execute native function:                     No
  Queue on Host properties:
    Out-of-Order:                                Yes
    Profiling :                                  Yes
  Queue on Device properties:
    Out-of-Order:                                No
    Profiling :                                  No
  Platform ID:                                   0000018ACCBDA330
  Name:                                          NVIDIA GeForce RTX 3050 Laptop GPU
  Vendor:                                        NVIDIA Corporation
  Device OpenCL C version:                       OpenCL C 1.2
  Driver version:                                527.56
  Profile:                                       FULL_PROFILE
  Version:                                       OpenCL 3.0 CUDA
  Extensions:                                    cl_khr_global_int32_base_atomics cl_khr_global_int32_extended_atomics cl_khr_local_int32_base_atomics cl_khr_local_int32_extended_atomics cl_khr_fp64 cl_khr_3d_image_writes cl_khr_byte_addressable_store cl_khr_icd cl_khr_gl_sharing cl_nv_compiler_options cl_nv_device_attribute_query cl_nv_pragma_unroll cl_nv_d3d10_sharing cl_khr_d3d10_sharing cl_nv_d3d11_sharing cl_nv_copy_opts cl_nv_create_buffer cl_khr_int64_base_atomics cl_khr_int64_extended_atomics cl_khr_device_uuid cl_khr_pci_bus_info cl_khr_external_semaphore cl_khr_external_memory cl_khr_external_semaphore_win32 cl_khr_external_memory_win32


  Platform Name:                                 Intel(R) OpenCL
Number of devices:                               1
  Device Type:                                   CL_DEVICE_TYPE_CPU
  Vendor ID:                                     8086h
  Max compute units:                             16
  Max work items dimensions:                     3
    Max work items[0]:                           8192
    Max work items[1]:                           8192
    Max work items[2]:                           8192
  Max work group size:                           8192
  Preferred vector width char:                   1
  Preferred vector width short:                  1
  Preferred vector width int:                    1
  Preferred vector width long:                   1
  Preferred vector width float:                  1
  Preferred vector width double:                 1
  Native vector width char:                      32
  Native vector width short:                     16
  Native vector width int:                       8
  Native vector width long:                      4
  Native vector width float:                     8
  Native vector width double:                    4
  Max clock frequency:                           0Mhz
  Address bits:                                  64
  Max memory allocation:                         16860727296
  Image support:                                 Yes
  Max number of images read arguments:           480
  Max number of images write arguments:          480
  Max image 2D width:                            16384
  Max image 2D height:                           16384
  Max image 3D width:                            2048
  Max image 3D height:                           2048
  Max image 3D depth:                            2048
  Max samplers within kernel:                    480
  Max size of kernel argument:                   3840
  Alignment (bits) of base address:              1024
  Minimum alignment (bytes) for any datatype:    128
  Single precision floating point capability
    Denorms:                                     Yes
    Quiet NaNs:                                  Yes
    Round to nearest even:                       Yes
    Round to zero:                               No
    Round to +ve and infinity:                   No
    IEEE754-2008 fused multiply-add:             No
  Cache type:                                    Read/Write
  Cache line size:                               64
  Cache size:                                    524288
  Global memory size:                            33721454592
  Constant buffer size:                          131072
  Max number of constant args:                   480
  Local memory type:                             Global
  Local memory size:                             32768
  Max pipe arguments:                            16
  Max pipe active reservations:                  16383
  Max pipe packet size:                          1024
  Max global variable size:                      65536
  Max global variable preferred total size:      65536
  Max read/write image args:                     480
  Max on device events:                          4294967295
  Queue on device max size:                      4294967295
  Max on device queues:                          4294967295
  Queue on device preferred size:                4294967295
  SVM capabilities:
    Coarse grain buffer:                         Yes
    Fine grain buffer:                           Yes
    Fine grain system:                           Yes
    Atomics:                                     Yes
  Preferred platform atomic alignment:           64
  Preferred global atomic alignment:             64
  Preferred local atomic alignment:              0
  Kernel Preferred work group size multiple:     128
  Error correction support:                      0
  Unified memory for Host and Device:            1
  Profiling timer resolution:                    100
  Device endianess:                              Little
  Available:                                     Yes
  Compiler available:                            Yes
  Execution capabilities:
    Execute OpenCL kernels:                      Yes
    Execute native function:                     Yes
  Queue on Host properties:
    Out-of-Order:                                Yes
    Profiling :                                  Yes
  Queue on Device properties:
    Out-of-Order:                                Yes
    Profiling :                                  Yes
  Platform ID:                                   0000018ACD13F7A0
  Name:                                          AMD Ryzen 7 5800HS with Radeon Graphics
  Vendor:                                        Intel(R) Corporation
  Device OpenCL C version:                       OpenCL C 3.0
  Driver version:                                2023.15.3.0.20_160000
  Profile:                                       FULL_PROFILE
  Version:                                       OpenCL 3.0 (Build 0)
  Extensions:                                    cl_khr_spirv_linkonce_odr cl_khr_fp64 cl_khr_icd cl_khr_global_int32_base_atomics cl_khr_global_int32_extended_atomics cl_khr_local_int32_base_atomics cl_khr_local_int32_extended_atomics cl_khr_int64_base_atomics cl_khr_int64_extended_atomics cl_khr_byte_addressable_store cl_khr_depth_images cl_khr_3d_image_writes cl_khr_il_program cl_intel_unified_shared_memory_preview cl_intel_device_attribute_query cl_intel_subgroups cl_intel_subgroups_char cl_intel_subgroups_short cl_intel_subgroups_long cl_intel_spirv_subgroups cl_intel_required_subgroup_size cl_intel_exec_by_local_thread cl_intel_vec_len_hint cl_khr_spir cl_khr_image2d_from_buffer
```