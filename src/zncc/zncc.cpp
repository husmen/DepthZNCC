#include "zncc.hpp"

mutex cout_mutex;

// Single threaded ZNCC
void zncc_single(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    const int numPixels = znccParams.width * znccParams.height;

    for (int idx = 0; idx < numPixels; idx++)
    {
        int x = idx % znccParams.width;
        int y = idx / znccParams.width;

        double maxZncc = -1.0;
        int bestDisp = 0;

        double mean1 = calculateMean(x, y, 0, leftImg, znccParams);

        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            double mean2 = calculateMean(x, y, d, rightImg, znccParams);

            double znccVal = calculateZncc(x, y, d, mean1, mean2, leftImg, rightImg, znccParams);

            if (znccVal > maxZncc)
            {
                maxZncc = znccVal;
                bestDisp = d;
            }
        }

        dispMap[idx] = static_cast<unsigned char>(bestDisp);

        if (idx > 0 && idx % (znccParams.width * 100) == 0)
        {
            cout << "Progress " << fixed << setprecision(2) << (idx + 1) / static_cast<double>(numPixels) * 100 << " %, " << idx + 1 << "/" << numPixels << " pixels done!" << endl;
        }
    }

    // cout << "ZNCC min,max = " << (int)*min_element(disparityImg.begin(), disparityImg.end()) << "," << (int)*max_element(disparityImg.begin(), disparityImg.end()) << endl;
}

// Multi threaded ZNCC
void zncc_multi(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    const int num_threads = thread::hardware_concurrency();
    const int chunk_size = znccParams.height / num_threads;

    vector<thread> threads(num_threads);

    for (int t = 0; t < num_threads; t++)
    {
        const int start_row = t * chunk_size;
        const int end_row = (t == num_threads - 1) ? znccParams.height : (t + 1) * chunk_size;

        threads[t] = thread([=, &leftImg, &rightImg, &dispMap]()
                            {
            for (int idx = start_row * znccParams.width; idx < end_row * znccParams.width; idx++) 
            {
                int j = idx / znccParams.width;
                int i = idx % znccParams.width;

                double maxZncc = -1.0;
                int bestDisp = 0;

                double mean1 = calculateMean(i, j, 0, leftImg, znccParams);

                for (int d = 0; d < znccParams.maxDisp; d++)
                {
                    double mean2 = calculateMean(i, j, d, rightImg, znccParams);

                    double znccVal = calculateZncc(i, j, d, mean1, mean2, leftImg, rightImg, znccParams);

                    if (znccVal > maxZncc)
                    {
                        maxZncc = znccVal;
                        bestDisp = d;
                    }
                }

                dispMap[idx] = static_cast<unsigned char>(bestDisp);
            } });
    }

    for (int t = 0; t < num_threads; t++)
    {
        threads[t].join();
    }
}


void zncc_openmp(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    const int numPixels = znccParams.width * znccParams.height;

#pragma omp parallel for schedule(dynamic)
    for (int idx = 0; idx < numPixels; idx++)
    {
        int x = idx % znccParams.width;
        int y = idx / znccParams.width;

        double maxZncc = -1.0;
        int bestDisp = 0;

        double mean1 = calculateMean(x, y, 0, leftImg, znccParams);

        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            double mean2 = calculateMean(x, y, d, rightImg, znccParams);

            double znccVal = calculateZncc(x, y, d, mean1, mean2, leftImg, rightImg, znccParams);

            if (znccVal > maxZncc)
            {
                maxZncc = znccVal;
                bestDisp = d;
            }
        }

        dispMap[idx] = static_cast<unsigned char>(bestDisp);
    }
}

