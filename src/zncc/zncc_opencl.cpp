#include "zncc_opencl.hpp"

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
    cout << "# Running " << kernel_name << " on " << deviceName << endl;

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

tuple<cl::Buffer, cl::Buffer, cl::Buffer> configure_buffers(cl::Context context, cl::CommandQueue queue, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, size_t inputSize)
{
    // Create OpenCL memory buffers
    cl::Buffer leftImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
    cl::Buffer rightImgBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, inputSize, NULL, NULL);
    cl::Buffer dispMapBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, inputSize, NULL, NULL);

    // Copy the input data to the input buffers
    queue.enqueueWriteBuffer(leftImgBuffer, CL_TRUE, 0, inputSize, &leftImg[0]);
    queue.enqueueWriteBuffer(rightImgBuffer, CL_TRUE, 0, inputSize, &rightImg[0]);

    return make_tuple(leftImgBuffer, rightImgBuffer, dispMapBuffer);
}


void zncc_opencl(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams, bool reverse)
{
    try
    {
        size_t inputSize = sizeof(unsigned char) * leftImg.size();
        auto [context, queue, program] = configure_opencl("zncc_kernels_naive.cl", znccParams.platformId);
        auto [leftImgBuffer, rightImgBuffer, dispMapBuffer] = configure_buffers(context, queue, leftImg, rightImg, inputSize);

        // Create the kernel
        cl::Kernel zncc_kernel(program, "zncc_kernel");

        // Set the kernel arguments
        zncc_kernel.setArg(0, leftImgBuffer);
        zncc_kernel.setArg(1, rightImgBuffer);
        zncc_kernel.setArg(2, dispMapBuffer);
        zncc_kernel.setArg(3, znccParams.width);
        zncc_kernel.setArg(4, znccParams.height);
        zncc_kernel.setArg(5, znccParams.winSize);
        zncc_kernel.setArg(6, reverse ? - znccParams.maxDisp : znccParams.maxDisp);

        // Execute the kernel
        cl::NDRange global(znccParams.width * znccParams.height);
        // cl::NDRange local(WIN_SIZE * WIN_SIZE);
        queue.enqueueNDRangeKernel(zncc_kernel, cl::NullRange, global);
        queue.finish();

        // Copy the output data back to the host
        // unsigned char *tmpDisparity;
        queue.enqueueReadBuffer(dispMapBuffer, CL_TRUE, 0, inputSize, &dispMap[0]);
    }
    catch (cl::Error error)
    {
        cout << error.what() << ": " << get_cl_err(error.err()) << endl;
    }
}

void zncc_opencl_opt1(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    try
    {
        size_t inputSize = sizeof(unsigned char) * leftImg.size();
        auto [context, queue, program] = configure_opencl("zncc_kernels_opt1.cl", znccParams.platformId);
        auto [leftImgBuffer, rightImgBuffer, dispMapBuffer] = configure_buffers(context, queue, leftImg, rightImg, inputSize);

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
        queue.finish();

        // Copy the output data back to the host
        // unsigned char *tmpDisparity;
        queue.enqueueReadBuffer(dispMapBuffer, CL_TRUE, 0, inputSize, &dispMap[0]);
    }
    catch (cl::Error error)
    {
        cout << error.what() << ": " << get_cl_err(error.err()) << endl;
    }
}


void zncc_opencl_opt(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams, bool reverse)
{
    try
    {
        size_t inputSize = sizeof(unsigned char) * leftImg.size();
        auto [context, queue, program] = configure_opencl("zncc_kernels_opt2.cl", znccParams.platformId);
        auto [leftImgBuffer, rightImgBuffer, dispMapBuffer] = configure_buffers(context, queue, leftImg, rightImg, inputSize);

        // Create the kernel
        cl::Kernel zncc_kernel(program, "zncc_kernel");

        // Set the kernel arguments
        zncc_kernel.setArg(0, leftImgBuffer);
        zncc_kernel.setArg(1, rightImgBuffer);
        zncc_kernel.setArg(2, dispMapBuffer);
        zncc_kernel.setArg(3, znccParams.width);
        zncc_kernel.setArg(4, znccParams.height);
        zncc_kernel.setArg(5, znccParams.winSize);
        zncc_kernel.setArg(6, reverse ? - znccParams.maxDisp : znccParams.maxDisp);

        // Execute the kernel
        cl::NDRange global(znccParams.width * znccParams.height);
        // cl::NDRange local(WIN_SIZE * WIN_SIZE);
        queue.enqueueNDRangeKernel(zncc_kernel, cl::NullRange, global);
        queue.finish();

        // Copy the output data back to the host
        // unsigned char *tmpDisparity;
        queue.enqueueReadBuffer(dispMapBuffer, CL_TRUE, 0, inputSize, &dispMap[0]);
    }
    catch (cl::Error error)
    {
        cout << error.what() << ": " << get_cl_err(error.err()) << endl;
    }
}

