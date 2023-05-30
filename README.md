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
| CUDA | 2.2 | - | - |
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

Full device specs are available [here](clinfo.md)

## TODO
- [x] ZNCC single-threaded.
- [X] ZNCC multi-threaded.
- [X] ZNCC OpenMP.
- [X] ZNCC SIMD.
- [X] ZNCC OpenCL.
- [X] ZNCC OpenCL optimization.
- [X] ZNCC CUDA.
- [X] Benchmarking all implementations.
- [ ] Advanced profiling (Orbit?)
- [ ] Unit tests (optional).
- [ ] Automatic data downloader (optional).
- [ ] Switch from CUDA implementation to Khronos OpenCL SDK with ICD Loader (optional).

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
- **Day 13**: Trying to figure out the write way to compile and link cuda files suing CMake.
- **Day 14**: 
    - CUDA implementation.
    - ~~OpenCL pipes, failed.~~
- **Day 15**: Code cleanup and report.

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
