# DepthZNCC
OpenCL implementation of Zero-mean Normalized Cross Correlation (ZNCC) - Project for 521288S Multiprocessor Programming, Spring 2023, @UniOulu

## Notes
- Project structure mixes guidlines from [Canonical Project Structure](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html) and [pitchfork](https://github.com/vector-of-bool/pitchfork)
- 

## TODO
- [ ] ZNCC single threaded.
- [ ] ZNCC multithreaded.
- [ ] ZNCC OpenCL.
- [ ] ZNCC OpenCL Optimization.
- [ ] ZNCC CUDA (optional).
- [ ] Benchmarking all implementations.

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
