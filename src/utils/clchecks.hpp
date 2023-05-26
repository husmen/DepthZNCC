#pragma once

#ifdef USE_OCL

#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

string get_cl_err(cl_int errCode);
void print_platform_info(cl_platform_id platform);
void print_device_info(cl_device_id device);
int clHelloWorld();
int clVecAdd();

#endif // USE_OCL