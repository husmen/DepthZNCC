#include "clchecks.hpp"

string get_cl_err(cl_int errCode)
{
    ostringstream os;

    if (errCode == CL_SUCCESS)
        os << "CL_SUCCESS (" << errCode << ")";
    else if (errCode == CL_INVALID_VALUE)
        os << "CL_INVALID_VALUE (" << errCode << ")";
    else if (errCode == CL_OUT_OF_HOST_MEMORY)
        os << "CL_OUT_OF_HOST_MEMORY (" << errCode << ")";
    else if (errCode == CL_PLATFORM_NOT_FOUND_KHR)
        os << "CL_PLATFORM_NOT_FOUND_KHR (" << errCode << ")";
    else if (errCode == CL_INVALID_KERNEL_NAME)
        os << "CL_INVALID_KERNEL_NAME (" << errCode << ")";
    else
        os << "UNKNOWN_ERROR (" << errCode << ")";

    return os.str();
}

int clHelloWorld()
{
        cout << "HelloWorld! (" <<  CL_SUCCESS << ")" << endl;

    try
    {
        cl_uint num_platforms;
        cl_platform_id *platforms = new cl_platform_id[1];

        cl_int err = clGetPlatformIDs(1, platforms, &num_platforms);
        cout << get_cl_err(err) << " Got platforms! " << int(num_platforms) << endl;
    }
    catch (cl::Error error)
    {
        cout << error.what() << ": " << get_cl_err(error.err()) << endl;
    }

    return 0;
}

int clVecAdd()
{
    // Heterogeneous computing with OpenCL

    const int elements = 2048;
    size_t datasize = sizeof(int) * elements;

    int *A = new int[elements];
    int *B = new int[elements];
    int *C = new int[elements];

    for (int i = 0; i < elements; i++)
    {
        A[i] = i;
        B[i] = i;
    }

    try
    {
        // Query for platforms
        vector <cl::Platform> platforms;
        cl::Platform::get(&platforms);

        // Get a list of devices on this platform
        vector<cl::Device> devices;
        platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);

        // Create a context for the devices
        cl::Context context(devices);

        // Create a command−queue for the first device
        cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

        // Create the memory buffers
        cl::Buffer bufferA = cl::Buffer(context, CL_MEM_READ_ONLY, datasize);
        cl::Buffer bufferB = cl::Buffer(context, CL_MEM_READ_ONLY, datasize);
        cl::Buffer bufferC = cl::Buffer(context, CL_MEM_WRITE_ONLY, datasize);

        // Copy the input data to the input buffers using the
        // command−queue for the first device
        queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, datasize, A);
        queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, datasize, B);

        // Read the program source
        ifstream sourceFile("kernels/vector_add_kernel.cl");
        string sourceCode(istreambuf_iterator<char>(sourceFile), (istreambuf_iterator<char>()));
        cl::Program::Sources source(1, make_pair(sourceCode.c_str(), sourceCode.length() + 1));

        // Create the program from the source code
        cl::Program program = cl::Program(context, source);

        // Build the program for the devices
        program.build(devices);

        // Create the kernel
        cl::Kernel vecadd_kernel(program, "vecadd");

        // Set the kernel arguments
        vecadd_kernel.setArg(0, bufferA);
        vecadd_kernel.setArg(1, bufferB);
        vecadd_kernel.setArg(2, bufferC);

        // Execute the kernel
        cl::NDRange global(elements);
        cl::NDRange local(256);
        queue.enqueueNDRangeKernel(vecadd_kernel, cl::NullRange, global, local);

        // Copy the output data back to the host
        queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, datasize, C);
    }
    catch (cl::Error error)
    {
        cout << error.what() << ": " << get_cl_err(error.err()) << endl;
    }

    return 0;
}