#ifdef USE_SIMD
void zncc_simd(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    const int numPixels = znccParams.width * znccParams.height;

#pragma omp parallel for
    for (int idx = 0; idx < numPixels; idx++)
    {
        int x = idx % znccParams.width;
        int y = idx / znccParams.width;

        double maxZncc = -1.0;
        int bestDisp = 0;

        auto meanVals = vector<double>(znccParams.maxDisp);
        auto znccVals = vector<double>(znccParams.maxDisp);

// #pragma omp parallel for simd
        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            meanVals[d] = calculateMeanSimd(x, y, d, znccParams.width, znccParams.height, znccParams.winSize / 2, rightImg);
        }

// #pragma omp parallel for simd
        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            znccVals[d] = calculateZnccSimd(x, y, d, meanVals[0], meanVals[d], znccParams.width, znccParams.height, znccParams.winSize / 2, leftImg, rightImg);
        }

// #pragma omp parallel for simd
        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            if (znccVals[d] > maxZncc)
            {
                maxZncc = znccVals[d];
                bestDisp = d;
            }
        }

        dispMap[idx] = static_cast<unsigned char>(bestDisp);
    }
}

#else
void zncc_simd(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    cout << "SIMD not enabled" << endl;
}
#endif

// OpenCL ZNCC
#ifdef USE_OCL

tuple<cl::Context, cl::CommandQueue, cl::Program> configure_opencl(const char* kernel_name, int platform_id)
{
    // Query for platforms
    vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // Get a list of devices on this platform
    vector<cl::Device> devices;
    platforms[platform_id].getDevices(CL_DEVICE_TYPE_ALL, &devices);
    auto deviceName = devices[0].getInfo<CL_DEVICE_NAME>();
    cout << "# Running OPENCL_2 on " << deviceName << endl;

    // Create a context for the devices
    cl::Context context(devices);

    // Create a command−queue for the first device
    cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

    // Read the program source
    ifstream sourceFile("kernels/" + string(kernel_name));
    string sourceCode(istreambuf_iterator<char>(sourceFile), (istreambuf_iterator<char>()));
    cl::Program::Sources source(1, make_pair(sourceCode.c_str(), sourceCode.length() + 1));

    // Create the program from the source code
    cl::Program program = cl::Program(context, source);

    // Build the program for the devices
    auto err = program.build(devices);
    cout << get_cl_err(err) << endl;

    return make_tuple(context, queue, program);
}


void zncc_opencl_1(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    try
    {
        auto [context, queue, program] = configure_opencl("zncc_kernels_1.cl", znccParams.platformId);

        // Create OpenCL memory buffers
        size_t inputSize = sizeof(unsigned char) * leftImg.size();
        cl::Buffer leftImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
        cl::Buffer rightImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
        cl::Buffer dispMapBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, inputSize, NULL, NULL);

        // Copy the input data to the input buffers
        queue.enqueueWriteBuffer(leftImgBuffer, CL_TRUE, 0, inputSize, &leftImg[0]);
        queue.enqueueWriteBuffer(rightImgBuffer, CL_TRUE, 0, inputSize, &rightImg[0]);

        // Create the kernel
        cl::Kernel zncc_kernel(program, "zncc_kernel");

        // Set the kernel arguments
        zncc_kernel.setArg(0, leftImgBuffer);
        zncc_kernel.setArg(1, rightImgBuffer);
        zncc_kernel.setArg(2, dispMapBuffer);
        zncc_kernel.setArg(3, znccParams.width);
        zncc_kernel.setArg(4, znccParams.height);

        // Execute the kernel
        cl::NDRange global(znccParams.width * znccParams.height);
        // cl::NDRange local(WIN_SIZE * WIN_SIZE);
        queue.enqueueNDRangeKernel(zncc_kernel, cl::NullRange, global);
        // queue.finish();

        // Copy the output data back to the host
        // unsigned char *tmpDisparity;
        queue.enqueueReadBuffer(dispMapBuffer, CL_TRUE, 0, inputSize, &dispMap[0]);
    }
    catch (cl::Error error)
    {
        cout << error.what() << ": " << get_cl_err(error.err()) << endl;
    }
}

void zncc_opencl_2(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    try
    {
        auto [context, queue, program] = configure_opencl("zncc_kernels_2.cl", znccParams.platformId);

        // Create OpenCL memory buffers
        size_t inputSize = sizeof(unsigned char) * leftImg.size();
        cl::Buffer leftImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
        cl::Buffer rightImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
        cl::Buffer dispMapBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, inputSize, NULL, NULL);

        size_t intermediateSize = sizeof(float) * znccParams.maxDisp;
        cl::Buffer meanValsBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, intermediateSize, NULL, NULL);
        cl::Buffer znccValsBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, intermediateSize, NULL, NULL);

        // Copy the input data to the input buffers
        queue.enqueueWriteBuffer(leftImgBuffer, CL_TRUE, 0, inputSize, &leftImg[0]);
        queue.enqueueWriteBuffer(rightImgBuffer, CL_TRUE, 0, inputSize, &rightImg[0]);

        // Create the kernel
        cl::Kernel zncc_kernel(program, "zncc_kernel");

        // Set the kernel arguments
        zncc_kernel.setArg(0, leftImgBuffer);
        zncc_kernel.setArg(1, rightImgBuffer);
        zncc_kernel.setArg(2, dispMapBuffer);
        zncc_kernel.setArg(3, meanValsBuffer);
        zncc_kernel.setArg(4, znccValsBuffer);
        zncc_kernel.setArg(5, znccParams.width);
        zncc_kernel.setArg(6, znccParams.height);
        zncc_kernel.setArg(7, znccParams.winSize);
        zncc_kernel.setArg(8, znccParams.maxDisp);

        // Execute the kernel
        cl::NDRange global(znccParams.width * znccParams.height);
        // cl::NDRange local(WIN_SIZE * WIN_SIZE);
        queue.enqueueNDRangeKernel(zncc_kernel, cl::NullRange, global);
        // queue.finish();

        // Copy the output data back to the host
        // unsigned char *tmpDisparity;
        queue.enqueueReadBuffer(dispMapBuffer, CL_TRUE, 0, inputSize, &dispMap[0]);
    }
    catch (cl::Error error)
    {
        cout << error.what() << ": " << get_cl_err(error.err()) << endl;
    }
}

void zncc_opencl_3(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    try
    {
        auto [context, queue, program] = configure_opencl("zncc_kernels_3.cl", znccParams.platformId);

        // Create OpenCL memory buffers
        size_t inputSize = sizeof(unsigned char) * leftImg.size();
        cl::Buffer leftImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
        cl::Buffer rightImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
        cl::Buffer dispMapBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, inputSize, NULL, NULL);

        // size_t intermediateSize = sizeof(float) * znccParams.maxDisp;
        // cl::Buffer meanValsBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, intermediateSize, NULL, NULL);
        // cl::Buffer znccValsBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, intermediateSize, NULL, NULL);

        // Copy the input data to the input buffers
        queue.enqueueWriteBuffer(leftImgBuffer, CL_TRUE, 0, inputSize, &leftImg[0]);
        queue.enqueueWriteBuffer(rightImgBuffer, CL_TRUE, 0, inputSize, &rightImg[0]);

        // cl::Pipe pipe(context, CL_MEM_READ_WRITE, sizeof(float), znccParams.maxDisp);


        // Create the kernels and their arguments
        cl::Kernel calculate_mean(program, "calculate_mean");
        calculate_mean.setArg(0, rightImgBuffer);
        calculate_mean.setArg(1, znccParams.width);
        calculate_mean.setArg(2, znccParams.height);
        calculate_mean.setArg(3, znccParams.winSize);
        calculate_mean.setArg(4, znccParams.maxDisp);

        cl::Kernel calculate_zncc(program, "calculate_zncc");
        calculate_zncc.setArg(0, leftImgBuffer);
        calculate_zncc.setArg(1, rightImgBuffer);
        calculate_zncc.setArg(2, znccParams.width);
        calculate_zncc.setArg(3, znccParams.height);
        calculate_zncc.setArg(4, znccParams.winSize);
        calculate_zncc.setArg(5, znccParams.maxDisp);

        cl::Kernel zncc_kernel(program, "zncc_kernel");
        zncc_kernel.setArg(0, dispMapBuffer);
        zncc_kernel.setArg(1, znccParams.width);
        zncc_kernel.setArg(2, znccParams.maxDisp);

        // Execute the kernel
        cl::NDRange global(znccParams.height);

        // vector<int> max_local = {256, 1024, 8192};
        // cl::NDRange local(max_local[znccParams.platformId]);
        // while (global[0] % local[0] != 0)
        // {
        //     local = local[0] - 1;
        // }
        // cout << "work group size: " << local[0] << endl;

        queue.enqueueNDRangeKernel(calculate_mean, cl::NullRange, global);
        queue.enqueueNDRangeKernel(calculate_zncc, cl::NullRange, global);
        queue.enqueueNDRangeKernel(zncc_kernel, cl::NullRange, global);
        // queue.finish();

        // Copy the output data back to the host
        // unsigned char *tmpDisparity;
        queue.enqueueReadBuffer(dispMapBuffer, CL_TRUE, 0, inputSize, &dispMap[0]);
    }
    catch (cl::Error error)
    {
        cout << error.what() << ": " << get_cl_err(error.err()) << endl;
    }
}

