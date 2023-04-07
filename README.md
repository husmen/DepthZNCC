# DepthZNCC
OpenCL implementation of Zero-mean Normalized Cross Correlation (ZNCC) - Project for 521288S Multiprocessor Programming, Spring 2023, @UniOulu

## Notes
- [Middlebury Stereo Datasets](https://vision.middlebury.edu/stereo/data/)
- Project structure mixes guidlines from [Canonical Project Structure](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html) and [pitchfork](https://github.com/vector-of-bool/pitchfork)

## How to

## Requirements
- C++17
- LodePNG
- OpenMP
- OpenCL

## Benchmarks:

| Method ⬇️\Resolution ➡️ | 2940x2016 | 1470x1008 | 735x504 |
|-----|-----|-----|-----|
| Single-Threaded | 348 | - | - |
| Multi-Threaded | 50.7 | - | - |
| OpenMP | 51.8 | - | - |
| OpenCL | - | - | - |
| OpenCL Optimized | - | - | - |
| CUDA | - | - | - |
[Benchmark results as runtime in seconds]

## openCL Info
```
Platform summary:
Name: NVIDIA CUDA
Vendor: NVIDIA Corporation
Version: OpenCL 3.0 CUDA 12.0.94

Device summary:
Name: NVIDIA GeForce RTX 3050 Laptop GPU
Max compute units: 16
Global memory size (bytes): 4294443008
Max work group size: 1024
```

## TODO
- [x] ZNCC single-threaded.
- [X] ZNCC multi-threaded.
- [X] ZNCC OpenMP.
- [ ] ZNCC OpenCL.
- [ ] ZNCC OpenCL optimization.
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
- **Day 7**:
