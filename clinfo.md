## Device specs

```bash
> clinfo.exe
```

```yaml
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