#else

void zncc_opencl(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    cout << "OpenCL not enabled" << endl;
}
#endif

void zncc_cuda(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    #ifdef USE_CUDA
    zncc_cuda_wrapper(&dispMap[0], &leftImg[0], &rightImg[0], znccParams.width, znccParams.height, znccParams.winSize, znccParams.maxDisp);
    #else
    cout << "# CUDA not enabled" << endl;
    #endif
}

// ZNCC wrapper function
void zncc(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    switch (znccParams.method)
    {
    case ZnccMethod::SINGLE_THREADED:
        zncc_single(dispMap, leftImg, rightImg, znccParams);
        break;
    case ZnccMethod::MULTI_THREADED:
        zncc_multi(dispMap, leftImg, rightImg, znccParams);
        break;
    case ZnccMethod::OPENMP:
        zncc_openmp(dispMap, leftImg, rightImg, znccParams);
        break;
    case ZnccMethod::SIMD:
        zncc_simd(dispMap, leftImg, rightImg, znccParams);
        break;
    case ZnccMethod::OPENCL_1:
        zncc_opencl_1(dispMap, leftImg, rightImg, znccParams);
        break;
    case ZnccMethod::OPENCL_2:
        zncc_opencl_2(dispMap, leftImg, rightImg, znccParams);
        break;
    case ZnccMethod::OPENCL_3:
        zncc_opencl_3(dispMap, leftImg, rightImg, znccParams);
        break;
    case ZnccMethod::CUDA:
        zncc_cuda(dispMap, leftImg, rightImg, znccParams);
        break;
    }
}

// ZNCC pipeline
ZnccResult zncc_pipeline(const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    int numPixels = znccParams.width * znccParams.height;
    ZnccResult znccResult;
    znccResult.dispMapLeft = vector<unsigned char>(numPixels);
    znccResult.dispMapRight = vector<unsigned char>(numPixels);


    // Compute the disparity map using ZNCC
    cout << "# ZNCC Left\n";
    {
        Timer timer;

        zncc(znccResult.dispMapLeft, leftImg, rightImg, znccParams);
        
    }

    if (znccParams.withRight || znccParams.withCrossChecking)
    {
        cout << "# ZNCC right\n";
        zncc(znccResult.dispMapRight, rightImg, leftImg, znccParams);
    }

    // apply cross checking if enabled
    znccResult.dispMapCC = znccParams.withCrossChecking ? crosscheck(znccResult.dispMapLeft, znccResult.dispMapRight, znccParams) : znccResult.dispMapLeft;

    // apply occlusion filling if enabled
    znccResult.dispMapOC = znccParams.withOcclusionFilling ? fillOcclusion(znccResult.dispMapCC, znccParams) : znccResult.dispMapCC;

    // Normalize the disparity map
    znccResult.dispMap = znccParams.withNormalization ? normalizeMap(znccResult.dispMapOC, znccParams) : znccResult.dispMapOC;

    return znccResult;
}