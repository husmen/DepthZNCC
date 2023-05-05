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

// OpenMP ZNCC
void zncc_openmp(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    const int num_threads = omp_get_max_threads();
    const int chunk_size = znccParams.height / num_threads;

#pragma omp parallel for schedule(dynamic)
    for (int t = 0; t < num_threads; t++)
    {
        const int start_row = t * chunk_size;
        const int end_row = (t == num_threads - 1) ? znccParams.height : (t + 1) * chunk_size;

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

            // TODO: Better logging for OpenMP and MultiThreading progress
            // if (idx > 0 && idx % (width * 10) == 0)
            // {
            //     #pragma omp critical
            //     {
            //         cout << "Progress " << fixed << setprecision(2) << (j + 1) / (double)height * 100 << " %, " << j + 1 << "/" << height << " rows done!" << endl;
            //     }
            // }
        }
    }
}

// OpenCL ZNCC
void zncc_opencl(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    try
    {
        // Query for platforms
        vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        // Get a list of devices on this platform
        vector<cl::Device> devices;
        platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);

        // Create a context for the devices
        cl::Context context(devices);

        // Create a command−queue for the first device
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

        // Create OpenCL memory buffers
        size_t inputSize = sizeof(unsigned char) * leftImg.size();
        cl::Buffer leftImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
        cl::Buffer rightImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
        cl::Buffer dispMapBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, inputSize, NULL, NULL);

        // Copy the input data to the input buffers
        queue.enqueueWriteBuffer(leftImgBuffer, CL_TRUE, 0, inputSize, &leftImg[0]);
        queue.enqueueWriteBuffer(rightImgBuffer, CL_TRUE, 0, inputSize, &rightImg[0]);

        // Read the program source
        ifstream sourceFile("kernels/zncc_kernels.cl");
        string sourceCode(istreambuf_iterator<char>(sourceFile), (istreambuf_iterator<char>()));
        cl::Program::Sources source(1, make_pair(sourceCode.c_str(), sourceCode.length() + 1));

        // Create the program from the source code
        cl::Program program = cl::Program(context, source);

        // Build the program for the devices
        auto err = program.build(devices);
        cout << get_cl_err(err) << endl;

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
    case ZnccMethod::OPENCL:
        zncc_opencl(dispMap, leftImg, rightImg, znccParams);
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