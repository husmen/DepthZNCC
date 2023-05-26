#ifdef USE_OCL

#include "clchecks.hpp"

string get_cl_err(cl_int errCode)
{
    ostringstream os;

    if (errCode == CL_SUCCESS)
        os << "CL_SUCCESS (" << errCode << ")";
    else if (errCode == CL_MEM_OBJECT_ALLOCATION_FAILURE)
        os << "CL_MEM_OBJECT_ALLOCATION_FAILURE (" << errCode << ")";
    else if (errCode == CL_OUT_OF_RESOURCES)
        os << "CL_OUT_OF_RESOURCES (" << errCode << ")";
    else if (errCode == CL_OUT_OF_HOST_MEMORY)
        os << "CL_OUT_OF_HOST_MEMORY (" << errCode << ")";
    else if (errCode == CL_BUILD_PROGRAM_FAILURE)
        os << "CL_BUILD_PROGRAM_FAILURE (" << errCode << ")";
    else if (errCode == CL_INVALID_VALUE)
        os << "CL_INVALID_VALUE (" << errCode << ")";
    else if (errCode == CL_PLATFORM_NOT_FOUND_KHR)
        os << "CL_PLATFORM_NOT_FOUND_KHR (" << errCode << ")";
    else if (errCode == CL_INVALID_COMMAND_QUEUE)
        os << "CL_INVALID_COMMAND_QUEUE (" << errCode << ")";
    else if (errCode == CL_INVALID_KERNEL_NAME)
        os << "CL_INVALID_KERNEL_NAME (" << errCode << ")";
    else if (errCode == CL_INVALID_WORK_GROUP_SIZE)
        os << "CL_INVALID_WORK_GROUP_SIZE (" << errCode << ")";
    else if (errCode == CL_INVALID_ARG_INDEX)
        os << "CL_INVALID_ARG_INDEX (" << errCode << ")";
    else if (errCode == CL_INVALID_BUFFER_SIZE)
        os << "CL_INVALID_BUFFER_SIZE (" << errCode << ")";
    else
        os << "UNKNOWN_ERROR (" << errCode << ")";

    return os.str();
}

void print_platform_info(cl_platform_id platform)
{
    char platform_name[1024];
    clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(platform_name), &platform_name, NULL);

    char platform_vendor[1024];
    clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, sizeof(platform_vendor), &platform_vendor, NULL);

    char platform_version[1024];
    clGetPlatformInfo(platform, CL_PLATFORM_VERSION, sizeof(platform_version), &platform_version, NULL);

    // Print the information to the console or a log file
    cout << "Platform summary:" << endl;
    cout << "Name: " << platform_name << endl;
    cout << "Vendor: " << platform_vendor << endl;
    cout << "Version: " << platform_version << endl << endl;
}

void print_device_info(cl_device_id device)
{
    char device_name[1024];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name), &device_name, NULL);

    cl_uint max_compute_units;
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units), &max_compute_units, NULL);

    cl_ulong global_mem_size;
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(global_mem_size), &global_mem_size, NULL);

    size_t max_work_group_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL);

    size_t kernel_work_group_size;
    clGetDeviceInfo(device, CL_KERNEL_WORK_GROUP_SIZE , sizeof(kernel_work_group_size), &kernel_work_group_size, NULL);

    // Print the information to the console or a log file
    cout << "Device summary:" << endl;
    cout << "Name: " << device_name << endl;
    cout << "Max compute units: " << max_compute_units << endl;
    cout << "Global memory size (bytes): " << (unsigned long long)global_mem_size << endl;
    cout << "Max work group size: " << max_work_group_size << endl;
    cout << "Kernel work group size: " << kernel_work_group_size << endl << endl;
}

int clHelloWorld()
{
    cout << "HelloWorld! (" <<  CL_SUCCESS << ")" << endl;

    try
    {
        cl_uint num_platforms;
        cl_platform_id *platforms = new cl_platform_id[3];
        cl_int err = clGetPlatformIDs(3, platforms, &num_platforms);
        cout << get_cl_err(err) << " Platforms found: " << int(num_platforms) << endl;

        for (int i = 0; i < num_platforms; i++)
        {
            print_platform_info(platforms[i]);
            
            cl_uint num_devices;
            cl_device_id *devices = new cl_device_id[3];
            err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 3, devices, &num_devices);
            cout << get_cl_err(err) << " Devices found: " << int(num_devices) << endl;

            for (int j = 0; j < num_devices; j++)
                print_device_info(devices[j]);
        }

        
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

#endif