void zncc_opencl_opt3(vector<unsigned char> &leftDispMap, vector<unsigned char> &rightDispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    try
    {
        size_t inputSize = sizeof(unsigned char) * leftImg.size();
        auto [context, queue, program] = configure_opencl("zncc_kernels_opt3.cl", znccParams.platformId);
        auto [leftImgBuffer, rightImgBuffer, leftDispMapBuffer] = configure_buffers(context, queue, leftImg, rightImg, inputSize);

        cl::Buffer rightDispMapBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, inputSize, NULL, NULL);

        // Create the kernel
        cl::Kernel zncc_kernel(program, "zncc_kernel");

        // Set the kernel arguments
        zncc_kernel.setArg(0, leftImgBuffer);
        zncc_kernel.setArg(1, rightImgBuffer);
        zncc_kernel.setArg(2, leftDispMapBuffer);
        zncc_kernel.setArg(3, rightDispMapBuffer);
        zncc_kernel.setArg(4, znccParams.width);
        zncc_kernel.setArg(5, znccParams.height);
        zncc_kernel.setArg(6, znccParams.winSize);
        zncc_kernel.setArg(7, znccParams.maxDisp);

        // Execute the kernel
        cl::NDRange global(znccParams.width * znccParams.height);
        // cl::NDRange local(WIN_SIZE * WIN_SIZE);
        queue.enqueueNDRangeKernel(zncc_kernel, cl::NullRange, global);
        queue.finish();

        // Copy the output data back to the host
        // unsigned char *tmpDisparity;
        queue.enqueueReadBuffer(leftDispMapBuffer, CL_TRUE, 0, inputSize, &leftDispMap[0]);
        queue.enqueueReadBuffer(rightDispMapBuffer, CL_TRUE, 0, inputSize, &rightDispMap[0]);
    }
    catch (cl::Error error)
    {
        cout << error.what() << ": " << get_cl_err(error.err()) << endl;
    }
}


// void zncc_opencl_pipe(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
// {
//     try
//     {
//         size_t inputSize = sizeof(unsigned char) * leftImg.size();
//         auto [context, queue, program] = configure_opencl("zncc_kernels_pipe.cl", znccParams.platformId);
//     auto [leftImgBuffer, rightImgBuffer, dispMapBuffer] = configure_buffers(context, queue, leftImg, rightImg, inputSize);

//         // size_t intermediateSize = sizeof(float) * znccParams.maxDisp;
//         // cl::Buffer meanValsBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, intermediateSize, NULL, NULL);
//         // cl::Buffer znccValsBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, intermediateSize, NULL, NULL);

//         // cl::Pipe pipe(context, CL_MEM_READ_WRITE, sizeof(float), znccParams.maxDisp);


//         // Create the kernels and their arguments
//         cl::Kernel calculate_mean(program, "calculate_mean");
//         calculate_mean.setArg(0, rightImgBuffer);
//         calculate_mean.setArg(1, znccParams.width);
//         calculate_mean.setArg(2, znccParams.height);
//         calculate_mean.setArg(3, znccParams.winSize);
//         calculate_mean.setArg(4, znccParams.maxDisp);

//         cl::Kernel calculate_zncc(program, "calculate_zncc");
//         calculate_zncc.setArg(0, leftImgBuffer);
//         calculate_zncc.setArg(1, rightImgBuffer);
//         calculate_zncc.setArg(2, znccParams.width);
//         calculate_zncc.setArg(3, znccParams.height);
//         calculate_zncc.setArg(4, znccParams.winSize);
//         calculate_zncc.setArg(5, znccParams.maxDisp);

//         cl::Kernel zncc_kernel(program, "zncc_kernel");
//         zncc_kernel.setArg(0, dispMapBuffer);
//         zncc_kernel.setArg(1, znccParams.width);
//         zncc_kernel.setArg(2, znccParams.maxDisp);

//         // Execute the kernel
//         cl::NDRange global(znccParams.height);

//         // vector<int> max_local = {256, 1024, 8192};
//         // cl::NDRange local(max_local[znccParams.platformId]);
//         // while (global[0] % local[0] != 0)
//         // {
//         //     local = local[0] - 1;
//         // }
//         // cout << "work group size: " << local[0] << endl;

//         queue.enqueueNDRangeKernel(calculate_mean, cl::NullRange, global);
//         queue.enqueueNDRangeKernel(calculate_zncc, cl::NullRange, global);
//         queue.enqueueNDRangeKernel(zncc_kernel, cl::NullRange, global);
//         // queue.finish();

//         // Copy the output data back to the host
//         // unsigned char *tmpDisparity;
//         queue.enqueueReadBuffer(dispMapBuffer, CL_TRUE, 0, inputSize, &dispMap[0]);
//     }
//     catch (cl::Error error)
//     {
//         cout << error.what() << ": " << get_cl_err(error.err()) << endl;
//     }
// }

#endif
